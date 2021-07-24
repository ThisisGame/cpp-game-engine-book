#include <list>
#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(First, ThreeElements)
{
    std::vector<int> src = {1,2,3};

    EXPECT_EQ(1, from(src).first());
    EXPECT_EQ(2, from(src).first([](int a){return a%2 == 0;}));
    EXPECT_EQ(1, from(src).firstOrDefault());
    EXPECT_EQ(2, from(src).firstOrDefault([](int a){return a%2 == 0;}));
}

TEST(First, OneElement)
{
    std::vector<int> src = {1};

    EXPECT_EQ(1, from(src).first());
    EXPECT_THROW(from(src).first([](int a){return a%2 == 0;}), LinqEndException);
    EXPECT_EQ(1, from(src).firstOrDefault());
    EXPECT_EQ(0, from(src).firstOrDefault([](int a){return a%2 == 0;}));
}

TEST(First, NoneElements)
{
    std::vector<int> src = {};

    EXPECT_THROW(from(src).first(), LinqEndException);
    EXPECT_THROW(from(src).first([](int a){return a%2 == 0;}), LinqEndException);
    EXPECT_EQ(0, from(src).firstOrDefault());
    EXPECT_EQ(0, from(src).firstOrDefault([](int a){return a%2 == 0;}));
}

TEST(First, NoneElementsWithArg)
{
    std::vector<int> src = {};

    EXPECT_EQ(2, from(src).firstOrDefault(2));
    EXPECT_EQ(3, from(src).firstOrDefault([](int a){return a%2 == 0;}, 3));
}
