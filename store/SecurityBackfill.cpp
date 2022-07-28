/***
 * An application to create data for backtesting
 */
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <store/SecurityFile.hpp>
#include <vendor/date/include/date/tz.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

std::string get_dir(const tf::Contract& contract, const tf::BarTimeSpan& span)
{
   std::string out("./");
   // security type
   switch (contract.securityType)
   {
      case tf::SecurityType::SECURITY_TYPE_STOCK:
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
      case tf::BarTimeSpan::TICK:
         out += "TICK/";
      default:
         break;
   }
   out += contract.ticker + ".csv";
   return out;
}

class tick_writer
{
   public:
   tick_writer(const tf::Contract& contract)
   {
      std::string dir = get_dir(contract, tf::BarTimeSpan::TICK);
      out.open(dir);
   }
   ~tick_writer()
   {
      out.close();
   }
   std::string format_line( std::chrono::system_clock::time_point timestamp, const market_data::TickMessage& msg)
   {
      if (msg.tickType == market_data::TickType::LAST)
      {
         last = msg.price;
      }
      if (msg.tickType == market_data::TickType::VOLUME )
      {
         std::stringstream ss;
         using namespace date;
         ss << timestamp << "," << std::setprecision(7) << last << "," << std::to_string(msg.volume);
         return ss.str();
      }
      return "";
   }
   void process_tick(const market_data::TickMessage& msg)
   {
      std::string line = format_line( std::chrono::system_clock::now(), msg );
      if (!line.empty())
         out << line << std::endl;
   }
   private:
   std::ofstream out;
   double last = 0;
}; 

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
      std::cerr << "Syntax: " << argv[0] << " [SYMBOL] [nD|nH|nM|nS|T]\n";
      exit(1);
   }
   std::string ticker = argv[1];
   std::string barSize = argv[2];
   tf::Stock contract(ticker);
   contract.exchange = "SMART";
   if (barSize == "T") // we want ticks
   {
      ib::IBStreamingService ibStream(IBConfiguration("127.0.0.1", 4001, 1) );
      tick_writer w(contract);
      ibStream.GetTimeAndSales(contract, [&](const market_data::TickMessage& msg) { w.process_tick(msg); });
      // now just wait
      while(true)
         std::this_thread::sleep_for( std::chrono::seconds(1) );
   }
   else
   {
      ib::IBHistoricalService ibHist(IBConfiguration("127.0.0.1", 4001, 1));
      tf::BarSettings span = tf::StringToDefaultBarSettings(barSize);
      if (span.barTimeSpan == tf::BarTimeSpan::INVALID)
      {
         std::cerr << "Syntax: " << argv[0] << " [SYMBOL] [nD|nH|nM|nS]\n";
         exit(1);
      }   
      auto fut = ibHist.GetBars(contract, span);   
      auto bars = fut.get();
      SecurityFile file(get_dir(contract, span.barTimeSpan));
      std::for_each(bars.begin(), bars.end(), [&file](const market_data::OHLCBar& in){
         file.AddBar(in);
      });      
   }
   

}