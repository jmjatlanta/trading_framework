#pragma once
#include <vector>
#include <future>
#include <map>
#include <list>
#include <market_data/historical/HistoricalService.hpp>

namespace backtest
{

class HistoricalStatistics
{
   public:
   HistoricalStatistics(){}
   void AddBar( const market_data::OHLCBar& bar )
   {
      if (bar.high > high)
         high = bar.high;
      if (low == 0 || bar.low < low)
         low = bar.low;
      if (open == 0 )
         open = bar.open;
      close = bar.close;
      volume += bar.volume;
   }
   std::list<market_data::OHLCBar> bars;
   double high = 0;
   double low = 0;
   double open = 0;
   double close = 0;
   uint64_t volume = 0;
};

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
   std::future<std::list<market_data::OHLCBar>> GetBars(const tf::Contract& contract, const tf::BarSettings& span);
   std::optional<double> LowOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract, bool market_hours = true)
         { return contract_stats[contract].low; }
   std::optional<double> HighOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract, bool market_hours = true)
         { return contract_stats[contract].high; }
   std::optional<double> OpeningGapPct(tf::Contract contract) { return std::optional<double>(); }
   std::optional<double> PreviousDayClose(std::chrono::time_point<std::chrono::system_clock> today, tf::Contract contract)
         { return std::optional<double>(); }
   std::optional<double> LastTradePrice(tf::Contract contract)
         { return contract_stats[contract].close; }
   std::optional<double> SMA(uint16_t numBars, tf::Contract contract, bool market_hours = true)
         { return std::nullopt; }
   std::optional<double> LastBidPrice(tf::Contract contract)
         { return contract_stats[contract].close - 0.01; }
   std::optional<double> LastAskPrice(tf::Contract contract)
         { return contract_stats[contract].close + 0.01; }

   void AddBar(const tf::Contract& contract, market_data::OHLCBar bar)
   {
         auto& stats = contract_stats[contract];
         stats.AddBar(bar);
   }
   private:
   std::list<market_data::OHLCBar> get_bars(const tf::Contract& contract, const tf::BarSettings& span);
   std::map<tf::Contract, HistoricalStatistics> contract_stats;
};

} // namespace backtest
