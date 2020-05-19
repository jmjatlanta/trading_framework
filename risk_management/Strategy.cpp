#include <algorithm>
#include <risk_management/Strategy.hpp>

namespace strategy
{  

void Strategy::SubscribeToEvent(strategy::EventType eventType, tf::Contract contract)
{
   eventContractPairs.push_back( { eventType, contract} );
}

/*
// events coming from data sources
void OnTick(market_data::TickMessage in)
{
   // is someone subscribed?
   // send the message
}

int StrategyRunner::run()
{
   while (!shutting_down)
   {
      // look for an event to process
      streamingService->do_something();
   }
   return 0;
}
*/

} // namespace strategy