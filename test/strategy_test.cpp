#define BOOST_TEST_MODULE trading_framework
#include <boost/test/included/unit_test.hpp>

#include <ib/IBWrapper.h>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <ib/IBAccountService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/BarSettings.hpp>
#include <market_data/Contract.hpp>
#include <risk_management/Order.hpp>

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
   Scalp1(tf::Contract contract) : strategy::Strategy()
   {
      subscribe_event(contract, strategy::EventType::LAST);
   }
   strategy::EvaluationResult OnPretradeEvent(strategy::Event e )
   {
      prerequisite_or_wait(market_status(Market::Open)); // the market is open (just as a reminder I should build this func)
      prerequisite_or_wait(market_open_minutes(30)); // 10am NY time
      prerequisite_or_fail(today_opening_gap_pct(contract) > 0.1); // this contract gapped up by at least 0.1%
      prerequisite_or_fail(today_green_to_red(false)); // this contract has never gone below yesterday's close (since open)
      prerequisite(retrace(today_high(), current_last()) > 0.1); // this contract has traded 0.1% below the high of the day after the high was created
      prerequisite(consolidation(4)); // the last 4 bars show consolidation (lower volatility, volume, narrowing range)
      prerequisite(price_vs_bars(4) > 0) // the current price is higher than the last 4 bars
      prerequisite(volume_compare(4) > 0) // the volume of the current bar is more than the previous 3
      prerequisite(current_bid() - sma(9) < 0.02 && current_bid() - sma(9) > 0); // price touching sma(9)
      prerequisite(!price_history_below(sma(9), consolidation_point)); // since consolidation, price has not fallen below sma(9) 
      // if we got here, we've passed all the above tests
      CreateOrder(e);
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
   strategy::EvalutaiontResult OnTradeClosed(strategy::Event e)
   {
      
   }
}

BOOST_AUTO_TEST_SUITE ( strategy )

BOOST_AUTO_TEST_CASE ( opening_gap )
{
   // this is the "main" system
   strategy::StrategyRunner strategyRunner;
   // run this strategy
   strategyRunner.AddStrategy(strategy::Scalp1(tf::Contract("MSFT")));

}

BOOST_AUTO_TEST_SUITE_END()