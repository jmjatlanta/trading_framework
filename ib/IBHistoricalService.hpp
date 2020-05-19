#pragma once

#include <future>
#include <vector>

#include <ib/IBWrapper.h>
#include <ib/IBConfiguration.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/Contract.hpp>

namespace ib
{

class IBHistoricalService : public market_data::HistoricalService<IBHistoricalService>
{
   public:
   IBHistoricalService(const IBConfiguration& config);
   std::future<std::vector<market_data::OHLCBar>> GetBars(tf::Contract contract, tf::BarSettings span);
   private:
   std::shared_ptr<ib::IBWrapper> ibWrapper = nullptr;
   // methods that talk to IB
   /***
    * @brief gets bar data for contract
    * NOTE: This blocks until complete
    * @returns a vector of OHLCBar up to the most recent
    */
   std::vector<market_data::OHLCBar> requestHistoricalData(tf::Contract contract, tf::BarSettings span);
};

} // namespace ib