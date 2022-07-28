#pragma once
#include <cstdint>
#include <string>

#include <domain/Contract.hpp>

namespace tf
{

enum OrderStatus {
   ORDER_STATUS_UNKNOWN,
   ORDER_STATUS_UNSENT,
   ORDER_STATUS_SENT,
   ORDER_STATUS_AWAITING_EXECUTION,
   ORDER_STATUS_REJECTED,
   ORDER_STATUS_CANCELED,
   ORDER_STATUS_PARTIALLY_FILLED,
   ORDER_STATUS_FILLED
};

class OrderInfo
{
   public:
   OrderStatus status;
   std::uint64_t id;
   std::string brokerId;
};

class Order
{
   public:
   long num_shares;
   double limit_price;
   tf::Contract* contract;
   bool going_long = false;
};

} // namespace tf