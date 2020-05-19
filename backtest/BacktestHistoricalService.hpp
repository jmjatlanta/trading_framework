#pragma once
#include <vector>
#include <future>
#include <market_data/historical/HistoricalService.hpp>

namespace backtest
{

/***
 * A mock historical service to backtest strategies
 * This should keep track of the Accounting Services's
 * current time, and provide a status of the market based
 * on that time
 */
class BacktestHistoricalService : public market_data::HistoricalService<BacktestHistoricalService>
{
   public:
   /****
    * @brief fetch bars for a particular contract
    * @param contract the contract
    * @param span the parameters for what to get
    * @returns a future that will be the vector of bars
    */
   std::future<std::vector<market_data::OHLCBar>> GetBars(const tf::Contract& contract, const tf::BarSettings& span);
   private:
   std::vector<market_data::OHLCBar> get_bars(const tf::Contract& contract, const tf::BarSettings& span);
};

} // namespace backtest
