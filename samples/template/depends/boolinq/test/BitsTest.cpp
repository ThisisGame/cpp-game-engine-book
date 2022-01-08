#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(Bits, OneByteDefault)
{
    unsigned char src[] = {0xAA};
    int ans[] = {1,0,1,0,1,0,1,0};

    auto rng = from(src);
    auto dst = rng.bits();

    CheckRangeEqArray(dst, ans);
}

TEST(Bits, OneByteHL)
{
    unsigned char src[] = {0xAA};
    int ans[] = {1,0,1,0,1,0,1,0};

    auto rng = from(src);
    auto dst = rng.bits(BitsHighToLow);

    CheckRangeEqArray(dst, ans);
}

TEST(Bits, OneByteLH)
{
    unsigned char src[] = {0xAA};
    int ans[] = {0,1,0,1,0,1,0,1};

    auto rng = from(src);
    auto dst = rng.bits(BitsLowToHigh);

    CheckRangeEqArray(dst, ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Bits, OneIntDefault)
{
    unsigned int src[] = {0x12345678};
    int ans[] = 
    {
        0,1,1,1,1,0,0,0,
        0,1,0,1,0,1,1,0,
        0,0,1,1,0,1,0,0,
        0,0,0,1,0,0,1,0,
    };

    auto rng = from(src);
    auto dst = rng.bits();

    CheckRangeEqArray(dst, ans);
}

TEST(Bits, OneIntHL)
{
    unsigned int src[] = {0x12345678};
    int ans[] = 
    {
        0,1,1,1,1,0,0,0,
        0,1,0,1,0,1,1,0,
        0,0,1,1,0,1,0,0,
        0,0,0,1,0,0,1,0,
    };

    auto rng = from(src);
    auto dst = rng.bits(BitsHighToLow);

    CheckRangeEqArray(dst, ans);
}

TEST(Bits, OneIntLH)
{
    unsigned int src[] = {0x12345678};
    int ans[] = 
    {
        0,0,0,1,1,1,1,0,
        0,1,1,0,1,0,1,0,
        0,0,1,0,1,1,0,0,
        0,1,0,0,1,0,0,0,
    };

    auto rng = from(src);
    auto dst = rng.bits(BitsLowToHigh,BytesFirstToLast);
    auto vvv = dst.toStdVector();

    CheckRangeEqArray(dst, ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Bits, IntsDefault)
{
    unsigned int src[] = {0x12345678,0xAABBCCDD};
    int ans[] = 
    {
        0,1,1,1,1,0,0,0, // 78
        0,1,0,1,0,1,1,0, // 56
        0,0,1,1,0,1,0,0, // 34
        0,0,0,1,0,0,1,0, // 12

        1,1,0,1,1,1,0,1, // DD
        1,1,0,0,1,1,0,0, // CC
        1,0,1,1,1,0,1,1, // BB
        1,0,1,0,1,0,1,0, // AA
    };

    auto rng = from(src);
    auto dst = rng.bits();

    CheckRangeEqArray(dst, ans);
}

TEST(Bits, IntsHL)
{
    unsigned int src[] = {0x12345678,0xAABBCCDD};
    int ans[] = 
    {
        0,1,1,1,1,0,0,0, // 78
        0,1,0,1,0,1,1,0, // 56
        0,0,1,1,0,1,0,0, // 34
        0,0,0,1,0,0,1,0, // 12

        1,1,0,1,1,1,0,1, // DD
        1,1,0,0,1,1,0,0, // CC
        1,0,1,1,1,0,1,1, // BB
        1,0,1,0,1,0,1,0, // AA
    };

    auto rng = from(src);
    auto dst = rng.bits(BitsHighToLow);

    CheckRangeEqArray(dst, ans);
}

TEST(Bits, IntsLH)
{
    unsigned int src[] = {0x12345678,0xAABBCCDD};
    int ans[] = 
    {
        0,0,0,1,1,1,1,0, // -87
        0,1,1,0,1,0,1,0, // -65
        0,0,1,0,1,1,0,0, // -43
        0,1,0,0,1,0,0,0, // -21

        1,0,1,1,1,0,1,1, // -DD
        0,0,1,1,0,0,1,1, // -CC
        1,1,0,1,1,1,0,1, // -BB
        0,1,0,1,0,1,0,1, // -AA
    };

    auto rng = from(src);
    auto dst = rng.bits(BitsLowToHigh);

    CheckRangeEqArray(dst, ans);
}
