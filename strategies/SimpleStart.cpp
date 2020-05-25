#include <risk_management/Strategy.hpp>
#include <market_data/Contract.hpp>
#include <backtest/BacktestingConfiguration.hpp>
#include <backtest/BacktestHistoricalService.hpp>
#include <backtest/BacktestStrategyRunner.hpp>
#include <util/SysLogger.hpp>

/****
 * This strategy is just to test the system and demonstrate how to write a strategy
 */
template<class HistoricalService>
class MACross : public strategy::Strategy<HistoricalService>
{
   public:
   MACross(const tf::Contract& contract) : contract(contract)
   {
      this->SubscribeToEvent( strategy::EventType::LAST, contract );
      this->SubscribeToEvent( strategy::EventType::MARKET_TIME, contract ); // Get out of trades as market is closing
   }
   /***
    * This is fired each time the market moves (i.e. a tick happens)
    */
   strategy::EvaluationResult OnPretradeEvent(strategy::Event e )
   {
      auto sma9 = historicalService.SMA(9, contract);
      auto sma20 = historicalService.SMA(20, contract);
      if ( sma9 && sma20 && !active_order && (sma9.value() > sma20.value() || sma20.value() > sma9.value() ))
         return strategy::EvaluationResult::PASSES_EVALUATION; 
      return strategy::EvaluationResult::FAILED_FOR_EVENT;
   }
   /**
    * This is fired if the OEM system wants to place an order on the market
    * TODO: A generic form of the order should be returned
    */
   strategy::EvaluationResult OnCreateOrder(strategy::Event e)
   {
      // prerequisites passed, build an order
      // pass order to OEM system
   }
   /***
    * This is fired if the OEM system sends an order to the market
    * TODO: A generic form of the order should be sent as a parameter. That
    * way we can track things like amounts
    */
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
   /***
    * This could get sticky. LIFO or FIFO?
    */
   strategy::EvaluationResult OnTradeClosed(strategy::Event e)
   {
      
   }   
   protected:
   HistoricalService historicalService;
   tf::Contract contract;
   bool active_order = false;
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
   auto strategy = std::make_shared<MACross<backtest::BacktestHistoricalService>>(contract);
   runner.AddStrategy( strategy );

   std::future<int> fut = std::async(std::launch::async, [&] { return runner.start(); } );

   fut.wait();

   return 0;
}

