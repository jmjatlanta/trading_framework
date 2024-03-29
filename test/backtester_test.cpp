#include <gtest/gtest.h>

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

TEST(backtester_test, accounting_test )
{
   backtest::BacktestAccountingService acct_svc;
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 24 2020 13:00:00" )); // a Sunday, 9AM Eastern
   EXPECT_FALSE( acct_svc.MarketOpen( tf::Market::NASDAQ ) );
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 25 2020 13:00:00" )); // a Monday pre-market
   EXPECT_FALSE( acct_svc.MarketOpen( tf::Market::NASDAQ ) );
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 25 2020 13:30:00" )); // a Monday at market open
   EXPECT_TRUE( acct_svc.MarketOpen( tf::Market::NASDAQ ) );
   acct_svc.AdvanceTime( convert_string_to_time_point( "May 25 2020 20:00:01" )); // a Monday at market close
   EXPECT_FALSE( acct_svc.MarketOpen( tf::Market::NASDAQ ) );
}

TEST(backtester_test, history_test )
{
   backtest::BacktestHistoricalService hist_svc;
   // create some bars
   tf::Stock contract("MSFT");
   for(int i = 0; i < 100; i++)
   {
      market_data::OHLCBar bar(std::to_string(i), 1, 2, 0.5, 0.75, 3);
      hist_svc.AddBar(contract, bar);
   }
   EXPECT_EQ(2.0, hist_svc.HighOfDay(std::chrono::system_clock::now(), contract).value() );
   EXPECT_EQ(0.5, hist_svc.LowOfDay(std::chrono::system_clock::now(), contract).value() );
}

TEST(backtester_test, strategy_runner_test )
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
   class CrazyStrategy : public tf::Strategy<backtest::BacktestHistoricalService>
   {
      public:
      CrazyStrategy() {}
      tf::EvaluationResult OnPretradeEvent(tf::Event e )
      {
         if (e.eventType == tf::EventType::MARKET_CLOSE)
         {
            // market is closing
            if (current_shares != 0)
               return tf::EvaluationResult::PASSES_EVALUATION;
            return tf::FAILED_FOR_DAY;
         }
         // is this the first time through?
         if (previous_event.price == 0)
         {
            previous_event = e;
            return tf::EvaluationResult::FAILED_FOR_EVENT;
         }
         if (current_shares > 0)
         {
            // only allow short
            if (previous_event.price > e.price) // ticked down
               return tf::EvaluationResult::PASSES_EVALUATION;
         }
         if (current_shares < 0)
         {
            // only allow long
            if (previous_event.price < e.price) // ticked up
               return tf::EvaluationResult::PASSES_EVALUATION;
         }
         if (current_shares == 0 && previous_event.price != e.price) // allow either
            return tf::EvaluationResult::PASSES_EVALUATION;
         return tf::EvaluationResult::FAILED_FOR_EVENT;
      }
      tf::EvaluationResult OnCreateOrder(tf::Event e, tf::Order& order)
      {
         if (e.eventType == tf::EventType::MARKET_CLOSE)
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
         return tf::EvaluationResult::PASSES_EVALUATION;
      }
      tf::EvaluationResult OnOrderSent(tf::Event e, const tf::Order& order)
      {  
         return tf::EvaluationResult::PASSES_EVALUATION;
      }
      tf::EvaluationResult OnOrderPartiallyFilled(tf::Event e, const tf::Order& order)
      {
         if (order.going_long)
            current_shares += order.num_shares;
         else
            current_shares -= order.num_shares;
         return tf::EvaluationResult::PASSES_EVALUATION;
      }
      tf::EvaluationResult OnOrderFilled(tf::Event e, const tf::Order& order)
      {
         if (order.going_long)
            current_shares += order.num_shares;
         else
            current_shares -= order.num_shares;         
         return tf::EvaluationResult::PASSES_EVALUATION;
      }
      tf::EvaluationResult OnOrderCanceled(tf::Event e, const tf::Order& order)
      {
         return tf::EvaluationResult::PASSES_EVALUATION;
      }
      tf::Event previous_event;
      long current_shares = 0;
   };

   backtest::BacktestStrategyRunner runner(config);
   std::shared_ptr<CrazyStrategy> crazyStrategy = std::make_shared<CrazyStrategy>();
   runner.AddStrategy( crazyStrategy );
   runner.start();
   // check results
   EXPECT_EQ(runner.get_result().trades.size(), 52);
   EXPECT_EQ(runner.get_result().shares_traded_long, 2600);
   EXPECT_EQ(runner.get_result().shares_traded_short, 2600);
   EXPECT_EQ(runner.get_result().max_car, 18323.0);
   EXPECT_EQ(runner.get_result().available_capital, 99970.0 );

}
