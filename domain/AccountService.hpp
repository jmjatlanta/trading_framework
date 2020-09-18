#pragma once

#include <vector>
#include <domain/Order.hpp>

namespace tf
{

template<class Derived>
class AccountService
{
   public:
   virtual std::vector<tf::Order> GetOpenOrders(uint16_t account_id) = 0;
   virtual bool MarketOpen( tf::Market mkt ) = 0;

};

} // namespace tf