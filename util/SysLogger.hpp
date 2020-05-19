/*
 * SysLogger.h
 *
 *  Created on: Nov 16, 2017
 *      Author: parallels
 */
#pragma once

#include <string>

namespace util
{

enum LogLevel {
   error, warning, info, debug
};

/**
 * An application-wide logger
 */
class SysLogger {
public:
	virtual ~SysLogger();
	static util::SysLogger* getInstance();
public:
   void error(std::string msg);
   void warning(std::string msg);
   void debug(std::string msg);
   void info(std::string msg);
	void setToConsole(bool value) { toConsole = value; }
	void setToSysLog(bool value) { toSysLog = value; }
   void setLogLevel(enum LogLevel newLevel) { log_level = newLevel; }
   void setErrorStreamToNormalStream(bool err_to_norm) { err_to_normal = err_to_norm; }
   bool isDebug() { return log_level == LogLevel::debug; }
private:
   // log at error level (deprecated for public use)
	void log(std::string msg, enum LogLevel lvl = LogLevel::error);
   int convertToSyslogLevel(enum LogLevel lvl);
	SysLogger();
	bool toConsole;
	bool toSysLog;
   enum LogLevel log_level;
   bool err_to_normal = false;
};

} // namespace util
