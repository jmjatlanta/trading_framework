#pragma once

#include <future>
#include <vector>
#include <sstream>
#include <iomanip>
#include <market_data/Contract.hpp>
#include <market_data/BarSettings.hpp>

namespace market_data
{

class OHLCBar
{
   public:
   OHLCBar(std::string timestamp, double o, double h, double l, double c, unsigned long v) 
         : timestamp(timestamp), open(o), high(h), low(l), close(c), volume(v) {}
   std::string timestamp;
   double open;
   double high;
   double low;
   double close;
   unsigned long volume;
   std::string toCSV() const
   {
      std::stringstream out;
      out << timestamp << ","
            << std::setprecision(6) << open << "," 
            << high  << ","
            << low  << ","
            << close  << ","
            << volume;
      return out.str();
   }
};

template<class Derived>
class HistoricalService
{
   public:
   std::future<std::vector<OHLCBar>> GetBars(const tf::Contract& c, const tf::BarSettings& span)
   {
      return static_cast<Derived*>(this)->GetBars(c, span);
   }
   double OpeningGapPct(tf::Contract contract);
   double LowOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract);
   double HighOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract);
   double PreviousDayClose(std::chrono::time_point<std::chrono::system_clock> today, tf::Contract contract);
   double LastTradePrice(tf::Contract contract);
   double LastBidPrice(tf::Contract contract);
   double LastAskPrice(tf::Contract contract);
   double SMA(uint16_t numBars, tf::Contract contract);
};

} // namespace market_data