#pragma once

#include <string>
#include <chrono>

namespace tf
{

enum SecurityType { 
   SECURITY_TYPE_UNKNOWN, 
   SECURITY_TYPE_STOCK,
   SECURITY_TYPE_BOND, 
   SECURITY_TYPE_FUTURE, 
   SECURITY_TYPE_OPTION, 
   SECURITY_TYPE_INDEX 
};

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
         securityType(SecurityType::SECURITY_TYPE_STOCK)
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
      securityType = SecurityType::SECURITY_TYPE_INDEX;
   }
};

} // namespace tf