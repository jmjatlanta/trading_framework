#pragma once

#include <future>
#include <vector>
#include <market_data/Contract.hpp>

namespace market_data
{

enum TickType { BID, ASK, LAST, VOLUME, TEXT };

class TickMessage
{
   public:
   TickMessage(TickType tt, double price, long volume, std::string message) 
      : tickType(tt), price(price), volume(volume), message(message) {}
   TickType tickType;
   double price;
   long volume;
   std::string message;
};

template<class Derived>
class StreamingService
{
   public:
   void GetTimeAndSales(tf::Contract c, std::function<void(TickMessage)> func)
   {
      return static_cast<Derived*>(this)->GetTimeAndSales(c, func);
   }
};

} // namespace market_data