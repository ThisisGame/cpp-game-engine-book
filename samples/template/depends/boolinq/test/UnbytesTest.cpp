#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(Unbytes, OneIntDefault)
{
    unsigned char src[] = {0xAA,0xBB,0xCC,0xDD};
    unsigned ans[] = {0xDDCCBBAA};

    auto rng = from(src);
    auto dst = rng.unbytes<unsigned>();

    CheckRangeEqArray(dst, ans);
}

TEST(Unbytes, OneIntFL)
{
    unsigned char src[] = {0xAA,0xBB,0xCC,0xDD};
    unsigned ans[] = {0xDDCCBBAA};

    auto rng = from(src);
    auto dst = rng.unbytes<unsigned>(BytesFirstToLast);

    CheckRangeEqArray(dst, ans);
}

TEST(Unbytes, OneIntLF)
{
    unsigned char src[] = {0xAA,0xBB,0xCC,0xDD};
    unsigned ans[] = {0xAABBCCDD};

    auto rng = from(src);
    auto dst = rng.unbytes<unsigned>(BytesLastToFirst);

    CheckRangeEqArray(dst, ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Unbytes, TwoIntsDefault)
{
    unsigned char src[] = 
    {
        0x78,0x56,0x34,0x12,
        0xAA,0xBB,0xCC,0xDD,
    };
    unsigned ans[] = {0x12345678,0xDDCCBBAA};

    auto rng = from(src);
    auto dst = rng.unbytes<unsigned>();

    CheckRangeEqArray(dst, ans);
}

TEST(Unbytes, TwoIntsFL)
{
    unsigned char src[] = 
    {
        0x78,0x56,0x34,0x12,
        0xAA,0xBB,0xCC,0xDD,
    };
    unsigned ans[] = {0x12345678,0xDDCCBBAA};

    auto rng = from(src);
    auto dst = rng.unbytes<unsigned>(BytesFirstToLast);

    CheckRangeEqArray(dst, ans);
}

TEST(Unbytes, TwoIntsLF)
{
    unsigned char src[] = 
    {
        0x78,0x56,0x34,0x12,
        0xAA,0xBB,0xCC,0xDD,
    };
    unsigned ans[] = {0x78563412,0xAABBCCDD};

    auto rng = from(src);
    auto dst = rng.unbytes<unsigned>(BytesLastToFirst);

    CheckRangeEqArray(dst, ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Unbytes, EmptyDefault)
{
    std::vector<unsigned char> src;
    
    auto rng = from(src);
    auto dst = rng.unbytes<int>();

    EXPECT_THROW(dst.next(), LinqEndException);
}
