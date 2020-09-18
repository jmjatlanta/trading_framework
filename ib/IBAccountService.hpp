#pragma once

#include <memory>
#include <ib/IBWrapper.h>
#include <ib/IBConfiguration.hpp>
#include <domain/AccountService.hpp>
#include <domain/Order.hpp>

namespace ib
{

class IBAccountService : public tf::AccountService<IBAccountService>
{
   public:
   IBAccountService(const IBConfiguration& config);
   std::vector<tf::Order> GetOpenOrders(uint16_t accountId);
   bool MarketOpen( tf::Market mkt );
   private:
   std::shared_ptr<ib::IBWrapper> ibWrapper = nullptr;
};

} // namespace ib