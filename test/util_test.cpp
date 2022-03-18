#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <util/Configuration.hpp>

BOOST_AUTO_TEST_SUITE ( util )

BOOST_AUTO_TEST_CASE( config )
{
    // the default configuration
    {
        Configuration* conf = Configuration::GetInstance();
        BOOST_CHECK_EQUAL( conf->GetIBHost(), "127.0.0.1");
        BOOST_CHECK_EQUAL( conf->GetIBPort(), 4001);
        BOOST_CHECK_EQUAL( conf->GetIBHistoricalConnectionId(), 1);
        BOOST_CHECK_EQUAL( conf->GetIBStreamingConnectionId(), 2);
        BOOST_CHECK_EQUAL( conf->GetIBAccountConnectionId(), 3);
    }
    // set it with a config file
    {
        Configuration* conf = Configuration::GetInstance("test_config.json");
        BOOST_CHECK_EQUAL( conf->GetIBHost(), "127.0.0.1");
        BOOST_CHECK_EQUAL( conf->GetIBPort(), 1);
        BOOST_CHECK_EQUAL( conf->GetIBHistoricalConnectionId(), 10);
        BOOST_CHECK_EQUAL( conf->GetIBStreamingConnectionId(), 11);
        BOOST_CHECK_EQUAL( conf->GetIBAccountConnectionId(), 12);
    }
    // once set, it does not reset
    {
        Configuration* conf = Configuration::GetInstance();
        BOOST_CHECK_EQUAL( conf->GetIBHost(), "127.0.0.1");
        BOOST_CHECK_EQUAL( conf->GetIBPort(), 1);
        BOOST_CHECK_EQUAL( conf->GetIBHistoricalConnectionId(), 10);
        BOOST_CHECK_EQUAL( conf->GetIBStreamingConnectionId(), 11);
        BOOST_CHECK_EQUAL( conf->GetIBAccountConnectionId(), 12);
    }
}

BOOST_AUTO_TEST_SUITE_END()