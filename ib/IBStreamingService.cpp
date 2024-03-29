#include <algorithm>
#include <ib/IBStreamingService.hpp>
#include <market_data/BarSettings.hpp>


namespace ib
{

IBStreamingService::IBStreamingService(const IBConfiguration& config)
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

void IBStreamingService::GetTimeAndSales(tf::Contract contract, std::function<void(market_data::TickMessage)> func)
{
   // convert TickMessage to market_data::tick_message
   auto conv_func = [&func](const ib::TickMessage& ib_msg) {
      market_data::TickType md_type = IBTickTypeToMDTickType(ib_msg.tickType);
      switch (md_type)
      {
         case (market_data::TickType::BID):
         case (market_data::TickType::ASK):
         case (market_data::TickType::LAST):
         case market_data::TickType::VOLUME:
         case market_data::TickType::TEXT:
            func(market_data::TickMessage(md_type, ib_msg.price, ib_msg.volume, ib_msg.message));
            break;
      }
   };

   ibWrapper->SubscribeToTickData(ContractToIBContract(contract), conv_func );
}

void IBStreamingService::GetBookData(tf::Contract contract, std::function<void(market_data::BookMessage)> func)
{
    // convert IB market data message to its generic equivalent
    auto conv_func = [&func](const ib::BookMessage& ib_msg) {
      func( as<market_data::BookMessage>(ib_msg));
    };

    ibWrapper->SubscribeToBookData(ContractToIBContract(contract), conv_func);
}

} // namespace ib