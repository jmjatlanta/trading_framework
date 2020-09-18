#pragma once

#include <future>
#include <vector>
#include <domain/Event.hpp>
#include <market_data/streaming/StreamingService.hpp>

namespace market_data
{

template<class Derived>
class AccountingService
{
   public:
   void GetTimeAndSales(tf::Contract c, std::function<tf::EvaluationResult(TickMessage)> func)
   {
      return static_cast<Derived*>(this)->GetTimeAndSales(c, func);
   }
   void GetBookData(tf::Contract c, std::function<void(BookMessage)> func)
   {
      return static_cast<Derived*>(this)->GetBookData(c, func);
   }
};

} // namespace market_data