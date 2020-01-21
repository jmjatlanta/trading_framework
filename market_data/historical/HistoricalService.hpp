#pragma once

#include <future>
#include <vector>
#include <market_data/Contract.hpp>

class OHLCBar
{
   public:
   OHLCBar(double o, double h, double l, double c, unsigned long v) 
         : open(o), high(h), low(l), close(c), volume(v) {}
   double open;
   double high;
   double low;
   double close;
   unsigned long volume;
};

template<class Derived>
class HistoricalService
{
   public:
   std::future<std::vector<OHLCBar>> GetBars(tf::Contract c, tf::BarSettings span)
   {
      return static_cast<Derived*>(this)->GetBars(c, span);
   }
};