#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(SkipWhile, ManyToMore)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it < 0 || it > 10;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToMoreByIndex)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx > 10;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToMoreByItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it < 0 || it > 10;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToMoreByIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx*it > 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhile, ManyToMany)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it < 0 && it > 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToManyByIndex)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx > 5;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToManyByItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it < 1 || it > 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToManyByIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx > 5 || it < 0;});

    CheckRangeEqArray(dst, ans);
}


TEST(SkipWhile, ManyToLess)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it < 3 || it > 4;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToLessByIndex)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx < 3 || idx > 3;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToLessByItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it < 3 || it > 4;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToLessByIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx*it < 7;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhile, ManyToOne)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it != 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToOneByIndex)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx < 5;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToOneByItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it < 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, ManyToOneByIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx*it < 30;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhile, ManyToZero)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it > 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhileRange_i, ManyToZeroeByIndex)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx < 6;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhileRange_i, ManyToZeroByItemValue)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it > 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhileRange_i, ManyToZeroIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx != it;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(SkipWhile, OneToOne)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it != 5;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, OneToOneByIndex)
{
    int src[] = {6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx > 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, OneToOneByItemValue)
{
    int src[] = {6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it != 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhileRange_i, OneToOneByIndexAndItemValue)
{
    int src[] = {6};
    int ans[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx != 0 || it != 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(SkipWhile, OneToZero)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.skipWhile([](int it){return it == 5;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhileRange_i, OneToZeroByIndex)
{
    int src[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int idx){return idx < 6;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhileRange_i, OneToZeroByItemValue)
{
    int src[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int /*idx*/){return it > 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhileRange_i, OneToZeroIndexAndItemValue)
{
    int src[] = {6};

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int it, int idx){return idx != it;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(SkipWhile, ZeroToZero)
{
    std::vector<int> src;

    auto rng = from(src);
    auto dst = rng.skipWhile([](int){return false;});

    EXPECT_THROW(rng.next(), LinqEndException);
}

TEST(SkipWhileRange_i, ZeroToZero)
{
    std::vector<int> src;

    auto rng = from(src);
    auto dst = rng.skipWhile_i([](int /*it*/, int /*idx*/){return true;});

    EXPECT_THROW(rng.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////
