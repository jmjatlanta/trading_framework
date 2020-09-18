#include <algorithm>
#include <fstream>
#include <backtest/BacktestStreamingService.hpp>
#include <market_data/BarSettings.hpp>

namespace backtest
{

BacktestStreamingService::BacktestStreamingService(const BacktestingConfiguration& config)
{
   // set up the test
}

/***
 * Subscribe to tick data
 * @param contract the contract to subscribe to
 * @param func the callback function to call each time data comes in
 */
void BacktestStreamingService::GetTimeAndSales(tf::Contract contract, std::function<tf::EvaluationResult(market_data::TickMessage)> func)
{
   // flip through the file, firing events for every line
   std::ifstream file("hello.txt");
   while(!file.eof())
   {
      market_data::TickMessage msg(market_data::TickType::LAST, 1.0, 1, "Blah");
      func(msg);
   }
}

void BacktestStreamingService::GetBookData(tf::Contract contract, std::function<void(market_data::BookMessage)> func)
{
   // do nothing
}

} // namespace ib