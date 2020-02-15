#include <algorithm>
#include <risk_management/Strategy.hpp>

namespace strategy
{

void Strategy::SubscribeToEvent(strategy::EventType eventType, tf::Contract contract)
{
   eventContractPairs.push_back( { eventType, contract} );
}

/**
 * This should connect to external systems and manage everything related to a strategy
 */
StrategyRunner::StrategyRunner()
{
}
   
void StrategyRunner::AddStrategy(std::shared_ptr<Strategy> s) 
{
   strategies.push_back(s);
   // make sure that we are subscribed to the information we need for this strategy
   std::for_each(s->eventContractPairs.begin(), s->eventContractPairs.end(), [&](std::pair<EventType, tf::Contract> in) {
      switch(in.first)
      {
         case EventType::BID:
         case EventType::ASK:
         case EventType::LAST:
            //streamingService.GetTimeAndSales(in.second, OnTick);
            break;
         default:
            break;
      }
   });

}

// events coming from data sources
void OnTick(market_data::TickMessage in)
{
   // is someone subscribed?
   // send the message
}

} // namespace strategy