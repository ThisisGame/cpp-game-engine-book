#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(All, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_TRUE(rng.all());
    
    EXPECT_TRUE(rng.all([](int a){return a>0;}));
    EXPECT_TRUE(rng.all([](int a){return a<4;}));
    EXPECT_TRUE(rng.all([](int a){return a>0 && a<4;}));

    EXPECT_FALSE(rng.all([](int a){return a>2;}));
    EXPECT_FALSE(rng.all([](int a){return a==1;}));
    EXPECT_FALSE(rng.all([](int a){return a<3;}));
}

TEST(All, ThreeIntsSecond)
{
    std::vector<int> src;
    src.push_back(0);
    src.push_back(1);
    src.push_back(2);

    auto rng = from(src);

    EXPECT_FALSE(rng.all());
    
    EXPECT_TRUE(rng.all([](int a){return a>=0;}));
    EXPECT_TRUE(rng.all([](int a){return a<=2;}));
    EXPECT_TRUE(rng.all([](int a){return a>=0 && a<=2;}));

    EXPECT_FALSE(rng.all([](int a){return a>1;}));
    EXPECT_FALSE(rng.all([](int a){return a==1;}));
    EXPECT_FALSE(rng.all([](int a){return a<2;}));
}
