#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iomanip>
#include <backtest/BacktestAccountingService.hpp>
#include <backtest/BacktestHistoricalService.hpp>

/***
 * @brief convert string to system_clock::time_point
 * @param in time in format like "May 20 2020 09:00:00"
 * @returns a time_point
 */
std::chrono::system_clock::time_point convert_string_to_time_point( std::string in )
{
   std::tm tm = {};
   std::stringstream ss(in); // a Sunday
   ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
   return std::chrono::system_clock::from_time_t( std::mktime(&tm) );
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

BOOST_AUTO_TEST_SUITE_END()