#include <risk_management/Strategy.hpp>
#include <market_data/Contract.hpp>
#include <backtest/BacktestingConfiguration.hpp>
#include <backtest/BacktestStrategyRunner.hpp>
#include <util/SysLogger.hpp>

/****
 * This strategy is just to test the system and demonstrate how to write a strategy
 */

class MACross : public strategy::Strategy
{
   public:
   MACross(const tf::Contract& contract) : contract(contract)
   {
      SubscribeToEvent( strategy::EventType::LAST, contract );
      SubscribeToEvent( strategy::EventType::MARKET_TIME, contract ); // Get out of trades as market is closing
   }
   strategy::EvaluationResult OnPretradeEvent(strategy::Event e )
   {
      std::chrono::time_point<std::chrono::system_clock> today;
      bool not_long = false, not_short = false;

      double sma9 = SMA(9, contract);
      double sma20 = SMA(20, contract);
      if (sma9 > sma20 && not_long ||
         sma20 > sma9 && not_short )
         return strategy::EvaluationResult::PASSES_EVALUATION; 
      return strategy::EvaluationResult::FAILED_FOR_EVENT;
   }
   strategy::EvaluationResult OnCreateOrder(strategy::Event e)
   {
      // prerequisites passed, build an order
      // pass order to OEM system
   }
   strategy::EvaluationResult OnOrderSent(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnOrderFilled(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnOrderCanceled(strategy::Event e)
   {

   }
   strategy::EvaluationResult OnTradeClosed(strategy::Event e)
   {
      
   }   
   protected:
   tf::Contract contract;
};

int main(int argc, char** argv)
{
   util::SysLogger* logger = util::SysLogger::getInstance();
   logger->setToConsole(true);
   logger->setToSysLog(false);
   logger->setLogLevel(util::LogLevel::debug);

   backtest::BacktestingConfiguration config;
   config.fileName = "STOCK/IB/1MIN/GNUS.csv";
   backtest::BacktestStrategyRunner runner( config );
   tf::Contract contract;
   contract.ticker = "GNUS";
   contract.exchange = "NASDAQ";
   contract.currency = "USD";
   auto strategy = std::make_shared<MACross>(contract);
   runner.AddStrategy( strategy );

   std::future<int> fut = std::async(std::launch::async, [&] { return runner.start(); } );

   fut.wait();

   return 0;
}

