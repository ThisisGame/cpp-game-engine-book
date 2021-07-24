#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Min, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_EQ(1, rng.min());
    EXPECT_EQ(3, rng.min([](int a){return -a;}));
}

TEST(Min, FiveStrings)
{
    std::vector<std::string> src;
    src.push_back("hell");
    src.push_back("apple");
    src.push_back("zip");

    auto rng = from(src);

    EXPECT_EQ("apple",  rng.min());
    EXPECT_EQ("zip",  rng.min([](const std::string & s){return s.size();}));
}
