#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

TEST(Range, OneToFive)
{
    int ans[] = {1,2,3,4};

    CheckRangeEqArray(range(1,5,1), ans);
}

TEST(Range, OneToFiveStep2)
{
    int ans[] = {1,3};

    CheckRangeEqArray(range(1,5,2), ans);
}