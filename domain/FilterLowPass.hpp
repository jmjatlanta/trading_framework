#pragma once
/*****
 * Copyright 2022 JMJAtlanta See LICENSE file for restrictions
 */
#include "Price.hpp"

/****
 * @brief a Low Pass filter
 * @param input the price series
 * @param period for calculating a
 * @returns a series where the first element has been filtered
 */
Price LowPass(const PriceSeries& input, int period)
{
    // we need at least 3
    if (input.size() < 3)
        return *input.begin();

    double a = 2.0/(1+period);
    // we are only interested in the last 3 items in the series
    auto itr = input.begin();
    auto p0 = *itr;
    auto p1 = *(++itr);
    auto p2 = *(++itr);
    // adjust the last item
    return p0 * (a-0.25*a*a)
            + p1 * 0.5*a*a
            - p2 * (a-0.75*a*a)
            + p1 * (2-2*a)
            - p2 * (1-a)*(1-a);
}