#pragma once

#include <string>

namespace strategy
{

enum EvaluationResult
{
   FAILED_FOR_DAY,
   FAILED_FOR_EVENT,
   PASSES_EVALUATION
};

enum Market { NASDAQ, NYSE };

enum EventType { BID, ASK, LAST, MARKET_OPEN, MARKET_CLOSE };

class Event
{
   public:
   Event() {}
   Event(const std::chrono::system_clock::time_point timestamp, EventType eventType, double price) 
         : timestamp(timestamp), eventType(eventType), price(price) {} 
   std::chrono::system_clock::time_point timestamp;
   EventType eventType;
   double price;

};

} // namespace strategy