#include <list>
#include <vector>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(Count, ThreeIntsVector)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);
    
    EXPECT_EQ(3, rng.count());
}

TEST(Count, ThreeIntsList)
{
    std::list<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    auto rng = from(src);

    EXPECT_EQ(3, rng.count());
}

TEST(Count, FiveInts)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);

    auto rng = from(src);
    auto dst0 = rng.where([](int a){return a%2 == 0;});
    auto dst1 = rng.where([](int a){return a%2 == 1;});

    EXPECT_EQ(2, dst0.count());
    EXPECT_EQ(3, dst1.count());
}

TEST(Count, OddCount)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(0);
    src.push_back(1);
    src.push_back(0);
    src.push_back(1);

    auto rng = from(src);
    
    EXPECT_EQ(2, rng.count(0));
    EXPECT_EQ(3, rng.count(1));
}
