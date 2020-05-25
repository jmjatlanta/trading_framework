#pragma once

#include <future>
#include <list>
#include <sstream>
#include <iomanip>
#include <optional>
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
   bool operator < (const OHLCBar& in)
   {
      return timestamp < in.timestamp;
   }
};

template<class Derived>
class HistoricalService
{
   public:
   std::future<std::list<OHLCBar>> GetBars(const tf::Contract& c, const tf::BarSettings& span)
   {
      return static_cast<Derived*>(this)->GetBars(c, span);
   }
   std::optional<double> OpeningGapPct(tf::Contract contract);
   std::optional<double> LowOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract, bool market_hours = true);
   std::optional<double> HighOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract, bool market_hours = true);
   std::optional<double> PreviousDayClose(std::chrono::time_point<std::chrono::system_clock> today, tf::Contract contract);
   std::optional<double> LastTradePrice(tf::Contract contract);
   virtual std::optional<double> LastBidPrice(tf::Contract contract) = 0;
   virtual std::optional<double> LastAskPrice(tf::Contract contract) = 0;
   virtual std::optional<double> SMA(uint16_t numBars, tf::Contract contract, bool market_hours = true) = 0;
};

} // namespace market_data