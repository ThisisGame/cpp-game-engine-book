#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(TakeWhile, ManyToMore)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int it){return it >= 0 && it <= 10;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToMoreByIndex)
{
    int src[] = {1,3,5,7,9,11};
    int ans[] = {1,3,5,7,9,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx >= 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToMoreByItemValue)
{
    int src[] = {1,3,5,7,9,11};
    int ans[] = {1,3,5,7,9,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it%2 != 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToMoreByIndexAndItemValue)
{
    int src[] = {1,3,5,7,9,11};
    int ans[] = {1,3,5,7,9,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return idx < 3 || it > 5;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhile, ManyToMany)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int it){return it >= 1 && it <= 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToManyByIndex)
{
    int src[] = {1,3,5,7,9,11};
    int ans[] = {1,3,5,7,9,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx >= 0 && idx < 6;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToManyByItemValue)
{
    int src[] = {1,3,5,7,9,11};
    int ans[] = {1,3,5,7,9,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it > 0 && it < 12;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToManyByIndexAndItemValue)
{
    int src[] = {1,3,5,7,9,11};
    int ans[] = {1,3,5,7,9,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return idx < 3 || (it > 5 && it < 12);});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhile, ManyToLess)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1,2,3};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int it){return it != 4;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToLessByIndex)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};
    int ans[] = {1,2,3,4};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx < 4;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToLessByItemValue)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};
    int ans[] = {1,2,3,4};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it <5;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToLessByIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};
    int ans[] = {1,2,3,4};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return idx < 4 && it > 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhile, ManyToOne)
{
    int src[] = {1,2,3,4,5,6};
    int ans[] = {1};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int it){return it%2 != 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToOneByIndex)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};
    int ans[] = {1};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx == 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToOneByItemValue)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};
    int ans[] = {1};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it == 1;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, ManyToOneByIndexAndItemValue)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};
    int ans[] = {1};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return idx < 5 && it < 2;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhile, ManyToZero)
{
    int src[] = {1,2,3,4,5,6};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int it){return it < 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhileRange_i, ManyToZeroByIndex)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx > 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhileRange_i, ManyToZeroByItemValue)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it > 2;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhileRange_i, ManyToZeroByIdexAndItemValue)
{
    int src[] = {1,2,3,4,5,6,7,8,9,10,11};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return it < 0 || idx > 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(TakeWhile, OneToOne)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int it){return it > 0;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, OneToOneByIndex)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx < 1;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, OneToOneByItemValue)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it > 2;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhileRange_i, OneToOneByIndexAndItemValue)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return idx == 0 && it > 2;});

    CheckRangeEqArray(dst, ans);
}

TEST(TakeWhile, OneToZero)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile([](int){return false;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhileRange_i, OneToZeroByIndex)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int idx){return idx > 0;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhileRange_i, OneToZeroByItemValue)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int /*idx*/){return it < 5;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhileRange_i, OneToZeroByIndexAndItemValue)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int it, int idx){return idx == 0 && it > 5;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

TEST(TakeWhile, ZeroToZero)
{
    std::vector<int> src;

    auto rng = from(src);
    auto dst = rng.takeWhile([](int){return false;});

    EXPECT_THROW(rng.next(), LinqEndException);
}

TEST(TakeWhileRange_i, ZeroToZero)
{
    int src[] = {5};

    auto rng = from(src);
    auto dst = rng.takeWhile_i([](int /*it*/, int /*idx*/){return false;});

    EXPECT_THROW(dst.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////
