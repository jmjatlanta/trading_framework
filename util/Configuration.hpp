#pragma once

#include <string>

namespace util
{

class Configuration
{
   public:
   static Configuration* GetInstance();
   std::string GetIBHost();
   uint16_t GetIBPort();
   uint16_t GetIBHistoricalConnectionId();
   uint16_t GetIBStreamingConnectionId();
   uint16_t GetIBAccountConnectionId();
   private:
   Configuration();
};

} // namespace util