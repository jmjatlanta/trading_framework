#include <util/Time.hpp>
#include <vendor/date/include/date/tz.h>

namespace util
{

/***
 * Convert to Eastern time
 * @param in the local time
 * @returns a tm struct in Eastern time
 */
struct tm* adjust_to_eastern( const std::chrono::system_clock::time_point& in )
{
   auto zone = date::locate_zone( "America/New_York" );
   auto t = date::make_zoned( zone, in ); // convert incoming to Eastern time
   std::chrono::system_clock::time_point eastern_time_point( t.get_local_time().time_since_epoch() );
   auto my_time_t = std::chrono::system_clock::to_time_t( eastern_time_point );
   return std::localtime( &my_time_t );
}

} // namespace util