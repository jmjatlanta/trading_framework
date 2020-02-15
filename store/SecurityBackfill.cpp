/***
 * An application to create data for backtesting
 */
#include <ib/IBHistoricalService.hpp>
#include <store/SecurityFile.hpp>
#include <iostream>
#include <algorithm>
#include <vector>

std::string get_dir(const tf::Contract& contract, const tf::BarTimeSpan& span)
{
   std::string out("./");
   // security type
   switch (contract.securityType)
   {
      case tf::SecurityType::STOCK:
         out += "STOCK/";
         break;
      default:
         break;
   }
   out += "IB/";
   switch(span)
   {
      case tf::BarTimeSpan::ONE_DAY:
         out += "DAY/";
         break;
      case tf::BarTimeSpan::FIVE_MINUTES:
         out += "5MIN/";
         break;
      case tf::BarTimeSpan::ONE_MINUTE:
         out += "1MIN/";
         break;
      default:
         break;
   }
   out += contract.ticker + ".csv";
   return out;
}

/***
 * NOTE: > 60 requests within 10 minutes is the limit for IB
 * 1 request every 60 seconds is 10 requests in 10 minues
 * 1 request every 30 seconds is 20 requests
 * 1 request every 15 seconds is 40 requests
 * 1 request every 10 seconds is 60 requests
 */
int main(int argc, char** argv)
{
   if (argc < 3)
   {
      std::cerr << "Syntax: " << argv[0] << " [SYMBOL] [D|5|1]\n";
      exit(1);
   }
   ib::IBHistoricalService ibHist;
   std::string ticker = argv[1];
   std::string barSize = argv[2];
   tf::Contract contract(ticker);
   tf::BarSettings span;
   if (barSize != "D" && barSize != "5" && barSize != "1")
   {
      std::cerr << "Syntax: " << argv[0] << " [SYMBOL] [D|5|1]\n";
      exit(1);
   }   
   if (barSize == "D")
   {
      span.barTimeSpan = tf::BarTimeSpan::ONE_DAY; 
      span.duration = std::chrono::hours(3*24*365); // 253 trading days per year avg
   }
   if (barSize == "5")
   {
      span.barTimeSpan = tf::BarTimeSpan::FIVE_MINUTES;
      span.duration = std::chrono::hours(24*253); // 1 trading year
   }
   if (barSize == "1")
   {
      span.barTimeSpan = tf::BarTimeSpan::ONE_MINUTE;
      span.duration = std::chrono::hours(24*126); // 6 trading months
 }
   auto fut = ibHist.GetBars(contract, span);   
   auto bars = fut.get();
   SecurityFile file(get_dir(contract, span.barTimeSpan));
   std::for_each(bars.begin(), bars.end(), [&file](const OHLCBar& in){
      file.AddBar(in);
   });

}