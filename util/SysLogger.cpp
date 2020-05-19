/*
 * SysLogger.cpp
 *
 *  Created on: Nov 16, 2017
 *      Author: parallels
 */

#include <string>
#include <iostream>
#include "sys/syslog.h"
#include "SysLogger.hpp"

namespace util
{

util::SysLogger* syslogger_instance = nullptr;

/**
 * Default constructor. Note: this is private
 */
SysLogger::SysLogger() : toConsole(false), toSysLog(true), log_level(LogLevel::error) {

}

util::SysLogger* util::SysLogger::getInstance() {
	if (syslogger_instance == nullptr) {
		syslogger_instance = new SysLogger();
	}
	return syslogger_instance;
}

SysLogger::~SysLogger() {
	// TODO Auto-generated destructor stub
}

int SysLogger::convertToSyslogLevel(enum LogLevel lvl)
{
   switch(lvl)
   {
      case (LogLevel::error): return LOG_ERR;
      case (LogLevel::debug): return LOG_DEBUG;
      case (LogLevel::warning): return LOG_WARNING;
      default: return LOG_INFO;
   }
}

void SysLogger::log(std::string msg, enum LogLevel lvl) {
   if (lvl <= log_level)
   {
      if (toConsole) {
         if (lvl == LogLevel::error && !err_to_normal)
            std::cerr << msg << std::endl;
         else
            std::cout << msg << std::endl;
      }
      if (toSysLog) {
         syslog(convertToSyslogLevel(lvl), "%s\n", msg.c_str());
      }
   }
}

void SysLogger::error(std::string msg)
{
   log(msg, LogLevel::error);
}

void SysLogger::warning(std::string msg)
{
   log(msg, LogLevel::warning);
}

void SysLogger::debug(std::string msg)
{
   log(msg, LogLevel::debug);
}

void SysLogger::info(std::string msg)
{
   log(msg, LogLevel::info);
}

} // namespace util
