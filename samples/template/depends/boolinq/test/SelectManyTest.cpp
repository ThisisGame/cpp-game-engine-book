#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(SelectMany, AxA)
{
    int src[] = {1,2,3};
    int ans[] = {1,2,2,3,3,3};

    auto rng = from(src);
    auto dst = rng.selectMany([](int a){return repeat(a, a);});

    CheckRangeEqArray(dst, ans);
}

TEST(SelectMany, OneTwoThree)
{
    int src[] = {1,2,3};
    int ans[] = {1,2,3,2,4,6,3,6,9};

    auto rng = from(src);
    auto dst = rng.selectMany([&src](int a){
        return from(src).select([a](int v){return a*v;});
    });

    CheckRangeEqArray(dst, ans);
}