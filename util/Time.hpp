#pragma once

#include <chrono>

namespace util
{

/***
 * Convert to Eastern time
 * @param in the local time
 * @returns a tm struct in Eastern time
 */
struct tm* adjust_to_eastern( const std::chrono::system_clock::time_point& in );

} // namespace util