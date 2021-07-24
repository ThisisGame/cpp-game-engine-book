#include <list>
#include <deque>
#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(DotCall, BytesRange)
{
    unsigned src[] = {0x12345678,0xAABBCCDD};
    int ansFL[] = 
    {
        0x78,0x56,0x34,0x12,
        0xDD,0xCC,0xBB,0xAA,
    };
    int ansLF[] = 
    {
        0x12,0x34,0x56,0x78,
        0xAA,0xBB,0xCC,0xDD,
    };

    auto dstFL1 = from(src).bytes();
    auto dstFL2 = from(src).bytes(BytesFirstToLast);
    auto dstLF1 = from(src).bytes(BytesLastToFirst);

    CheckRangeEqArray(dstFL1, ansFL);
    CheckRangeEqArray(dstFL2, ansFL);
    CheckRangeEqArray(dstLF1, ansLF);
}            

//////////////////////////////////////////////////////////////////////////

TEST(DotCall, UnbytesRange)
{
    unsigned char src[] = 
    {
        0x78,0x56,0x34,0x12,
        0xDD,0xCC,0xBB,0xAA,
    };
    unsigned ansFL[] = {0x12345678,0xAABBCCDD};
    unsigned ansLF[] = {0x78563412,0xDDCCBBAA};

    auto dstFL1 = from(src).unbytes<unsigned>();
    auto dstFL2 = from(src).unbytes<unsigned>(BytesFirstToLast);
    auto dstLF1 = from(src).unbytes<unsigned>(BytesLastToFirst);

    CheckRangeEqArray(dstFL1, ansFL);
    CheckRangeEqArray(dstFL2, ansFL);
    CheckRangeEqArray(dstLF1, ansLF);
} 

//////////////////////////////////////////////////////////////////////////

TEST(DotCall, BitsRangeHL)
{
    unsigned src[] = {0xAABBCCDD};
    int ansFL[] = 
    {
        1,1,0,1,1,1,0,1,
        1,1,0,0,1,1,0,0,
        1,0,1,1,1,0,1,1,
        1,0,1,0,1,0,1,0,
    };
    int ansLF[] = 
    {
        1,0,1,0,1,0,1,0,
        1,0,1,1,1,0,1,1,
        1,1,0,0,1,1,0,0,
        1,1,0,1,1,1,0,1,
    };

    auto dstFL1 = from(src).bits();
    auto dstFL2 = from(src).bits(BitsHighToLow);
    auto dstFL3 = from(src).bits(BitsHighToLow, BytesFirstToLast);
    auto dstLF1 = from(src).bits(BitsHighToLow, BytesLastToFirst);

    CheckRangeEqArray(dstFL1, ansFL);
    CheckRangeEqArray(dstFL2, ansFL);
    CheckRangeEqArray(dstFL3, ansFL);
    CheckRangeEqArray(dstLF1, ansLF);
}

TEST(DotCall, BitsRangeLH)
{
    unsigned src[] = {0xAABBCCDD};
    int ansFL[] = 
    {
        1,0,1,1,1,0,1,1,
        0,0,1,1,0,0,1,1,
        1,1,0,1,1,1,0,1,
        0,1,0,1,0,1,0,1,
    };
    int ansLF[] = 
    {
        0,1,0,1,0,1,0,1,
        1,1,0,1,1,1,0,1,
        0,0,1,1,0,0,1,1,
        1,0,1,1,1,0,1,1,
    };

    auto dstFL1 = from(src).bits(BitsLowToHigh);
    auto dstFL2 = from(src).bits(BitsLowToHigh, BytesFirstToLast);
    auto dstLF1 = from(src).bits(BitsLowToHigh, BytesLastToFirst);

    CheckRangeEqArray(dstFL1, ansFL);
    CheckRangeEqArray(dstFL2, ansFL);
    CheckRangeEqArray(dstLF1, ansLF);
}

//////////////////////////////////////////////////////////////////////////

TEST(DotCall, UnbitsRangeHLFL)
{
    int src[] =
    {
        1,1,0,1,1,1,0,1,
        1,1,0,0,1,1,0,0,
        1,0,1,1,1,0,1,1,
        1,0,1,0,1,0,1,0
    };
    int ans_4b[] = {0xDD,0xCC,0xBB,0xAA};
    unsigned ans_1i[] = {0xAABBCCDD};
    unsigned ansLF_1i[] = {0xDDCCBBAA};

    auto dst1_4b = from(src).unbits();
    auto dst2_4b = from(src).unbits(BitsHighToLow);
    auto dst1_1i = from(src).unbits<unsigned>(BitsHighToLow);
    auto dst2_1i = from(src).unbits<unsigned>(BitsHighToLow, BytesFirstToLast);
    auto dst3_1i = from(src).unbits<unsigned>(BitsHighToLow, BytesLastToFirst);

    CheckRangeEqArray(dst1_4b, ans_4b);
    CheckRangeEqArray(dst2_4b, ans_4b);
    CheckRangeEqArray(dst1_1i, ans_1i);
    CheckRangeEqArray(dst2_1i, ans_1i);
    CheckRangeEqArray(dst3_1i, ansLF_1i);
}

//TEST(DotCall, UnbitsRangeLH)
//{
//    int src[] = {0xAABBCCDD};
//    int ansFL[] = 
//    {
//        1,0,1,1,1,0,1,1,
//        0,0,1,1,0,0,1,1,
//        1,1,0,1,1,1,0,1,
//        0,1,0,1,0,1,0,1,
//    };
//    int ansLF[] = 
//    {
//        0,1,0,1,0,1,0,1,
//        1,1,0,1,1,1,0,1,
//        0,0,1,1,0,0,1,1,
//        1,0,1,1,1,0,1,1,
//    };
//
//    auto dstFL1 = from(src).bits<BitsLowToHigh>();
//    auto dstFL2 = from(src).bits<BitsLowToHigh,BytesFirstToLast>();
//    auto dstLF1 = from(src).bits<BitsLowToHigh,BytesLastToFirst>();
//
//    CheckRangeEqArray(dstFL1, ansFL);
//    CheckRangeEqArray(dstFL2, ansFL);
//    CheckRangeEqArray(dstLF1, ansLF);
//}

