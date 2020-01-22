#pragma once

#include <future>
#include <vector>

#include <ib/IBWrapper.h>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/Contract.hpp>

namespace ib
{

class IBStreamingService : public market_data::StreamingService<IBStreamingService>
{
   public:
   IBStreamingService();
   template<class T>
   void GetTimeAndSales(tf::Contract contract, std::function<void(market_data::TickMessage<T>)>);
   private:
   std::shared_ptr<ib::IBWrapper> ibWrapper = nullptr;
   // methods that talk to IB
};

} // namespace ib