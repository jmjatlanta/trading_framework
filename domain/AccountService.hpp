#pragma once

#include <vector>
#include <domain/Order.hpp>

namespace tf
{

template<class Derived>
class AccountService
{
   public:
   /****
    * @param account_id the account id
    * @return collection of open orders
    */
   virtual std::vector<tf::Order> GetOpenOrders(uint16_t account_id) = 0;
   /****
    * @param mkt the market to examine
    * @return true if market is currently open
    */
   virtual bool MarketOpen( tf::Market mkt ) = 0;

};

} // namespace tf