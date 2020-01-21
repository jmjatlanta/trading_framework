#pragma once

#include <future>
#include <vector>

#include <ib/IBWrapper.h>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/Contract.hpp>

namespace ib
{

class IBHistoricalService : public HistoricalService<IBHistoricalService>
{
   public:
   IBHistoricalService();
   std::future<std::vector<OHLCBar>> GetBars(tf::Contract contract, tf::BarSettings span);
   private:
   std::shared_ptr<ib::IBWrapper> ibWrapper = nullptr;
   // methods that talk to IB
   /***
    * @brief gets bar data for contract
    * NOTE: This blocks until complete
    * @returns a vector of OHLCBar up to the most recent
    */
   std::vector<OHLCBar> requestHistoricalData(tf::Contract contract, tf::BarSettings span);
};

} // namespace ib