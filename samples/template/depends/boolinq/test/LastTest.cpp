#include <list>
#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Last, ThreeElements)
{
    std::vector<int> src = {1,2,3};

    EXPECT_EQ(3, from(src).last());
    EXPECT_EQ(2, from(src).last([](int a){return a%2 == 0;}));
    EXPECT_EQ(3, from(src).lastOrDefault());
    EXPECT_EQ(2, from(src).lastOrDefault([](int a){return a%2 == 0;}));
}

TEST(Last, OneElement)
{
    std::vector<int> src = {1};

    EXPECT_EQ(1, from(src).last());
    EXPECT_THROW(from(src).last([](int a){return a%2 == 0;}), LinqEndException);
    EXPECT_EQ(1, from(src).lastOrDefault());
    EXPECT_EQ(0, from(src).lastOrDefault([](int a){return a%2 == 0;}));
}

TEST(Last, NoneElements)
{
    std::vector<int> src = {};

    EXPECT_THROW(from(src).last(), LinqEndException);
    EXPECT_THROW(from(src).last([](int a){return a%2 == 0;}), LinqEndException);
    EXPECT_EQ(0, from(src).lastOrDefault());
    EXPECT_EQ(0, from(src).lastOrDefault([](int a){return a%2 == 0;}));
}

TEST(Last, NoneElementsWithArgument) {
    std::vector<int> src = {};

    EXPECT_EQ(2, from(src).lastOrDefault(2));
    EXPECT_EQ(3, from(src).lastOrDefault([](int a) { return a % 2 == 0; }, 3));
}
