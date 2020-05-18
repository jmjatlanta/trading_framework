#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <ib/IBWrapper.h>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <ib/IBAccountService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/BarSettings.hpp>
#include <market_data/Contract.hpp>
#include <risk_management/Order.hpp>
#include <risk_management/Strategy.hpp>

class MockHistoricalService : public HistoricalService<MockHistoricalService>
{
   public:
   std::future<std::vector<OHLCBar>> GetBars(tf::Contract cont, tf::BarSettings span)
   {
      return std::async(&MockHistoricalService::do_nothing, this, cont, span);
   }
   private:
   std::vector<OHLCBar> do_nothing(tf::Contract cont, tf::BarSettings span)
   {
      std::vector<OHLCBar> retVal;
      return retVal;
   }
};

class Scalp1 : public strategy::Strategy
{
   public:
   Scalp1(tf::Contract contract) : strategy::Strategy(), contract(contract)
   {
      SubscribeToEvent(strategy::EventType::LAST, contract);
   }
   strategy::EvaluationResult OnPretradeEvent(strategy::Event e )
   {
      if ( MarketOpenMinutes() < 30 ) // market must have been open for at least 30 minutes
         return strategy::EvaluationResult::FAILED_FOR_EVENT;
      if (OpeningGapPct(contract) > 0.1 || LowOfDay(today, contract) > PreviousDayClose(today, contract)) // gapped up by at least 0.1%, never went red
         return strategy::EvaluationResult::FAILED_FOR_DAY;
      if ( retrace(HighOfDay(today, contract), LastTradePrice(contract)) > 0.1 ) // this contract has traded 0.1% below the high of the day after the high was created
         return strategy::EvaluationResult::FAILED_FOR_EVENT;    
      
      double sma9 = SMA(9, contract);
      if (consolidation(4) // the last 4 bars show consolidation (lower volatility, volume, narrowing range)
            && price_vs_bars(4) > 0 // the current price is higher than the last 4 bars
            && volume_compare(4) > 0 // the volume of the current bar is more than the previous 3
            && LastBidPrice(contract) - sma9 < 0.02 && LastBidPrice(contract) - sma9 > 0 // price touching sma(9)
            //&& !price_history_below(sma9, consolidation_point) // since consolidation, price has not fallen below sma(9)
            )
         return strategy::EvaluationResult::PASSES_EVALUATION; 
      return strategy::EvaluationResult::FAILED_FOR_EVENT;
   }
   strategy::EvaluationResult OnCreateOrder(strategy::Event e)
   {
      // prerequisites passed, build an order
      // pass order to OEM system
   }
   strategy::EvaluationResult OnOrderSent(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnOrderFilled(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnOrderCanceled(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnTradeClosed(strategy::Event e)
   {
      
   }
   private:
   std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
   tf::Contract contract;
   /***
    * What percentage retrace is this price?
    */
   double retrace(double highOfDay, double lastTradePrice) { return 0.0; }
   /**
    * Has the security been consolidating within that time?
    */
   bool consolidation(uint16_t numMinutes) { return false; }
   /***
    * Is the current price higher than the last n bars?
    */
   double price_vs_bars(uint16_t numBars) { return 0.0; }
   /****
    * What is this bar's volume compared to the previous n?
    */
   uint32_t volume_compare(uint16_t numBars) { return 1; }
};

/**
 * Capture events to CSV file
 */
class DataCapture : public strategy::Strategy
{
   public:
   DataCapture(tf::Contract contract)
   {
      SubscribeToEvent(strategy::EventType::LAST, contract);
      SubscribeToEvent(strategy::EventType::BID, contract);
      SubscribeToEvent(strategy::EventType::ASK, contract);
      std::string fileName = contract.ticker + ".CSV";
      file.open(fileName);
   }
   strategy::EvaluationResult OnPretradeEvent(strategy::Event e )
   {
      // print time,event,value
      file << e.timestamp << "," << e.eventType << "," << e.price << std::endl;
      return strategy::EvaluationResult::FAILED_FOR_EVENT;
   }
   strategy::EvaluationResult OnCreateOrder(strategy::Event e) { }
   strategy::EvaluationResult OnOrderSent(strategy::Event e) { }
   strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e) { }
   strategy::EvaluationResult OnOrderFilled(strategy::Event e) { }
   strategy::EvaluationResult OnOrderCanceled(strategy::Event e) { }
   strategy::EvaluationResult OnTradeClosed(strategy::Event e) { }
   private:
   std::ofstream file;
};

BOOST_AUTO_TEST_SUITE ( strategy )

BOOST_AUTO_TEST_CASE ( opening_gap )
{
   // this is the "main" system
   IBConfiguration config("127.0.0.1", 4007, 1);
   strategy::StrategyRunner strategyRunner(config, config, config);
   // run this strategy
   strategyRunner.AddStrategy( std::make_shared<Scalp1>(tf::Contract("MSFT")));
}

BOOST_AUTO_TEST_CASE ( data_capture )
{
   IBConfiguration config("127.0.0.1", 4007, 1);
   strategy::StrategyRunner strategyRunner(config, config, config);
   strategyRunner.AddStrategy( std::make_shared<DataCapture>(tf::Contract("AAPL")));
   //strategyRunner.AddStrategy(DataCapture(tf::Index("SPX")));
}

BOOST_AUTO_TEST_SUITE_END()