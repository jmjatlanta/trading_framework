#include <backtest/BacktestHistoricalService.hpp>

namespace backtest
{

/****
 * @brief fetch bars for a particular contract
 * @param contract the contract
 * @param span the parameters for what to get
 * @returns a future that will be the vector of bars
 */
std::future< std::list<market_data::OHLCBar> > BacktestHistoricalService::GetBars(const tf::Contract& contract, const tf::BarSettings& span)
{
   return std::async(&BacktestHistoricalService::get_bars, this, contract, span);
}

std::list<market_data::OHLCBar> BacktestHistoricalService::get_bars(const tf::Contract& contract, const tf::BarSettings& span)
{
   auto stats = contract_stats[contract];
   return stats.bars;
}

} // namespace backtest