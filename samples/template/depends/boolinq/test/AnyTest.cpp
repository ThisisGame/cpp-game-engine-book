#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Any, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_TRUE(rng.any());

    EXPECT_TRUE(rng.any([](int a){return a==1;}));
    EXPECT_TRUE(rng.any([](int a){return a==2;}));
    EXPECT_TRUE(rng.any([](int a){return a==3;}));
    EXPECT_TRUE(rng.any([](int a){return a>1;}));
    EXPECT_TRUE(rng.any([](int a){return a<3;}));
    EXPECT_TRUE(rng.any([](int a){return a!=2;}));

    EXPECT_FALSE(rng.any([](int a){return a==0;}));
    EXPECT_FALSE(rng.any([](int a){return a==4;}));
    EXPECT_FALSE(rng.any([](int a){return a<1;}));
    EXPECT_FALSE(rng.any([](int a){return a>3;}));
}
