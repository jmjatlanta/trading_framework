#include <chrono>
#include <market_data/accounting/AccountingService.hpp>
#include <util/SysLogger.hpp>

namespace backtest
{

/**
 * This is the OEM part of the system
 */
class BacktestAccountingService : public market_data::AccountingService<BacktestAccountingService>
{
   public:
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
   private:
   std::chrono::time_point<std::chrono::system_clock> currentTime;
};

} // namespace backtest