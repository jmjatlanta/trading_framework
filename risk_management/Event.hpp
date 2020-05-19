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

enum EventType { BID, ASK, LAST, MARKET_TIME };

class Event
{
   public:
   Event() {}
   Event(std::string timestamp, EventType eventType, double price) : timestamp(timestamp), eventType(eventType), price(price) {} 
   std::string timestamp;
   EventType eventType;
   double price;

};

} // namespace strategy