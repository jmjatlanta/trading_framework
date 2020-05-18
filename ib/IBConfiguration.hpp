#pragma once
#include <string>
#include <cstdint>

class IBConfiguration
{
   public:
   IBConfiguration(std::string ip, uint16_t port, uint16_t client_id)
         : ip(ip), port(port), client_id(client_id) {}
   public:
   std::string ip;
   uint16_t port;
   uint16_t client_id;
};