#pragma once

#include <future>
#include <vector>

#include <ib/IBWrapper.h>
#include <ib/IBConfiguration.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/Contract.hpp>

namespace ib
{

class IBStreamingService : public market_data::StreamingService<IBStreamingService>
{
   public:
   IBStreamingService(const IBConfiguration& config);
   void GetTimeAndSales(tf::Contract contract, std::function<void(market_data::TickMessage)> func);
   void GetBookData(tf::Contract contract, std::function<void(market_data::BookMessage)> func);
   private:
   std::shared_ptr<ib::IBWrapper> ibWrapper = nullptr;
   // methods that talk to IB
};

} // namespace ib