#pragma once

#include <string>

namespace util
{

class Configuration
{
   public:
   static Configuration* GetInstance();
   static Configuration* GetInstance(const std::string& configFileName);
   std::string GetIBHost();
   uint16_t GetIBPort();
   uint16_t GetIBHistoricalConnectionId();
   uint16_t GetIBStreamingConnectionId();
   uint16_t GetIBAccountConnectionId();
   protected:
   Configuration();
   std::string host = "127.0.0.1";
   uint16_t port = 4001;
   uint16_t historicalId = 1;
   uint16_t streamingId = 2;
   uint16_t accountId = 3;
};

} // namespace util