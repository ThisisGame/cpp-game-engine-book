#pragma once

#include <set>

#include "boolinq.h"

using namespace boolinq;

//////////////////////////////////////////////////////////////////////////
// Compare Range with array
//////////////////////////////////////////////////////////////////////////

template<typename R, typename T, unsigned N, typename F>
void CheckRangeEqArray(R dst, T (&ans)[N], F f)
{
    for (unsigned i = 0; i < N; i++)
        EXPECT_EQ(f(ans[i]), f(dst.next()));

    EXPECT_THROW(dst.next(), LinqEndException);
}

template<typename R, typename T, unsigned N>
void CheckRangeEqArray(R dst, T (&ans)[N])
{
    for (unsigned i = 0; i < N; i++)
        EXPECT_EQ(ans[i], dst.next());

    EXPECT_THROW(dst.next(), LinqEndException);
}

template<typename T, unsigned N>
std::set<T> ArrayToSet(T (&ans)[N])
{
    std::set<T> res;
    for(unsigned i = 0; i < N; i++)
        res.insert(ans[i]);

    EXPECT_EQ(N, res.size());

    return res;
}

template<typename R, typename T, unsigned N>
void CheckRangeEqSet(R dst, T (&ans)[N])
{
    std::set<T> expected = ArrayToSet(ans);
    std::set<typename R::value_type> actual = dst.toStdSet();

    EXPECT_EQ(expected.size(), actual.size());

    typename std::set<T>::iterator it1 = expected.begin();
    typename std::set<typename R::value_type>::iterator it2 = actual.begin();
    for(; it1 != expected.end() && it2 != actual.end(); it1++, it2++)
    {
        EXPECT_EQ(*it2, *it1);
    }
}
