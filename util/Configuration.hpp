#pragma once

#include <string>

namespace util
{

class Configuration
{
   public:
   static Configuration* GetInstance();
   std::string GetIBHost();
   unsigned int GetIBPort();
   unsigned int GetIBHistoricalConnectionId();
   private:
   Configuration();
};

} // namespace util