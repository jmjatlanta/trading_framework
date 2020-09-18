#include <util/Configuration.hpp>
#include <util/Time.hpp>
#include <ib/IBAccountService.hpp>

namespace ib
{

IBAccountService::IBAccountService(const IBConfiguration& config)
{
   ibWrapper = std::make_shared<IBWrapper>( config.ip, config.port, config.client_id );
   ConnectionStatus currStatus = ibWrapper->GetConnectionStatus();
   int counter = 0;
   while (currStatus != ConnectionStatus::CONNECTED && counter < 30)
   {
      if (currStatus == ConnectionStatus::DISCONNECTED)
         break;
      std::this_thread::sleep_for( std::chrono::milliseconds(500));
      currStatus = ibWrapper->GetConnectionStatus();
      counter++;
   }
   if (currStatus != ConnectionStatus::CONNECTED)
      throw IBException("Unable to connect");   
}

std::vector<tf::Order> IBAccountService::GetOpenOrders(uint16_t accountId)
{
   return std::vector<tf::Order>();
}

bool IBAccountService::MarketOpen( tf::Market mkt )
{
   struct tm* local_time = util::adjust_to_eastern( std::chrono::system_clock::now() );
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

} // namespace ib