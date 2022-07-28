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
    protected:
    Contract(SecurityType type) : securityType(type) {}
    Contract(const std::string& ticker, SecurityType type) 
        : ticker(ticker),
        securityType(type)
        {}
    public:
    std::string ticker;
    std::string currency;
    std::string exchange;
    const SecurityType securityType;
    friend bool operator < (const Contract& lhs, const Contract& rhs );
};

class Index : public Contract
{
   public:
   Index() : Contract(SecurityType::SECURITY_TYPE_INDEX) {}
   Index(const std::string& ticker) : Contract(ticker, SecurityType::SECURITY_TYPE_INDEX) {}
};

class Stock : public Contract
{
    public:
    Stock() : Contract(SecurityType::SECURITY_TYPE_STOCK) {}
    Stock(const std::string& ticker) : Contract(ticker, SecurityType::SECURITY_TYPE_STOCK) {}
};

} // namespace tf