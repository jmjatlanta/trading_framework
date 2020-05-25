#pragma once

#include <memory>
#include <ib/IBWrapper.h>
#include <ib/IBConfiguration.hpp>
#include <risk_management/AccountService.hpp>
#include <risk_management/Order.hpp>

namespace ib
{

class IBAccountService : public risk_management::AccountService<IBAccountService>
{
   public:
   IBAccountService(const IBConfiguration& config);
   std::vector<risk_management::Order> GetOpenOrders(uint16_t accountId);
   bool MarketOpen( strategy::Market mkt );
   private:
   std::shared_ptr<ib::IBWrapper> ibWrapper = nullptr;
};

} // namespace ib