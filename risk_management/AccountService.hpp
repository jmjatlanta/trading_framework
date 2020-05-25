#pragma once

#include <vector>
#include <risk_management/Order.hpp>

namespace risk_management
{

template<class Derived>
class AccountService
{
   public:
   virtual std::vector<risk_management::Order> GetOpenOrders(uint16_t account_id) = 0;
   virtual bool MarketOpen( strategy::Market mkt ) = 0;

};

} // namespace risk_management