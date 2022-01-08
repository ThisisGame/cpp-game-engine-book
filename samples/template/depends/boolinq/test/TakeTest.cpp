#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(Take, ManyToMore)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.take(10);

    CheckRangeEqArray(dst, ans);
}

TEST(Take, ManyToMany)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.take(6);

    CheckRangeEqArray(dst, ans);
}

TEST(Take, ManyToLess)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3};

    auto rng = from(src);
    auto dst = rng.take(3);

    CheckRangeEqArray(dst, ans);
}

TEST(Take, ManyToOne)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1};

    auto rng = from(src);
    auto dst = rng.take(1);

    CheckRangeEqArray(dst, ans);
}

TEST(Take, ManyToZero)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.take(0);

    EXPECT_THROW(dst.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(Take, OneToMore)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.take(5);

    CheckRangeEqArray(dst, ans);
}

TEST(Take, OneToOne)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.take(1);

    CheckRangeEqArray(dst, ans);
}

TEST(Take, OneToZero)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.take(0);

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(Take, ZeroToZero)
{
    std::vector<int> src;

    auto rng = from(src);
    auto dst = rng.take(0);

    EXPECT_THROW(rng.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////
