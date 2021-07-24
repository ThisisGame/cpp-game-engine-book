#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(Prepend, ThreePlusOne)
{
    std::vector<int> src = {1,2,3};

    auto rng = from(src).prepend(4);
    int ans[] = {4,1,2,3};

    CheckRangeEqArray(rng, ans);
}

TEST(Prepend, ThreePlusTwo)
{
    std::vector<int> src = {1,2,3};

    auto rng = from(src).prepend(4, 5);
    int ans[] = {4,5,1,2,3};

    CheckRangeEqArray(rng, ans);
}

TEST(Prepend, ZeroPlusTwo)
{
    std::vector<int> src;

    auto rng = from(src).prepend(7,8);
    int ans[] = {7,8};

    CheckRangeEqArray(rng, ans);
}