#include <algorithm>
#include <ib/IBHistoricalService.hpp>
#include <market_data/BarSettings.hpp>


namespace ib
{

IBHistoricalService::IBHistoricalService(const IBConfiguration& config)
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

std::future<std::vector<market_data::OHLCBar>> IBHistoricalService::GetBars(tf::Contract contract, tf::BarSettings span)
{
   // use IB to get historical information
   return std::async(&ib::IBHistoricalService::requestHistoricalData, this, contract, span);
}

std::vector<market_data::OHLCBar> IBHistoricalService::requestHistoricalData(tf::Contract contract, tf::BarSettings span)
{
   // NOTE: this will probably be running in a separate thread
   // todo: avoid wait locks by signalling
   // translate incoming parameters into something IB understands
   Contract ib_contract;
   ib_contract.localSymbol = contract.ticker;
   ib_contract.symbol = contract.ticker;
   ib_contract.exchange = "SMART";
   ib_contract.secType = SecurityTypeToIBString(contract.securityType);
   ib_contract.currency = contract.currency;
   std::string endDateTime = TimeToIBTime(span.endDateTime);
   std::string durationString = DurationToIBDuration(span.duration);
   std::string barSizeSetting = BarTimeSpanToIBString(span.barTimeSpan);
   std::string whatToShow("TRADES");
   int useRTH = 1; // true
   int formatDate = 1; // yyyyMMdd HH:mm::ss, or 2 for system time
   bool keepUpToDate = false;
   TagValueListSPtr chartOptions = nullptr;
   int reqId = ibWrapper->reqHistoricalData(ib_contract, endDateTime, durationString, barSizeSetting, whatToShow, useRTH, formatDate, keepUpToDate, chartOptions );
   // wait for completion
   while (!ibWrapper->isCompleted(reqId))
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }
   std::vector<Bar> bars = ibWrapper->GetHistoricalData(reqId);
   std::vector<market_data::OHLCBar> retVal;
   std::for_each(bars.begin(), bars.end(), [&retVal](Bar b){
      market_data::OHLCBar newBar(b.time, b.open, b.high, b.low, b.close, b.volume);
      retVal.push_back( newBar );
   });
   return retVal;
}

} // namespace ib