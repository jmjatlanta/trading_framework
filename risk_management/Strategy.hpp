#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>
#include <functional>
#include <market_data/Contract.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/accounting/AccountingService.hpp>
#include <risk_management/Order.hpp>
#include <risk_management/Event.hpp>

namespace strategy
{

/***
 * @brief Basics of a strategy. Strategies themselves will probably be based on this class
 */
template<class HistoricalService>
class Strategy
{

   public:
   Strategy() {}
   virtual strategy::EvaluationResult OnPretradeEvent(strategy::Event e ) = 0;
   virtual strategy::EvaluationResult OnCreateOrder(strategy::Event e, tf::Order& order) = 0;
   virtual strategy::EvaluationResult OnOrderSent(strategy::Event e, const tf::Order& order) = 0;
   virtual strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e, const tf::Order& order) = 0;
   virtual strategy::EvaluationResult OnOrderFilled(strategy::Event e, const tf::Order& order) = 0;
   virtual strategy::EvaluationResult OnOrderCanceled(strategy::Event e, const tf::Order& order) = 0;
   void SubscribeToEvent(strategy::EventType eventType, tf::Contract contract)
   {
      eventContractPairs.push_back( { eventType, contract} );
   }
   std::vector<std::pair<EventType, tf::Contract>> eventContractPairs;
   HistoricalService historicalService;
};

/***
 * @brief This runs strategies.
 * It was originally designed to handle multiple strategies. Now I don't think that is needed. So far,
 * each strategy is its own executable, with the accounting service being a separate network service
 */
template<class Derived, class StreamingSerivce, class AccountingService, class HistoricalService>
class StrategyRunner
{
   public:
   StrategyRunner() {}
   void AddStrategy(std::shared_ptr< Strategy<HistoricalService> > s)
   {
      strategies.push_back(s);
      /*
      // make sure that we are subscribed to the information we need for this strategy
      std::for_each(s->eventContractPairs.begin(), s->eventContractPairs.end(), [&](std::pair<EventType, tf::Contract> in) {
         switch(in.first)
         {
            case EventType::BID:
            case EventType::ASK:
            case EventType::LAST:
               streamingService.GetTimeAndSales(in.second, std::bind(s.get()->OnPretradeEvent, s.get());
               break;
            default:
               break;
         }
      });
      */
   }

   /**
    * This will continue to run until it receives orders to shut down
    */
   int run();
   void shutdown() { shutting_down = true; }
   // provides the minutes since market has been opened
   int32_t MarketOpenMinutes();
   protected:
   std::vector< std::shared_ptr<Strategy<HistoricalService>> > strategies;
   bool shutting_down = false;
   AccountingService accountingService;
   StreamingSerivce streamingService;
   HistoricalService historicalService;
};

} // namespace strategy