#pragma once

#include <chrono>
#include <market_data/accounting/AccountingService.hpp>
#include <domain/Strategy.hpp>
#include <vendor/date/include/date/tz.h>
#include <util/SysLogger.hpp>
#include <util/Time.hpp>

namespace backtest
{

/**
 * This is the OEM part of the system
 */
class BacktestAccountingService : public market_data::AccountingService<BacktestAccountingService>
{
   public:
   /**
    * @brief advance the clock to the specified time (expects UTC)
    * Note that the system expects time to advance forward only
    * 
    * @param newTime the new time
    */
   void AdvanceTime(std::chrono::time_point<std::chrono::system_clock> newTime)
   {
      currentTime = newTime;
      util::SysLogger* logger = util::SysLogger::getInstance();
      if (logger->isDebug())
      {
         std::time_t current = std::chrono::system_clock::to_time_t(currentTime);
         logger->debug( std::string() + std::ctime( &current ) );
      }
   }
   /***
    * @brief Determines if the market is open
    * Note that this does not check for holidays
    * @param the market to check (currently unused, expects a NY market
    * @returns TRUE if the market is opened based on the "current time" of the simulation
    */
   bool MarketOpen( tf::Market exch )
   {
      struct tm* local_time = util::adjust_to_eastern( currentTime );
      // are we on a weekday
      if (local_time->tm_wday == 0 || local_time->tm_wday == 6 )
         return false;
      // are we within market hours
      if ( (local_time->tm_hour < 9 )
            || (local_time->tm_hour == 9 && local_time->tm_min < 30)
            || (local_time->tm_hour >= 16 ) )
         return false;
      return true;
   }
   private:
   std::chrono::time_point<std::chrono::system_clock> currentTime;
};

} // namespace backtest