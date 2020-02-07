#pragma once

#include <string>
#include <chrono>

namespace tf
{

enum SecurityType { UNKNOWN, STOCK, BOND, FUTURE, OPTION };

/**
 * A generic "contract" to hold many types of securities
 */
class Contract
{
   public:
   Contract(){}
   Contract(std::string ticker) 
       : ticker(ticker),
         currency("USD"),
         securityType(SecurityType::STOCK)
         {}
   std::string ticker;
   std::string currency;
   std::string exchange;
   SecurityType securityType;
};

} // namespace tf