#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iomanip>
#include <vendor/date/include/date/tz.h>
#include <backtest/BacktestAccountingService.hpp>
#include <backtest/BacktestHistoricalService.hpp>
#include <backtest/BacktestStrategyRunner.hpp>

#define TEST_FILE_DIR std::string("/home/jmjatlanta/Development/cpp/trading_framework/STOCK/IB");

/***
 * @brief convert string to system_clock::time_point
 * @param in time in format like "May 20 2020 09:00:00"
 * @returns a time_point
 */
std::chrono::system_clock::time_point convert_string_to_time_point( std::string in )
{
   std::tm tm = {};
   std::stringstream ss(in); // a Sunday
   std::chrono::system_clock::time_point tp;
   ss >> date::parse("%Y-%m-%d %H:%M:%S", tp);
   return tp;
}

BOOST_AUTO_TEST_SUITE( backtester_test )

BOOST_AUTO_TEST_CASE( accounting_test )
{
   backtest::BacktestAccountingService acct_svc;
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 24 2020 13:00:00" )); // a Sunday, 9AM Eastern
   BOOST_CHECK( !acct_svc.MarketOpen( strategy::NASDAQ ) );
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 25 2020 13:00:00" )); // a Monday pre-market
   BOOST_CHECK( !acct_svc.MarketOpen( strategy::NASDAQ ) );
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 25 2020 13:30:00" )); // a Monday at market open
   BOOST_CHECK( acct_svc.MarketOpen( strategy::NASDAQ ) );
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 25 2020 20:00:01" )); // a Monday at market close
   BOOST_CHECK( !acct_svc.MarketOpen( strategy::NASDAQ ) );
}

BOOST_AUTO_TEST_CASE( history_test )
{
   backtest::BacktestHistoricalService hist_svc;
   // create some bars
   tf::Contract contract("MSFT");
   for(int i = 0; i < 100; i++)
   {
      market_data::OHLCBar bar(std::to_string(i), 1, 2, 0.5, 0.75, 3);
      hist_svc.AddBar(contract, bar);
   }
   BOOST_CHECK_EQUAL(2.0, hist_svc.HighOfDay(std::chrono::system_clock::now(), contract).value() );
   BOOST_CHECK_EQUAL(0.5, hist_svc.LowOfDay(std::chrono::system_clock::now(), contract).value() );
}

BOOST_AUTO_TEST_CASE( strategy_runner_test )
{

   // set up logging
   util::SysLogger* logger = util::SysLogger::getInstance();
   logger->setLogLevel(util::LogLevel::error);
   logger->setToConsole(true);
   logger->setToSysLog(false);

   backtest::BacktestingConfiguration config;
   config.fileName = TEST_FILE_DIR
   config.fileName += "/TICK/MSFT.csv";
   config.fileType = backtest::FileType::TICK;
   config.startingCapital = 100000.0;

   // strategy to buy on a down tick, sell on an up tick
   class CrazyStrategy : public strategy::Strategy<backtest::BacktestHistoricalService>
   {
      public:
      CrazyStrategy() {}
      strategy::EvaluationResult OnPretradeEvent(strategy::Event e )
      {
         if (e.eventType == strategy::EventType::MARKET_CLOSE)
         {
            // market is closing
            if (current_shares != 0)
               return strategy::EvaluationResult::PASSES_EVALUATION;
            return strategy::FAILED_FOR_DAY;
         }
         // is this the first time through?
         if (previous_event.price == 0)
         {
            previous_event = e;
            return strategy::EvaluationResult::FAILED_FOR_EVENT;
         }
         if (current_shares > 0)
         {
            // only allow short
            if (previous_event.price > e.price) // ticked down
               return strategy::EvaluationResult::PASSES_EVALUATION;
         }
         if (current_shares < 0)
         {
            // only allow long
            if (previous_event.price < e.price) // ticked up
               return strategy::EvaluationResult::PASSES_EVALUATION;
         }
         if (current_shares == 0 && previous_event.price != e.price) // allow either
            return strategy::EvaluationResult::PASSES_EVALUATION;
         return strategy::EvaluationResult::FAILED_FOR_EVENT;
      }
      strategy::EvaluationResult OnCreateOrder(strategy::Event e, tf::Order& order)
      {
         if (e.eventType == strategy::EventType::MARKET_CLOSE)
         {
            if (current_shares < 0)
            {
               order.going_long = true;
               order.limit_price = previous_event.price + 0.01;
               order.num_shares = current_shares * -1;
            }
            else
            {
               order.going_long = false;
               order.limit_price = previous_event.price - 0.01;
               order.num_shares = current_shares;
            }
         }
         else
         {
            order.going_long = e.price > previous_event.price;
            order.limit_price = e.price;
            order.num_shares = 100;
         }
         previous_event = e;
         return strategy::EvaluationResult::PASSES_EVALUATION;
      }
      strategy::EvaluationResult OnOrderSent(strategy::Event e, const tf::Order& order)
      {  
         return strategy::EvaluationResult::PASSES_EVALUATION;
      }
      strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e, const tf::Order& order)
      {
         if (order.going_long)
            current_shares += order.num_shares;
         else
            current_shares -= order.num_shares;
         return strategy::EvaluationResult::PASSES_EVALUATION;
      }
      strategy::EvaluationResult OnOrderFilled(strategy::Event e, const tf::Order& order)
      {
         if (order.going_long)
            current_shares += order.num_shares;
         else
            current_shares -= order.num_shares;         
         return strategy::EvaluationResult::PASSES_EVALUATION;
      }
      strategy::EvaluationResult OnOrderCanceled(strategy::Event e, const tf::Order& order)
      {
         return strategy::EvaluationResult::PASSES_EVALUATION;
      }
      strategy::Event previous_event;
      long current_shares = 0;
   };

   backtest::BacktestStrategyRunner runner(config);
   std::shared_ptr<CrazyStrategy> crazyStrategy = std::make_shared<CrazyStrategy>();
   runner.AddStrategy( crazyStrategy );
   runner.start();
   // check results
   BOOST_CHECK_EQUAL(runner.get_result().trades.size(), 43);
   BOOST_CHECK_EQUAL(runner.get_result().shares_traded_long, 2200);
   BOOST_CHECK_EQUAL(runner.get_result().shares_traded_short, 2100);
   BOOST_CHECK_EQUAL(runner.get_result().max_car, 18317.0);
   BOOST_CHECK_EQUAL(runner.get_result().available_capital, 100000.0 );

}

BOOST_AUTO_TEST_SUITE_END()