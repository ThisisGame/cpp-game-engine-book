#include <vector>
#include <string>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(GroupBy, Ints)
{
    int arr[] = {0,1,2,3,4,5,6,7,8,9};

    int ans_0[] = {0,3,6,9};
    int ans_1[] = {1,4,7};
    int ans_2[] = {2,5,8};

    auto dst = from(arr).groupBy([](int a){return a % 3;});

    EXPECT_EQ(0, dst.elementAt(0).first);
    EXPECT_EQ(1, dst.elementAt(1).first);
    EXPECT_EQ(2, dst.elementAt(2).first);
    CheckRangeEqArray(dst.elementAt(0).second, ans_0);
    CheckRangeEqArray(dst.elementAt(1).second, ans_1);
    CheckRangeEqArray(dst.elementAt(2).second, ans_2);

    EXPECT_THROW(dst.elementAt(3), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(GroupBy, CountChildrenByAge)
{
    struct Child
    {
        std::string name;
        int age;

        bool operator == (const Child & rhs) const
        {
            return (name == rhs.name) && (age == rhs.age);
        }
    };  

    Child children[] = {
        {"Piter", 12},
        {"Bella", 14},
        {"Torry", 15},
        {"Holly", 12},
        {"Zamza", 13},
    };

    Child ans_false[] = {
        {"Bella", 14},
        {"Torry", 15},
    };

    Child ans_true[] = {
        {"Piter", 12},
        {"Holly", 12},
        {"Zamza", 13},
    };

    auto dst = from(children).groupBy([](const Child & a){return a.age < 14;});

    EXPECT_EQ(true, dst.elementAt(0).first);
    EXPECT_EQ(false, dst.elementAt(1).first);
    CheckRangeEqArray(dst.elementAt(0).second, ans_true);
    CheckRangeEqArray(dst.elementAt(1).second, ans_false);

    EXPECT_THROW(dst.elementAt(2), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(GroupBy, CountChildrenByName)
{
    struct Human
    {
        std::string name;
        int age;

        bool operator == (const Human & rhs) const
        {
            return (name == rhs.name) && (age == rhs.age);
        }
    };

    std::vector<Human> people = {
        {"Kevin", 14},
        {"Kevin", 24},
        {"Kevin", 34},
        {"Kevin", 44},
        {"Anton", 18},
        {"Agata", 17},
        {"Terra", 20},
        {"Terra", 21},
        {"Layer", 15},
    };

    Human ans_1[] = {
        {"Kevin", 14},
        {"Kevin", 24},
        {"Kevin", 34},
        {"Kevin", 44},
    };

    Human ans_2[] = {
        {"Anton", 18},
    };

    Human ans_3[] = {
        {"Agata", 17},
    };

    Human ans_4[] = {
        {"Terra", 20},
        {"Terra", 21},
    };

    Human ans_5[] = {
        {"Layer", 15},
    };

    auto dst = from(people).groupBy([](const Human & a){return a.name;});

    EXPECT_EQ("Kevin", dst.elementAt(0).first);
    EXPECT_EQ("Anton", dst.elementAt(1).first);
    EXPECT_EQ("Agata", dst.elementAt(2).first);
    EXPECT_EQ("Terra", dst.elementAt(3).first);
    EXPECT_EQ("Layer", dst.elementAt(4).first);
    CheckRangeEqArray(dst.elementAt(0).second, ans_1);
    CheckRangeEqArray(dst.elementAt(1).second, ans_2);
    CheckRangeEqArray(dst.elementAt(2).second, ans_3);
    CheckRangeEqArray(dst.elementAt(3).second, ans_4);
    CheckRangeEqArray(dst.elementAt(4).second, ans_5);

    EXPECT_THROW(dst.elementAt(5), LinqEndException);
}
