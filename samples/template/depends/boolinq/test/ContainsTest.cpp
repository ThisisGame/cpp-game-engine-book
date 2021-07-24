#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Contains, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_TRUE(rng.contains(1));
    EXPECT_TRUE(rng.contains(2));
    EXPECT_TRUE(rng.contains(3));

    EXPECT_FALSE(rng.contains(0));
    EXPECT_FALSE(rng.contains(4));
}

TEST(Contains, FiveStrings)
{
    std::vector<std::string> src;
    src.push_back("hello");
    src.push_back("apple");
    src.push_back("nokia");
    src.push_back("oracle");
    src.push_back("ponny");

    auto rng = from(src);

    EXPECT_TRUE(rng.contains("hello"));
    EXPECT_TRUE(rng.contains("apple"));
    EXPECT_TRUE(rng.contains("nokia"));
    EXPECT_TRUE(rng.contains("oracle"));
    EXPECT_TRUE(rng.contains("ponny"));

    EXPECT_FALSE(rng.contains("dino"));
    EXPECT_FALSE(rng.contains("lord"));
}
