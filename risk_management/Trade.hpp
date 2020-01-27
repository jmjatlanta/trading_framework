#pragma once
/****
 * This contains the details of a potential trade.
 * Normally, this runs through the risk management
 * subsystem, which places the trade if it matches
 * risk management rules
 */

#include <vector>

#include <risk_management/Order.hpp>

class Trade
{
   public:
   std::string strategy;
   std::vector<Order> orders;
};