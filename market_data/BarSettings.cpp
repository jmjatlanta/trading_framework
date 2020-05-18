#include <string>
#include <sstream>
#include <market_data/BarSettings.hpp>

namespace tf
{

/****
 * @brief Convert common strings to their equivalent BarTimeSpan
 * Example: nD= number of Days, nH = number of Hours, nM = number of minutes, nS = number of seconds
 * @param in the string
 * @returns A BarTimeSpan
 */
BarTimeSpan StringToBarTimeSpan(const std::string& in)
{
   std::string temp = in;
   std::for_each(temp.begin(), temp.end(), [](char& c) {
      c = ::toupper(c); });
   // look for digit
   uint16_t qty = 0;
   std::stringstream(temp) >> qty;
   if (qty > 0)
   {
      // take off the integer
      std::for_each(temp.begin(), temp.end(), [](char& c) {
         if (::isdigit(c))
            c = ' ';
      });
      temp.erase(std::remove(temp.begin(), temp.end(), ' '), temp.end());
   }
   if (qty == 0)
      qty = 1;
   char freq = temp[0];
   if (qty == 1)
   {
      if (freq == 'D')
         return BarTimeSpan::ONE_DAY;
      if (freq == 'H')
         return BarTimeSpan::ONE_HOUR;
      if (freq == 'M')
         return BarTimeSpan::ONE_MINUTE;
      if (freq == 'S')
         return BarTimeSpan::ONE_SECOND;
   }
   if (qty == 30)
   {
      if (freq == 'M')
         return BarTimeSpan::THIRTY_MINUTES;
      if (freq == 'S')
         return BarTimeSpan::THIRTY_SECONDS;
   }
   if (qty == 15)
   {
      if (freq == 'M')
         return BarTimeSpan::FIFTEEN_MINUTES;
      if (freq == 'S')
         return BarTimeSpan::FIFTEEN_SECONDS;
   }
   if (qty == 5)
   {
      if (freq == 'M')
         return BarTimeSpan::FIVE_MINUTES;
      if (freq == 'S')
         return BarTimeSpan::FIVE_SECONDS;
   }
   if(freq == 'M')
   {
      if (qty == 3)
         return BarTimeSpan::THREE_MINUTES;
      if (qty==2)
         return BarTimeSpan::TWO_MINUTES;
   }
   return BarTimeSpan::INVALID;
}

BarSettings StringToDefaultBarSettings(const std::string& in)
{
   BarSettings retVal;
   retVal.barTimeSpan = StringToBarTimeSpan(in);
   switch( retVal.barTimeSpan )
   {
      case (BarTimeSpan::ONE_SECOND):
         retVal.duration = std::chrono::seconds(300);
         break;
      case (BarTimeSpan::FIVE_SECONDS):
         retVal.duration = std::chrono::seconds(300 * 5);
         break;
      case (BarTimeSpan::FIFTEEN_SECONDS):
         retVal.duration = std::chrono::seconds(300 * 15);
         break;
      case (BarTimeSpan::THIRTY_SECONDS):
         retVal.duration = std::chrono::seconds(300 * 30);
         break;
      case (BarTimeSpan::ONE_MINUTE):
         retVal.duration = std::chrono::minutes(300);
         break;
      case (BarTimeSpan::TWO_MINUTES):
         retVal.duration = std::chrono::minutes(300 * 2);
         break;
      case (BarTimeSpan::THREE_MINUTES):
         retVal.duration = std::chrono::minutes(300 * 3);
         break;
      case (BarTimeSpan::FIVE_MINUTES):
         retVal.duration = std::chrono::minutes(300 * 5);
         break;
      case (BarTimeSpan::FIFTEEN_MINUTES):
         retVal.duration = std::chrono::minutes(300 * 15);
         break;
      case (BarTimeSpan::THIRTY_MINUTES):
         retVal.duration = std::chrono::minutes(300 * 30);
         break;
      case (BarTimeSpan::ONE_HOUR):
         retVal.duration = std::chrono::hours(300);
         break;
      case (BarTimeSpan::ONE_DAY):
         retVal.duration = std::chrono::hours(3*24*365);
         break;
   }
   return retVal;
}

} // namespace tf