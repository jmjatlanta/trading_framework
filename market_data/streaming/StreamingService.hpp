#pragma once

#include <future>
#include <vector>
#include <market_data/Contract.hpp>

namespace market_data
{

enum TickType { BID, ASK, LAST, VOLUME, TEXT };

template<class T>
class TickMessage
{
   public:
   TickMessage(TickType tt, T value) : tickType(tt), value(value) {}
   TickType tickType;
   T value;
};

template<class Derived>
class StreamingService
{
   public:
   template<class T>
   std::future<std::vector<OHLCBar>> GetTimeAndSales(tf::Contract c, std::function<void(TickMessage<T>)> func)
   {
      return static_cast<Derived*>(this)->GetTimeAndSales(c, func);
   }
};

} // namespace market_data