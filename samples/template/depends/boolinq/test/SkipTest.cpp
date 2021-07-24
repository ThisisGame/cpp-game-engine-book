#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(Skip, ManyToMany)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skip(0);

    CheckRangeEqArray(dst, ans);
}

TEST(Skip, ManyToLess)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {4,5,6};

    auto rng = from(src);
    auto dst = rng.skip(3);

    CheckRangeEqArray(dst, ans);
}

TEST(Skip, ManyToOne)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skip(5);

    CheckRangeEqArray(dst, ans);
}

TEST(Skip, ManyToZero)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skip(6);

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(Skip, ManyToZeroLess)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skip(10);

    EXPECT_THROW(dst.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(Skip, OneToOne)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.skip(0);

    CheckRangeEqArray(dst, ans);
}

TEST(Skip, OneToZero)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.skip(1);

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(Skip, OneToZeroLess)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.skip(2);

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(Skip, ZeroToZero)
{
    std::vector<int> src;

    auto rng = from(src);
    auto dst = rng.skip(0);

    EXPECT_THROW(rng.next(), LinqEndException);
}

TEST(Skip, ZeroToZeroLess)
{
    std::vector<int> src;

    auto rng = from(src);
    auto dst = rng.skip(2);

    EXPECT_THROW(rng.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////
