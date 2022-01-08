#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(Select, Mul2)
{
    int src[] = {1,2,3,4};
    int ans[] = {2,4,6,8};

    auto rng = from(src);
    auto dst = rng.select([](int a){return a * 2;});

    CheckRangeEqArray(dst, ans);
}

TEST(Select, MakeChar)
{
    int src[] = {1,2,3,4};
    char ans[] = {'1','2','3','4'};

    auto rng = from(src);
    auto dst = rng.select([](int a){return static_cast<char>('0' + a);});

    CheckRangeEqArray(dst, ans);
}

TEST(Select, MakeString)
{
    int src[] = {1,2,3,4};

    static std::string ans[] =
    {
        "hello",
        "world",
        "apple",
        "intel",
    };

    auto rng = from(src);
    auto dst = rng.select([](int a){return ans[a-1];});

    CheckRangeEqArray(dst, ans);
}
