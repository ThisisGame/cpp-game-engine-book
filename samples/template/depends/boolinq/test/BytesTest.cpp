#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(Bytes, OneByteDefault)
{
    unsigned char src[] = {0xAA};
    int ans[] = {0xAA};

    auto rng = from(src);
    auto dst = rng.bytes();

    CheckRangeEqArray(dst, ans);
}

TEST(Bytes, OneByteFL)
{
    unsigned char src[] = {0xAA};
    int ans[] = {0xAA};

    auto rng = from(src);
    auto dst = rng.bytes(BytesFirstToLast);

    CheckRangeEqArray(dst, ans);
}

TEST(Bytes, OneByteLF)
{
    unsigned char src[] = {0xAA};
    int ans[] = {0xAA};

    auto rng = from(src);
    auto dst = rng.bytes(BytesLastToFirst);

    CheckRangeEqArray(dst, ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Bytes, OneIntDefault)
{
    int src[] = {0x12345678};
    int ans[] = {0x78,0x56,0x34,0x12};

    auto rng = from(src);
    auto dst = rng.bytes();

    CheckRangeEqArray(dst, ans);
}

TEST(Bytes, OneIntFL)
{
    int src[] = {0x12345678};
    int ans[] = {0x78,0x56,0x34,0x12};

    auto rng = from(src);
    auto dst = rng.bytes(BytesFirstToLast);

    CheckRangeEqArray(dst, ans);
}

TEST(Bytes, OneIntLF)
{
    int src[] = {0x12345678};
    int ans[] = {0x12,0x34,0x56,0x78};

    auto rng = from(src);
    auto dst = rng.bytes(BytesLastToFirst);

    CheckRangeEqArray(dst, ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Bytes, IntsDefault)
{
    unsigned src[] = {0x12345678, 0xAABBCCDD};
    int ans[] = 
    {
        0x78,0x56,0x34,0x12,
        0xDD,0xCC,0xBB,0xAA,
    };

    auto rng = from(src);
    auto dst = rng.bytes(BytesFirstToLast);

    CheckRangeEqArray(dst, ans);
}

TEST(Bytes, IntsFL)
{
    unsigned src[] = {0x12345678, 0xAABBCCDD};
    int ans[] = 
    {
        0x78,0x56,0x34,0x12,
        0xDD,0xCC,0xBB,0xAA,
    };

    auto rng = from(src);
    auto dst = rng.bytes(BytesFirstToLast);

    CheckRangeEqArray(dst, ans);
}

TEST(Bytes, IntsLF)
{
    unsigned src[] = {0x12345678, 0xAABBCCDD};
    int ans[] = 
    {
        0x12,0x34,0x56,0x78,
        0xAA,0xBB,0xCC,0xDD,
    };

    auto rng = from(src);
    auto dst = rng.bytes(BytesLastToFirst);

    CheckRangeEqArray(dst, ans);
}
