#include <list>
#include <deque>
#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(Reverse, IntVector)
{
    int src[] = {1,2,3,4};
    int ans[] = {4,3,2,1};

    auto rng = from(src);
    auto dst = rng.reverse();

    CheckRangeEqArray(dst, ans);
}

TEST(Reverse, DoubleReverse)
{
    int src[] = {1,2,3,4};
    int ans[] = {1,2,3,4};

    auto rng = from(src);
    auto dst = rng.reverse().reverse();

    CheckRangeEqArray(dst, ans);
}
