#include <util/Configuration.hpp>
#include <ib/IBAccountService.hpp>

namespace ib
{

IBAccountService::IBAccountService()
{
   // grab connection information from configuration
   util::Configuration* conf = util::Configuration::GetInstance();
   ibWrapper = std::make_shared<IBWrapper>( conf->GetIBHost(), conf->GetIBPort(), conf->GetIBAccountConnectionId() );
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

std::vector<risk_management::Order> IBAccountService::GetOpenOrders(uint16_t accountId)
{
   return std::vector<risk_management::Order>();
}

} // namespace ib