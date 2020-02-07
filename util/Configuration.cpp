#include <util/Configuration.hpp>

namespace util
{

Configuration* Configuration::GetInstance()
{
   static Configuration conf;
   return &conf;
}

Configuration::Configuration()
{
}

std::string Configuration::GetIBHost() { return "127.0.0.1"; }
uint16_t Configuration::GetIBPort() { return 4001; }
uint16_t Configuration::GetIBHistoricalConnectionId() { return 1; }
uint16_t Configuration::GetIBStreamingConnectionId() { return 2; }
uint16_t Configuration::GetIBAccountConnectionId() { return 3; }

} // namespace util