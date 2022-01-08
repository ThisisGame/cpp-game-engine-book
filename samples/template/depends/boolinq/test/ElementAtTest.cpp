#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(ElementAt, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_EQ(1, rng.elementAt(0));
    EXPECT_EQ(2, rng.elementAt(1));
    EXPECT_EQ(3, rng.elementAt(2));
}

TEST(ElementAt, FiveStrings)
{
    std::vector<std::string> src;
    src.push_back("hello");
    src.push_back("apple");
    src.push_back("nokia");
    src.push_back("oracle");
    src.push_back("ponny");

    auto rng = from(src);

    EXPECT_EQ("hello",  rng.elementAt(0));
    EXPECT_EQ("apple",  rng.elementAt(1));
    EXPECT_EQ("nokia",  rng.elementAt(2));
    EXPECT_EQ("oracle", rng.elementAt(3));
    EXPECT_EQ("ponny",  rng.elementAt(4));
}
