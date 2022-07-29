#include <gtest/gtest.h>

#include "domain/Price.hpp"
#include "domain/FilterLowPass.hpp"

TEST(test_domain, price_test)
{
    {
        // whole numbers
        Price p(1);
        EXPECT_EQ(p.to_double(), 1.0);
        EXPECT_EQ(p.to_string(), "1");
        Price d(2);
        EXPECT_EQ( d / p, d);
        EXPECT_NE( p / d, Price(0.5, 1));
        EXPECT_EQ( p + d, Price(3) );    
        EXPECT_EQ( d + p, Price(3) );
        EXPECT_EQ( d - p, p);
        EXPECT_EQ( p - d, Price(-1));   
        EXPECT_EQ( d * p, d);
        EXPECT_EQ( p * d, d);
    }
    {
        // floating point numbers
        Price p(1.0, 2);
        EXPECT_EQ(p.to_double(), 1.0);
        EXPECT_EQ(p.to_string(), "1.00");
        Price p2(2.0, 2);
        EXPECT_EQ( p + p2, Price(3));
        EXPECT_EQ( p2 + p, Price(3));
        EXPECT_EQ( p2 - p, p);
        EXPECT_EQ( p - p2, Price(-1));
        EXPECT_EQ( p * p2, p2);
        EXPECT_EQ( p2 * p, p2);
        EXPECT_EQ( p / p2, Price(50, 2, false));
        EXPECT_EQ( p / p2, Price(.5, 2));
        EXPECT_EQ( p2 / p, p2 );
    }
    {
        // float math
        Price p1(2.0, 2);
        EXPECT_EQ( p1 * 2.0, Price(4.0, 2));
        EXPECT_EQ( p1 / 2.0, Price(1.0, 2));
    }
    {
        PriceSeries prices;
        for(size_t i = 0; i < 100; ++i)
            prices.push_back(i);
    }
}

TEST(test_domain, LowPassFilter)
{
    Price last(10, 2);
    bool increase = true;
    PriceSeries real;
    PriceSeries filtered;
    for(int i = 0; i < 100; ++i)
    {
        if (increase)
            last += .1;
        else
            last -= .1;
        real.insert(real.begin(), last );
        if ( i % 5 == 0)
            increase = !increase;
        filtered.insert(filtered.begin(), LowPass(real, 3));
    }
    // display the results:
    std::cout << "Price       Filtered\n";
    auto realItr = real.rbegin();
    auto filteredItr = filtered.rbegin();
    while( realItr != real.rend() )
    {
        std::cout << (*realItr).to_string()
                << "   " << (*filteredItr).to_string()
                << "\n";
        realItr++;
        filteredItr++;
    }
}