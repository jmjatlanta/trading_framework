#define BOOST_TEST_MODULE trading_framework
#include <boost/test/included/unit_test.hpp>

#include <ib/IBWrapper.h>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <ib/IBAccountService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/BarSettings.hpp>
#include <market_data/Contract.hpp>
#include <risk_management/Order.hpp>

class MockHistoricalService : public HistoricalService<MockHistoricalService>
{
   public:
   std::future<std::vector<OHLCBar>> GetBars(tf::Contract cont, tf::BarSettings span)
   {
      return std::async(&MockHistoricalService::do_nothing, this, cont, span);
   }
   private:
   std::vector<OHLCBar> do_nothing(tf::Contract cont, tf::BarSettings span)
   {
      std::vector<OHLCBar> retVal;
      return retVal;
   }
};

BOOST_AUTO_TEST_SUITE ( ib )

BOOST_AUTO_TEST_CASE( login )
{
   try
   {
      IBWrapper ibw("localhost", 4001, 1);
   }
   catch(...)
   {
      BOOST_FAIL("ib/login: immediate creation and deletion threw exception");
   }
}

BOOST_AUTO_TEST_CASE( generic_historical_ifc )
{
   // Grab the last 4 days of MSFT bars
   tf::Contract msft("MSFT");
   tf::BarSettings barSettings;
   barSettings.duration = std::chrono::hours(96);
   barSettings.barTimeSpan = tf::BarTimeSpan::FIVE_MINUTES;

   MockHistoricalService svc;
   auto retVal = svc.GetBars(msft, barSettings);
   auto mock_bars = retVal.get();
   BOOST_TEST( mock_bars.size() == 0 );

   IBHistoricalService svc2;
   auto retVal2 = svc2.GetBars(msft, barSettings);
   auto bars = retVal2.get();
   BOOST_TEST( bars.size() > 0);
}

BOOST_AUTO_TEST_CASE( streaming_test )
{
   tf::Contract msft("MSFT");

   bool atLeastOne = false;
   auto func = [&atLeastOne](market_data::TickMessage tm) {
      switch (tm.tickType)
      {
         case (market_data::TickType::BID):
            std::cout << "Bid\n";
            break;
         case (market_data::TickType::ASK):
            std::cout << "Ask\n";
            break;
         case (market_data::TickType::LAST):
            std::cout << "Last\n";
            break;
         case (market_data::TickType::TEXT):
            std::cout << "Text\n";
            break;
         case (market_data::TickType::VOLUME):
            std::cout << "Volume\n";
            break;
      }
      atLeastOne = true;
   };

   IBStreamingService ib;
   ib.GetTimeAndSales(msft, func );
   std::this_thread::sleep_for( std::chrono::seconds(3) );
   BOOST_TEST( atLeastOne == true );
}

BOOST_AUTO_TEST_CASE( book_test )
{
   /**
    * Note: This requires subscription to L2
    */
   tf::Contract codx("T");
   codx.exchange = "ARCA";
   bool atLeastOne = false;

   auto func = [&atLeastOne](const market_data::BookMessage msg) {
      atLeastOne = true;
   };

   IBStreamingService ib;
   ib.GetBookData(codx, func);
   std::this_thread::sleep_for( std::chrono::seconds(3) );
   BOOST_TEST( atLeastOne == true );
}

BOOST_AUTO_TEST_CASE( account_test )
{
   ib::IBAccountService acctSvc;
   std::vector<risk_management::Order> orders = acctSvc.GetOpenOrders(1);
   
}

BOOST_AUTO_TEST_SUITE_END()