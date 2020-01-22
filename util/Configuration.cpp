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
unsigned int Configuration::GetIBPort() { return 4001; }
unsigned int Configuration::GetIBHistoricalConnectionId() { return 1; }
unsigned int Configuration::GetIBStreamingConnectionId() { return 2; }

} // namespace util