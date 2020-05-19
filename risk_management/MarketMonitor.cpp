#include <risk_management/Strategy.hpp>

namespace strategy
{

MarketMonitor::MarketMonitor( Market mkt )
{

}

double MarketMonitor::OpeningGapPct(tf::Contract contract)
{
   return -1.0;
}

double MarketMonitor::LowOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract)
{
   return 1.0;
}

double MarketMonitor::HighOfDay(std::chrono::time_point<std::chrono::system_clock> date, tf::Contract contract)
{
   return 10.0;
}

double MarketMonitor::PreviousDayClose(std::chrono::time_point<std::chrono::system_clock> today, tf::Contract contract)
{
   return 1.0;
}

double MarketMonitor::LastTradePrice(tf::Contract contract)
{
   return 5.0;
}

double MarketMonitor::LastBidPrice(tf::Contract contract)
{
   return 4.0;
}

double MarketMonitor::LastAskPrice(tf::Contract contract)
{
   return 6.0;
}

double MarketMonitor::SMA(uint16_t numBars, tf::Contract contract)
{
   return 4.0;
}

} // namespace strategy