#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(Append, ThreePlusOne)
{
    std::vector<int> src = {1,2,3};

    auto rng = from(src).append(4);
    int ans[] = {1,2,3,4};

    CheckRangeEqArray(rng, ans);
}

TEST(Append, ThreePlusTwo)
{
    std::vector<int> src = {1,2,3};

    auto rng = from(src).append(4, 5);
    int ans[] = {1,2,3,4,5};

    CheckRangeEqArray(rng, ans);
}

TEST(Append, ZeroPlusTwo)
{
    std::vector<int> src;

    auto rng = from(src).append(7,8);
    int ans[] = {7,8};

    CheckRangeEqArray(rng, ans);
}