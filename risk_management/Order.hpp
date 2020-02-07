#pragma once
#include <cstdint>
#include <string>

namespace risk_management
{

enum OrderStatus {
   UNKNOWN,
   UNSENT,
   SENT,
   AWAITING_EXECUTION,
   REJECTED,
   CANCELED,
   PARTIALLY_FILLED,
   FILLED
};

class Order
{
   public:
   OrderStatus status;
   std::uint64_t id;
   std::string brokerId;
};

} // namespace risk_management