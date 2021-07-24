#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(Concat, ArrayArray)
{
    int src1[] = {1,2,3,4,5};
    int src2[] = {6,7,8,9};

    int ans[] = {1,2,3,4,5,6,7,8,9};

    auto rng1 = from(src1);
    auto rng2 = from(src2);
    auto dst = rng1.concat(rng2);
             
    CheckRangeEqArray(dst, ans);
}

TEST(Concat, ArrayVector)
{
    int src1[] = {1,2,3,4,5};
    std::vector<int> src2;
    src2.push_back(6);
    src2.push_back(7);
    src2.push_back(8);
    src2.push_back(9);

    int ans[] = {1,2,3,4,5,6,7,8,9};

    auto rng1 = from(src1);
    auto rng2 = from(src2);
    auto dst = rng1.concat(rng2);

    CheckRangeEqArray(dst, ans);
}

TEST(Concat, ArrayVectorArray)
{
    int src1[] = {1,2,3,4,5};
    std::vector<int> src2;
    src2.push_back(6);
    src2.push_back(7);
    src2.push_back(8);
    src2.push_back(9);
    int src3[] = {10,11,12,13,14,15};

    int ans[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    auto rng1 = from(src1);
    auto rng2 = from(src2);
    auto rng3 = from(src3);
    auto dst = rng1.concat(rng2).concat(rng3);

    CheckRangeEqArray(dst, ans);
}
