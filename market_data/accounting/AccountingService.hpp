#pragma once

#include <future>
#include <vector>
#include <risk_management/Event.hpp>
#include <market_data/streaming/StreamingService.hpp>

namespace market_data
{

template<class Derived>
class AccountingService
{
   public:
   void GetTimeAndSales(tf::Contract c, std::function<strategy::EvaluationResult(TickMessage)> func)
   {
      return static_cast<Derived*>(this)->GetTimeAndSales(c, func);
   }
   void GetBookData(tf::Contract c, std::function<void(BookMessage)> func)
   {
      return static_cast<Derived*>(this)->GetBookData(c, func);
   }
};

} // namespace market_data