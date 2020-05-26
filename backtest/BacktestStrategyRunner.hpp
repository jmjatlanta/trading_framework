#pragma once
#include <risk_management/Strategy.hpp>
#include <backtest/BacktestAccountingService.hpp>
#include <backtest/BacktestHistoricalService.hpp>
#include <backtest/BacktestingConfiguration.hpp>
#include <backtest/BacktestStreamingService.hpp>

namespace backtest
{

class Trade
{
   public:
   bool buy = false; // TRUE = went long
   double limit_price = 0;
   long num_shares = 0;
   std::chrono::system_clock::time_point execution_time;
};

class BacktestStrategyResult
{
   public:
   long num_orders = 0;
   long num_trades = 0;
   long num_orders_canceled = 0;
   long shares_traded_long = 0;
   long shares_traded_short = 0;
   long current_num_shares = 0;
   double max_car = 0;
   double available_capital = 0.0;
   std::vector<Trade> trades;
};

class BacktestStrategyRunner : public strategy::StrategyRunner<BacktestStrategyRunner, 
      BacktestStreamingService, 
      BacktestAccountingService, 
      BacktestHistoricalService>
{
   public:
   BacktestStrategyRunner(const BacktestingConfiguration& config);
   int start();
   BacktestStrategyResult get_result() { return result; }
   private:
   /**
    * @brief Callback function to respond to incoming messages
    */
   void tick_func(const market_data::TickMessage& message);
   BacktestingConfiguration config;
   BacktestStrategyResult result;
   void process_event(std::shared_ptr<strategy::Strategy<BacktestHistoricalService>> strategy, const strategy::Event& e);
};

} // namespace backtest