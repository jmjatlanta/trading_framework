#pragma once
#include <cstdint>
#include <vector>
#include <domain/Order.hpp>

namespace tf
{

class Account
{
   public:
   std::uint64_t id;
   double totalBalance; // if liquidated all holdings, what would the current balance be?
   double availableBalance; // current liquid balance (not tied up in securities), open orders not included
   double openOrdersBalance; // funds tied up in open orders
   std::vector<Order> openOrders;

};

} // namespace tf