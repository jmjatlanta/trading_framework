#pragma once

#include <future>
#include <vector>
#include <domain/Contract.hpp>
#include <domain/Event.hpp>

namespace market_data
{

enum TickType { BID, ASK, LAST, VOLUME, TEXT, UNKNOWN };

class TickMessage
{
   public:
   TickMessage(TickType tt, double price, long volume, std::string message) 
      : tickType(tt), price(price), volume(volume), text_message(message) {}
   TickType tickType;
   double price = 0.0;
   long volume = 0;
   std::string text_message = "";
};

class BookMessage
{
   public:
   tf::Contract contract;
   int position;
   int operation;
   int side;
   double price;
   int size;
};

template<class Derived>
class StreamingService
{
   public:
   void GetTimeAndSales(tf::Contract c, std::function<tf::EvaluationResult(TickMessage)> func)
   {
      return static_cast<Derived*>(this)->GetTimeAndSales(c, func);
   }
   void GetBookData(tf::Contract c, std::function<void(BookMessage)> func)
   {
      return static_cast<Derived*>(this)->GetBookData(c, func);
   }
};

} // namespace market_data