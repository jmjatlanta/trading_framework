#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>
#include <market_data/Contract.hpp>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <ib/IBAccountService.hpp>


namespace strategy
{

enum EvaluationResult
{
   FAILED_FOR_DAY,
   FAILED_FOR_EVENT,
   PASSES_EVALUATION
};

enum Market { NASDAQ, NYSE };

enum EventType { BID, ASK, LAST };

class Event
{
   public:
   Event() {}
   Event(std::string timestamp, EventType eventType, double price) : timestamp(timestamp), eventType(eventType), price(price) {} 
   std::string timestamp;
   EventType eventType;
   double price;

};

class MarketMonitor
{
   public:
   MarketMonitor( Market market );
   int32_t MarketOpenMinutes();
   double OpeningGapPct(tf::Contract contract);
   double LowOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract);
   double HighOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract);
   double PreviousDayClose(std::chrono::time_point<std::chrono::system_clock> today, tf::Contract contract);
   double LastTradePrice(tf::Contract contract);
   double LastBidPrice(tf::Contract contract);
   double LastAskPrice(tf::Contract contract);
   double SMA(uint16_t numBars, tf::Contract contract);
};

class Strategy : public MarketMonitor
{

   public:
   Strategy() : MarketMonitor( Market::NASDAQ )
   {

   }
   virtual strategy::EvaluationResult OnPretradeEvent(strategy::Event e ) = 0;
   virtual strategy::EvaluationResult OnCreateOrder(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderSent(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderFilled(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderCanceled(strategy::Event e) = 0;
   void SubscribeToEvent(EventType eventType, tf::Contract contract);
   std::vector<std::pair<EventType, tf::Contract>> eventContractPairs;
};

class StrategyRunner
{
   public:
   StrategyRunner();
   void AddStrategy(std::shared_ptr<Strategy> s);
   private:
   ib::IBAccountService accountService;
   ib::IBHistoricalService historicalService;
   ib::IBStreamingService streamingService;
   std::vector<std::shared_ptr<Strategy>> strategies;
};

} // namespace strategy