#define BOOST_TEST_MODULE trading_framework
#include <boost/test/included/unit_test.hpp>

#include <ib/IBWrapper.h>
#include <ib/IBHistoricalService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/BarSettings.hpp>
#include <market_data/Contract.hpp>


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
   tf::Contract msft("MSFT");
   MockHistoricalService svc;
   tf::BarSettings barSettings;
   barSettings.duration = std::chrono::hours(96);
   barSettings.barTimeSpan = tf::BarTimeSpan::FIVE_MINUTES;
   auto retVal = svc.GetBars(msft, barSettings);
   auto mock_bars = retVal.get();
   BOOST_TEST( mock_bars.size() == 0 );

   IBHistoricalService svc2;
   auto retVal2 = svc2.GetBars(msft, barSettings);
   auto bars = retVal2.get();
   BOOST_TEST( bars.size() > 0);
}

BOOST_AUTO_TEST_SUITE_END()