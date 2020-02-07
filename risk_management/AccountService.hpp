#pragma once

#include <vector>
#include <risk_management/Order.hpp>

namespace risk_management
{

template<class Derived>
class AccountService
{
   public:
   std::vector<risk_management::Order> GetOpenOrders();

};

} // namespace risk_management