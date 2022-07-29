#include <gtest/gtest.h>

#include <util/Configuration.hpp>

TEST(util_test, config )
{
    // the default configuration
    {
        util::Configuration* conf = util::Configuration::GetInstance();
        EXPECT_EQ( conf->GetIBHost(), "127.0.0.1");
        EXPECT_EQ( conf->GetIBPort(), 4001);
        EXPECT_EQ( conf->GetIBHistoricalConnectionId(), 1);
        EXPECT_EQ( conf->GetIBStreamingConnectionId(), 2);
        EXPECT_EQ( conf->GetIBAccountConnectionId(), 3);
    }
    // set it with a config file
    {
        util::Configuration* conf = util::Configuration::GetInstance("test_config.json");
        EXPECT_EQ( conf->GetIBHost(), "127.0.0.1");
        EXPECT_EQ( conf->GetIBPort(), 1);
        EXPECT_EQ( conf->GetIBHistoricalConnectionId(), 10);
        EXPECT_EQ( conf->GetIBStreamingConnectionId(), 11);
        EXPECT_EQ( conf->GetIBAccountConnectionId(), 12);
    }
    // once set, it does not reset
    {
        util::Configuration* conf = util::Configuration::GetInstance();
        EXPECT_EQ( conf->GetIBHost(), "127.0.0.1");
        EXPECT_EQ( conf->GetIBPort(), 1);
        EXPECT_EQ( conf->GetIBHistoricalConnectionId(), 10);
        EXPECT_EQ( conf->GetIBStreamingConnectionId(), 11);
        EXPECT_EQ( conf->GetIBAccountConnectionId(), 12);
    }
}
