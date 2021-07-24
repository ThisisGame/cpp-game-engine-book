#include <list>
#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(ForEach, ThreeCharsSum)
{
    std::vector<char> src;
    src.push_back('a');
    src.push_back('b');
    src.push_back('c');

    std::string str = "";
    from(src).for_each([&](char a){str += a;});

    EXPECT_EQ("abc", str);
}

TEST(ForEach, ThreeCharsUpperSum)
{
    std::vector<char> src;
    src.push_back('a');
    src.push_back('b');
    src.push_back('c');

    std::string str = "";
    from(src).for_each([&](char a){str += a + ('A' - 'a');});

    EXPECT_EQ("ABC", str);
}

TEST(ForEach, ThreeIntsSum)
{
    std::vector<int> src;
    src.push_back(10);
    src.push_back(20);
    src.push_back(30);

    int sum = 0;
    from(src).for_each([&](int a){sum += a;});

    EXPECT_EQ(60, sum);
}
