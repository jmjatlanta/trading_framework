#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>
#include <functional>
#include <domain/Contract.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/accounting/AccountingService.hpp>
#include <domain/Order.hpp>
#include <domain/Event.hpp>

namespace tf
{

/***
 * @brief Basics of a strategy. Strategies themselves will probably be based on this class
 */
template<class HistoricalService>
class Strategy
{

   public:
   Strategy() {}
   /*****
    * @brief fired when an event happens
    * @param e the event
    * @returns the result
    */
   virtual tf::EvaluationResult OnPretradeEvent(tf::Event e ) = 0;
   /****
    * @brief fired when an order is created
    * @param e the event
    * @param order the order
    * @return the result
    */
   virtual tf::EvaluationResult OnCreateOrder(tf::Event e, tf::Order& order) = 0;
   /****
    * @brief fired when an order is sent to the broker
    * @param e the event
    * @param order the order
    * @param the result
    */
   virtual tf::EvaluationResult OnOrderSent(tf::Event e, const tf::Order& order) = 0;
   /*****
    * @brief fired when the order is partially filled
    * @param e the event
    * @param order the order
    * @return the result
    */
   virtual tf::EvaluationResult OnOrderPartiallyFilled(tf::Event e, const tf::Order& order) = 0;
   /*****
    * @brief fired with the order is filled completely
    * @param e the event
    * @param order the order
    * @return the result
    */
   virtual tf::EvaluationResult OnOrderFilled(tf::Event e, const tf::Order& order) = 0;
   /****
    * @brief fired when the order has been canceled
    * @param e the event
    * @param order the order
    * @return the result
    */
   virtual tf::EvaluationResult OnOrderCanceled(tf::Event e, const tf::Order& order) = 0;

   /****
    * @brief called to make this object subscribe to an event
    * @param eventType the event type
    * @param contract the security to listen for
    */
   void SubscribeToEvent(tf::EventType eventType, tf::Contract contract)
   {
      eventContractPairs.push_back( { eventType, contract} );
   }

   /***
    * The collection of events and contracts that are being listened to
    */
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

} // namespace tf