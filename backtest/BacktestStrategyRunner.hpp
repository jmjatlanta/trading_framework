#pragma once
#include <risk_management/Strategy.hpp>
#include <backtest/BacktestAccountingService.hpp>
#include <backtest/BacktestingConfiguration.hpp>
#include <backtest/BacktestStreamingService.hpp>

namespace backtest
{


class BacktestStrategyRunner : public strategy::StrategyRunner<BacktestStrategyRunner, BacktestStreamingService, BacktestAccountingService>
{
   public:
   BacktestStrategyRunner(const BacktestingConfiguration& config);
   int start();
   private:
   /**
    * @brief Callback function to respond to incoming messages
    */
   void tick_func(const market_data::TickMessage& message);
   BacktestingConfiguration config;
};

} // namespace backtest