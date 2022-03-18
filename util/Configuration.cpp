#include <util/Configuration.hpp>
#include <rapidjson/document.h>

#include <fstream>
#include <sstream>

namespace util
{

class MutableConfiguration : public Configuration
{
public:
    void SetIBHost(const std::string& in) { host = in; }
    void SetIBPort(uint16_t in) { port = in; }
    void SetIBHistoraicalConnectionId(uint16_t in) { historicalId = in; }
    void SetIBStreamingConnectionId(uint16_t in) { streamingId = in; }
    void SetIBAccountConnectionId(uint16_t in) { accountId = in; }
};

Configuration* Configuration::GetInstance()
{
   static Configuration conf;
   return &conf;
}

std::string ReadTextFile(const std::string& fileName)
{
    std::ifstream in(fileName);
    if (in)
    {
        std::stringstream ss;
        ss << in.rdbuf();
        in.close();
        return ss.str();
    }
    return "";
}

Configuration* Configuration::GetInstance(const std::string& fileName)
{
    Configuration* conf = GetInstance();
    MutableConfiguration* mut = reinterpret_cast<MutableConfiguration*>(conf);
    // read file
    rapidjson::Document doc;
    doc.Parse( ReadTextFile(fileName).c_str());
    if (doc.HasParseError())
    {
        auto error = doc.GetParseError();
        throw std::logic_error("Unable to parse configuration file. Error " + std::to_string(error) );
    }
    if (!doc.HasMember("InteractiveBrokers"))
        throw std::logic_error("Invalid configuration file");
    auto& ib = doc["InteractiveBrokers"];
    mut->SetIBHost( ib["Host"].GetString() );
    mut->SetIBPort( ib["Port"].GetUint() );
    mut->SetIBHistoraicalConnectionId( ib["HistoricalConnectionId"].GetUint() );
    mut->SetIBStreamingConnectionId( ib["StreamingConnectionId"].GetUint() );
    mut->SetIBAccountConnectionId( ib["AccountConnectionId"].GetUint() );
    return conf;
}

Configuration::Configuration()
{
}

std::string Configuration::GetIBHost() { return host; }
uint16_t Configuration::GetIBPort() { return port; }
uint16_t Configuration::GetIBHistoricalConnectionId() { return historicalId; }
uint16_t Configuration::GetIBStreamingConnectionId() { return streamingId; }
uint16_t Configuration::GetIBAccountConnectionId() { return accountId; }

} // namespace util