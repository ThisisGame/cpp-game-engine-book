#include <list>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

TEST(ToStdList, List2List)
{
    std::list<int> src;
    src.push_back(100);
    src.push_back(200);
    src.push_back(300);

    auto rng = from(src);
    auto dst = rng.toStdList();

    EXPECT_EQ(dst,src);
}
