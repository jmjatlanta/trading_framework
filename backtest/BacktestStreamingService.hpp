#pragma once

#include <future>
#include <vector>

#include <backtest/BacktestingConfiguration.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/Contract.hpp>

namespace backtest
{

class BacktestStreamingService : public market_data::StreamingService<BacktestStreamingService>
{
   public:
   BacktestStreamingService(const BacktestingConfiguration& config);
   BacktestStreamingService() {}
   void GetTimeAndSales(tf::Contract contract, std::function<strategy::EvaluationResult(market_data::TickMessage)> func);
   void GetBookData(tf::Contract contract, std::function<void(market_data::BookMessage)> func);
   private:
   std::function<void(market_data::TickMessage)> tickFunc;
};

} // namespace ib