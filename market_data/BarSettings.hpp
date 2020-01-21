#pragma once

#include <chrono>

namespace tf
{

enum BarTimeSpan { 
   ONE_SECOND, 
   FIVE_SECONDS,
   FIFTEEN_SECONDS, 
   THIRTY_SECONDS, 
   ONE_MINUTE, 
   TWO_MINUTES, 
   THREE_MINUTES, 
   FIVE_MINUTES, 
   FIFTEEN_MINUTES, 
   THIRTY_MINUTES, 
   ONE_HOUR, 
   ONE_DAY 
};

class BarSettings
{
   public:
   BarSettings()
      : endDateTime(std::chrono::system_clock::now() ),
      duration( std::chrono::hours(24) )
      {}
   std::chrono::system_clock::time_point endDateTime;
   std::chrono::system_clock::duration duration;
   BarTimeSpan barTimeSpan;
};

} // namespace tf