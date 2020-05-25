#pragma once

#include <string>
#include <chrono>

namespace tf
{

enum SecurityType { UNKNOWN, STOCK, BOND, FUTURE, OPTION, INDEX };

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
   friend bool operator < (const Contract& lhs, const Contract& rhs );
};

class Index : public Contract
{
   public:
   Index() : Contract() {}
   Index(std::string ticker) : Contract(ticker)
   {
      securityType = SecurityType::INDEX;
   }
};

} // namespace tf