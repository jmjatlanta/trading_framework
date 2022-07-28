#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <ib/IBWrapper.h>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <ib/IBAccountService.hpp>
#include <backtest/BacktestingConfiguration.hpp>
#include <backtest/BacktestStrategyRunner.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/BarSettings.hpp>
#include <domain/Contract.hpp>
#include <domain/Order.hpp>
#include <domain/Strategy.hpp>

class MockHistoricalService : public market_data::HistoricalService<MockHistoricalService>
{
   public:
   std::future<std::vector<market_data::OHLCBar>> GetBars(tf::Contract cont, tf::BarSettings span)
   {
      return std::async(&MockHistoricalService::do_nothing, this, cont, span);
   }
   private:
   std::vector<market_data::OHLCBar> do_nothing(tf::Contract cont, tf::BarSettings span)
   {
      std::vector<market_data::OHLCBar> retVal;
      return retVal;
   }
};

template<class HistoricalService>
class Scalp1 : public tf::Strategy<HistoricalService>
{
   public:
   Scalp1(tf::Contract contract);
   tf::EvaluationResult OnPretradeEvent(tf::Event e );
   tf::EvaluationResult OnCreateOrder(tf::Event e, tf::Order& order)
   {
      // prerequisites passed, build an order
      // pass order to OEM system
   }
   tf::EvaluationResult OnOrderSent(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnOrderPartiallyFilled(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnOrderFilled(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnOrderCanceled(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnTradeClosed(tf::Event e, const tf::Order& order)
   {
      
   }
   private:
   std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
   tf::Contract contract;
   /***
    * What percentage retrace is this price?
    */
   double retrace(std::optional<double> highOfDay, std::optional<double> lastTradePrice) { return 0.0; }
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

template<class HistoricalService>
Scalp1<HistoricalService>::Scalp1(tf::Contract contract) : tf::Strategy<HistoricalService>(), contract(contract)
{
   this->SubscribeToEvent(tf::EventType::LAST, contract);
}
template<class HistoricalService>
tf::EvaluationResult Scalp1<HistoricalService>::OnPretradeEvent(tf::Event e )
{
   if (this->historicalService.OpeningGapPct(contract) > 0.1 
         || this->historicalService.LowOfDay(today, contract) > this->historicalService.PreviousDayClose(today, contract)) // gapped up by at least 0.1%, never went red
      return tf::EvaluationResult::FAILED_FOR_DAY;
   if ( this->retrace(this->historicalService.HighOfDay(today, contract), 
         this->historicalService.LastTradePrice(contract)) > 0.1 ) // this contract has traded 0.1% below the high of the day after the high was created
      return tf::EvaluationResult::FAILED_FOR_EVENT;    
   
   auto sma9 = this->historicalService.SMA(9, contract);
   auto last_bid = this->historicalService.LastBidPrice(contract);
   if ( sma9 && last_bid
         && this->consolidation(4) // the last 4 bars show consolidation (lower volatility, volume, narrowing range)
         && this->price_vs_bars(4) > 0 // the current price is higher than the last 4 bars
         && this->volume_compare(4) > 0 // the volume of the current bar is more than the previous 3
         && last_bid.value() - sma9.value() < 0.02 && last_bid.value() - sma9.value() > 0 // price touching sma(9)
         //&& !price_history_below(sma9, consolidation_point) // since consolidation, price has not fallen below sma(9)
         )
      return tf::EvaluationResult::PASSES_EVALUATION; 
   return tf::EvaluationResult::FAILED_FOR_EVENT;
}

/**
 * Capture events to CSV file
 */
template<class HistoricalService>
class DataCapture : public tf::Strategy<HistoricalService>
{
   public:
   DataCapture(tf::Contract contract)
   {
      this->SubscribeToEvent(tf::EventType::LAST, contract);
      this->SubscribeToEvent(tf::EventType::BID, contract);
      this->SubscribeToEvent(tf::EventType::ASK, contract);
      std::string fileName = contract.ticker + ".CSV";
      file.open(fileName);
   }
   tf::EvaluationResult OnPretradeEvent(tf::Event e )
   {
      // print time,event,value
      using namespace date;
      file << e.timestamp << "," << e.eventType << "," << e.price << std::endl;
      return tf::EvaluationResult::FAILED_FOR_EVENT;
   }
   tf::EvaluationResult OnCreateOrder(tf::Event e, tf::Order& order) { }
   tf::EvaluationResult OnOrderSent(tf::Event e, const tf::Order& order) { }
   tf::EvaluationResult OnOrderPartiallyFilled(tf::Event e, const tf::Order& order) { }
   tf::EvaluationResult OnOrderFilled(tf::Event e, const tf::Order& order) { }
   tf::EvaluationResult OnOrderCanceled(tf::Event e, const tf::Order& order) { }
   tf::EvaluationResult OnTradeClosed(tf::Event e, const tf::Order& order) { }
   private:
   std::ofstream file;
};

BOOST_AUTO_TEST_SUITE ( strategy )

BOOST_AUTO_TEST_CASE ( opening_gap )
{
   // this is the "main" system
   backtest::BacktestingConfiguration config;
   backtest::BacktestStrategyRunner strategyRunner(config);
   // run this strategy
   strategyRunner.AddStrategy( std::make_shared<Scalp1<backtest::BacktestHistoricalService>>(tf::Stock("MSFT")));
}

BOOST_AUTO_TEST_CASE ( data_capture )
{
   backtest::BacktestingConfiguration config;
   backtest::BacktestStrategyRunner strategyRunner(config);
   strategyRunner.AddStrategy( std::make_shared<DataCapture<backtest::BacktestHistoricalService>>(tf::Stock("AAPL")));
   //strategyRunner.AddStrategy(DataCapture(tf::Index("SPX")));
}

BOOST_AUTO_TEST_SUITE_END()