#include <list>
#include <vector>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Sum, ThreeInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src).sum();

    EXPECT_EQ(6, rng);
}

TEST(Sum, FiveInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);

    auto rng = from(src);
    auto dst0 = rng.where([](int a){return a%2 == 0;}).sum();
    auto dst1 = rng.where([](int a){return a%2 == 1;}).sum();

    EXPECT_EQ(6, dst0);
    EXPECT_EQ(9, dst1);
}

TEST(Sum, TransformSum)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);

    auto rng1 = from(src).sum([](int a){return a/2;});
    auto rng2 = from(src).sum([](int a){return a%2;});

    EXPECT_EQ(6, rng1);
    EXPECT_EQ(3, rng2);
}

TEST(Sum, FiveStringsLen)
{
    std::vector<std::string> src;
    src.push_back("hello");
    src.push_back("apple");
    src.push_back("nokia");
    src.push_back("oracle");
    src.push_back("ponny");

    auto rng = from(src).sum([](const std::string & str){return str.size();});

    EXPECT_EQ(26U, rng);
}

TEST(Sum, FiveStringsData)
{
    std::vector<std::string> src;
    src.push_back("hello");
    src.push_back("apple");
    src.push_back("nokia");
    src.push_back("oracle");
    src.push_back("ponny");

    auto rng = from(src).sum();

    std::string ans = "helloapplenokiaoracleponny";

    EXPECT_EQ(ans, rng);
}

TEST(Sum, TransfromStringSum)
{
    std::vector<std::string> src;
    src.push_back("hello");
    src.push_back("apple");
    src.push_back("nokia");
    src.push_back("oracle");
    src.push_back("ponny");

    auto sum = from(src).sum([](std::string s) { return s.size(); });

    EXPECT_EQ(26, sum);
}
