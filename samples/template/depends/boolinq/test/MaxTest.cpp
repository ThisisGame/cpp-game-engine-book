#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Max, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_EQ(3, rng.max());
    EXPECT_EQ(1, rng.max([](int a){return -a;}));
}

TEST(Max, FiveStrings)
{
    std::vector<std::string> src;
    src.push_back("hell");
    src.push_back("apple");
    src.push_back("zip");

    auto rng = from(src);

    EXPECT_EQ("zip",  rng.max());
    EXPECT_EQ("apple", rng.max([](std::string s){return s.size();}));
}
