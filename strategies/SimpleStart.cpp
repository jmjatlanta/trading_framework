#include <domain/Strategy.hpp>
#include <domain/Contract.hpp>
#include <backtest/BacktestingConfiguration.hpp>
#include <backtest/BacktestHistoricalService.hpp>
#include <backtest/BacktestStrategyRunner.hpp>
#include <util/SysLogger.hpp>

/****
 * This strategy is just to test the system and demonstrate how to write a strategy
 * Q: Why the templating? A: to make it easy to switch services and do compile-time checks
 */
template<class HistoricalService>
class MACross : public tf::Strategy<HistoricalService>
{
   public:
   MACross(const tf::Contract& contract) : contract(contract)
   {
      // subscribe to necessary events for this strategy to work
      this->SubscribeToEvent( tf::EventType::LAST, contract );
      this->SubscribeToEvent( tf::EventType::MARKET_CLOSE, contract ); // Get out of trades as market is closing
   }
   /***
    * This is fired each time the market moves (i.e. a tick happens)
    */
   tf::EvaluationResult OnPretradeEvent(tf::Event e )
   {
      auto sma9 = historicalService.SMA(9, contract);
      auto sma20 = historicalService.SMA(20, contract);
      if ( sma9 && sma20 && !active_order && (sma9.value() > sma20.value() || sma20.value() > sma9.value() ))
         return tf::EvaluationResult::PASSES_EVALUATION; 
      return tf::EvaluationResult::FAILED_FOR_EVENT;
   }
   /**
    * This is fired if the OEM system wants to place an order on the market
    * TODO: A generic form of the order should be returned
    */
   tf::EvaluationResult OnCreateOrder(tf::Event e, tf::Order& order)
   {
      // prerequisites passed, build an order
      // pass order to OEM system
   }
   /***
    * This is fired if the OEM system sends an order to the market
    * TODO: A generic form of the order should be sent as a parameter. That
    * way we can track things like amounts
    */
   tf::EvaluationResult OnOrderSent(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnOrderPartiallyFilled(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnOrderFilled(tf::Event e, const tf::Order& order)
   {

   }
   tf::EvaluationResult OnOrderCanceled(tf::Event e, const tf::Order& order)
   {

   }
   /***
    * This could get sticky. LIFO or FIFO? Perhaps it is better to have the strategy calculate this
    */
   tf::EvaluationResult OnTradeClosed(tf::Event e)
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

