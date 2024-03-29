#include <gtest/gtest.h>

#include <iostream>
#include <ib/IBWrapper.h>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>
#include <ib/IBAccountService.hpp>
#include <market_data/historical/HistoricalService.hpp>
#include <market_data/streaming/StreamingService.hpp>
#include <market_data/BarSettings.hpp>
#include <domain/Contract.hpp>
#include <domain/Order.hpp>
#include <util/Configuration.hpp>

class MockHistoricalService : public market_data::HistoricalService<MockHistoricalService>
{
   public:
   std::future<std::vector<market_data::OHLCBar>> GetBars(tf::Contract cont, tf::BarSettings span)
   {
      return std::async(&MockHistoricalService::do_nothing, this, cont, span);
   }
   virtual std::optional<double> LastBidPrice(tf::Contract contract) { return 0.0; }
   virtual std::optional<double> LastAskPrice(tf::Contract contract) { return 0.0; }
   virtual std::optional<double> SMA(uint16_t numBars, tf::Contract contract, bool market_hours = true) { return 0.0; }

   private:
   std::vector<market_data::OHLCBar> do_nothing(tf::Contract cont, tf::BarSettings span)
   {
      std::vector<market_data::OHLCBar> retVal;
      return retVal;
   }
};

TEST(ibwrapper_test, login )
{
   try
   {
      ib::IBWrapper ibw("localhost", 4001, 1);
   }
   catch(...)
   {
      std::cerr << "ib/login: immediate creation and deletion threw exception\n";
      FAIL();
   }
}

TEST(ibwrapper_test, generic_historical_ifc )
{
   // Grab the last 4 days of MSFT bars
   tf::Stock msft("MSFT");
   tf::BarSettings barSettings;
   barSettings.duration = std::chrono::hours(96);
   barSettings.barTimeSpan = tf::BarTimeSpan::FIVE_MINUTES;

   MockHistoricalService svc;
   auto retVal = svc.GetBars(msft, barSettings);
   auto mock_bars = retVal.get();
   EXPECT_EQ( mock_bars.size(), 0 );

   util::Configuration* conf = util::Configuration::GetInstance("test_config.json");
   {
       /*
    IBHistoricalService svc2( IBConfiguration(conf->GetIBHost(), conf->GetIBPort(), conf->GetIBHistoricalConnectionId()));
    auto retVal2 = svc2.GetBars(msft, barSettings);
    auto bars = retVal2.get();
    BOOST_TEST( bars.size() > 0);
        */
   }

    {
        tf::Stock rog("ROG");
        ib::IBHistoricalService svc2( IBConfiguration(conf->GetIBHost(), 
                conf->GetIBPort(), conf->GetIBHistoricalConnectionId()));
        barSettings.barTimeSpan = tf::BarTimeSpan::ONE_DAY;
        barSettings.duration = std::chrono::hours(2160); // 3 mos
        auto retVal2 = svc2.GetBars(rog, barSettings);
        auto bars = retVal2.get();
        EXPECT_GT( bars.size(), 0);
        // output to csv
        for(auto b : bars)
        {
            std::cout << b.toCSV() << "\n";
        }
    }
}

TEST(ibwrapper_test, streaming_test )
{
   tf::Stock msft("MSFT");

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

   ib::IBStreamingService ib(IBConfiguration("127.0.0.1", 4007, 1));
   ib.GetTimeAndSales(msft, func );
   std::this_thread::sleep_for( std::chrono::seconds(3) );
   EXPECT_TRUE( atLeastOne );
}

TEST(ibwrapper_test, book_test )
{
   /**
    * Note: This requires subscription to L2
    */
   tf::Stock codx("T");
   codx.exchange = "ARCA";
   bool atLeastOne = false;

   auto func = [&atLeastOne](const market_data::BookMessage msg) {
      atLeastOne = true;
   };

   ib::IBStreamingService ib( IBConfiguration("127.0.0.1", 4007, 1));
   ib.GetBookData(codx, func);
   std::this_thread::sleep_for( std::chrono::seconds(3) );
   EXPECT_TRUE( atLeastOne );
}

TEST(ibwrapper_test, account_test )
{
   ib::IBAccountService acctSvc( IBConfiguration("127.0.0.1", 4007, 1));
   std::vector<tf::Order> orders = acctSvc.GetOpenOrders(1);
   
}
