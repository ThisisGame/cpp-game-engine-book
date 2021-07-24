#include <list>
#include <deque>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <array>

#include <gtest/gtest.h>
#include "CommonTests.h"

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////

TEST(Ctor, ListInt)
{
    std::list<int> lst;
    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);
    lst.push_back(4);
    lst.push_back(5);

    int ans[] = {1,2,3,4,5};

    CheckRangeEqArray(from(lst), ans);
    CheckRangeEqArray(from(lst.begin(), lst.end()), ans);
    CheckRangeEqArray(from(lst.cbegin(), lst.cend()), ans);
}

TEST(Ctor, DequeInt)
{
    std::deque<int> dck;
    dck.push_back(1);
    dck.push_back(2);
    dck.push_back(3);
    dck.push_back(4);
    dck.push_back(5);
    
    int ans[] = {1,2,3,4,5};

    CheckRangeEqArray(from(dck), ans);
    CheckRangeEqArray(from(dck.begin(), dck.end()), ans);
    CheckRangeEqArray(from(dck.cbegin(), dck.cend()), ans);
}

TEST(Ctor, VectorInt)
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    
    int ans[] = {1,2,3,4,5};

    CheckRangeEqArray(from(vec), ans);
    CheckRangeEqArray(from(vec.begin(), vec.end()), ans);
    CheckRangeEqArray(from(vec.cbegin(), vec.cend()), ans);
}

TEST(Ctor, SetInt)
{
    std::set<int> set = {1,2,3,4,5};
    int ans[] = {1,2,3,4,5};

    CheckRangeEqSet(from(set), ans);
    CheckRangeEqSet(from(set.begin(), set.end()), ans);
    CheckRangeEqSet(from(set.cbegin(), set.cend()), ans);
}

//TEST(Ctor, MapInt)
//{
//    std::map<int, int> map = {{5,1},{4,2},{3,3},{2,4},{1,5}};
//    std::pair<int, int> ans[] = {{5,1},{4,2},{3,3},{2,4},{1,5}};
//
//    CheckRangeEqArray(from(map)
//        .orderBy([](std::pair<int,int> p){ return p.second; }), ans);
//    CheckRangeEqArray(from(map.begin(), map.end())
//        .orderBy([](std::pair<int,int> p){ return p.second; }), ans);
//    CheckRangeEqArray(from(map.cbegin(), map.cend())
//        .orderBy([](std::pair<int,int> p){ return p.second; }), ans);
//}

TEST(Ctor, StdArrayInt)
{
    std::array<int, 5> arr = { {1,2,3,4,5} };
    int ans[] = {1,2,3,4,5};

    CheckRangeEqArray(from(arr), ans);
    CheckRangeEqArray(from(arr.begin(), arr.end()), ans);
    CheckRangeEqArray(from(arr.cbegin(), arr.cend()), ans);
}

TEST(Ctor, ArrayInt)
{
    int arr[] = {1,2,3,4,5};
    int * ptr = static_cast<int *>(arr);
    
    int ans[] = {1,2,3,4,5};

    CheckRangeEqArray(from(arr), ans);
    CheckRangeEqArray(from(ptr, 5), ans);
    CheckRangeEqArray(from(ptr, ptr + 5), ans);
}

//////////////////////////////////////////////////////////////////////////

TEST(Ctor, OneElement)
{
    int src[] = {5};
    int ans[] = {5};

    auto rng = from(src);

    CheckRangeEqArray(rng, ans);
}

TEST(Ctor, EmptyVector)
{
    std::vector<int> src;
    
    auto rng = from(src);

    EXPECT_THROW(rng.next(), LinqEndException);
}
