#pragma once

#include <limits.h>

#include <functional>
#include <tuple>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>

//

namespace boolinq {

    struct LinqEndException {};

    enum BytesDirection {
        BytesFirstToLast,
        BytesLastToFirst,
    };

    enum BitsDirection {
        BitsHighToLow,
        BitsLowToHigh,
    };

    template<typename S, typename T>
    class Linq {
        std::function<T(S &)> nextFunc;
        S storage;

    public:
        typedef T value_type;

        Linq() : nextFunc(), storage()
        {
        }

        Linq(S storage, std::function<T(S &)> nextFunc) : nextFunc(nextFunc), storage(storage)
        {
        }

        T next()
        {
            return nextFunc(storage);
        }

        void for_each_i(std::function<void(T, int)> apply) const
        {
            Linq<S, T> linq = *this;
            try {
                for (int i = 0; ; i++) {
                    apply(linq.next(), i);
                }
            }
            catch (LinqEndException &) {}
        }

        void for_each(std::function<void(T)> apply) const
        {
            return for_each_i([apply](T value, int) { return apply(value); });
        }

        Linq<std::tuple<Linq<S, T>, int>, T> where_i(std::function<bool(T, int)> filter) const
        {
            return Linq<std::tuple<Linq<S, T>, int>, T>(
                std::make_tuple(*this, 0),
                [filter](std::tuple<Linq<S, T>, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    int &index = std::get<1>(tuple);

                    while (true) {
                        T ret = linq.next();
                        if (filter(ret, index++)) {
                            return ret;
                        }
                    }
                }
            );
        }

        Linq<std::tuple<Linq<S, T>, int>, T> where(std::function<bool(T)> filter) const
        {
            return where_i([filter](T value, int) { return filter(value); });
        }

        Linq<std::tuple<Linq<S, T>, int>, T> take(int count) const
        {
            return where_i([count](T /*value*/, int i) {
                if (i == count) {
                    throw LinqEndException();
                }
                return true;
            });
        }

        Linq<std::tuple<Linq<S, T>, int>, T> takeWhile_i(std::function<bool(T, int)> predicate) const
        {
            return where_i([predicate](T value, int i) {
                if (!predicate(value, i)) {
                    throw LinqEndException();
                }
                return true;
            });
        }

        Linq<std::tuple<Linq<S, T>, int>, T> takeWhile(std::function<bool(T)> predicate) const
        {
            return takeWhile_i([predicate](T value, int /*i*/) { return predicate(value); });
        }

        Linq<std::tuple<Linq<S, T>, int>, T> skip(int count) const
        {
            return where_i([count](T /*value*/, int i) { return i >= count; });
        }

        Linq<std::tuple<Linq<S, T>, int, bool>, T> skipWhile_i(std::function<bool(T, int)> predicate) const
        {
            return Linq<std::tuple<Linq<S, T>, int, bool>, T>(
                std::make_tuple(*this, 0, false),
                [predicate](std::tuple<Linq<S, T>, int, bool> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    int &index = std::get<1>(tuple);
                    bool &flag = std::get<2>(tuple);

                    if (flag) {
                        return linq.next();
                    }
                    while (true) {
                        T ret = linq.next();
                        if (!predicate(ret, index++)) {
                            flag = true;
                            return ret;
                        }
                    }
                }
            );
        }

        Linq<std::tuple<Linq<S, T>, int, bool>, T> skipWhile(std::function<bool(T)> predicate) const
        {
            return skipWhile_i([predicate](T value, int /*i*/) { return predicate(value); });
        }

        template<typename ... Types>
        Linq<std::tuple<Linq<S, T>, std::vector<T>, int>, T> append(Types ... newValues) const
        {
            return Linq<std::tuple<Linq<S, T>, std::vector<T>, int>, T>(
                std::make_tuple(*this, std::vector<T>{ newValues... }, -1),
                [](std::tuple<Linq<S, T>, std::vector<T>, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    std::vector<T> &values = std::get<1>(tuple);
                    int &index = std::get<2>(tuple);

                    if (index == -1) {
                        try {
                            return linq.next();
                        }
                        catch (LinqEndException &) {
                            index = 0;
                        }
                    }

                    if (index < values.size()) {
                        return values[index++];
                    }

                    throw LinqEndException();
                }
            );
        }

        template<typename ... Types>
        Linq<std::tuple<Linq<S, T>, std::vector<T>, int>, T> prepend(Types ... newValues) const
        {
            return Linq<std::tuple<Linq<S, T>, std::vector<T>, int>, T>(
                std::make_tuple(*this, std::vector<T>{ newValues... }, 0),
                [](std::tuple<Linq<S, T>, std::vector<T>, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    std::vector<T> &values = std::get<1>(tuple);
                    int &index = std::get<2>(tuple);

                    if (index < values.size()) {
                        return values[index++];
                    }
                    return linq.next();
                }
            );
        }

        template<typename F, typename _TRet = typename std::result_of<F(T, int)>::type>
        Linq<std::tuple<Linq<S, T>, int>, _TRet> select_i(F apply) const
        {
            return Linq<std::tuple<Linq<S, T>, int>, _TRet>(
                std::make_tuple(*this, 0),
                [apply](std::tuple<Linq<S, T>, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    int &index = std::get<1>(tuple);

                    return apply(linq.next(), index++);
                }
            );
        }

        template<typename F, typename _TRet = typename std::result_of<F(T)>::type>
        Linq<std::tuple<Linq<S, T>, int>, _TRet> select(F apply) const
        {
            return select_i([apply](T value, int /*index*/) { return apply(value); });
        }

        template<typename TRet>
        Linq<std::tuple<Linq<S, T>, int>, TRet> cast() const
        {
            return select_i([](T value, int /*i*/) { return TRet(value); });
        }

        template<typename S2, typename T2>
        Linq<std::tuple<Linq<S, T>, Linq<S2, T2>, bool>, T> concat(const Linq<S2, T2> & rhs) const
        {
            return Linq<std::tuple<Linq<S, T>, Linq<S2, T2>, bool>, T>(
                std::make_tuple(*this, rhs, false),
                [](std::tuple<Linq<S, T>, Linq<S2, T2>, bool> &tuple){
                    Linq<S, T> &first = std::get<0>(tuple);
                    Linq<S2, T2> &second = std::get<1>(tuple);
                    bool &flag = std::get<2>(tuple);

                    if (!flag) {
                        try {
                            return first.next();
                        }
                        catch (LinqEndException &) {}
                    }
                    return second.next();
                }
            );
        }

        template<
            typename F,
            typename _TRet = typename std::result_of<F(T, int)>::type,
            typename _TRetVal = typename _TRet::value_type
        >
        Linq<std::tuple<Linq<S, T>, _TRet, int, bool>, _TRetVal> selectMany_i(F apply) const
        {
            return Linq<std::tuple<Linq<S, T>, _TRet, int, bool>, _TRetVal>(
                std::make_tuple(*this, _TRet(), 0, true),
                [apply](std::tuple<Linq<S, T>, _TRet, int, bool> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    _TRet &current = std::get<1>(tuple);
                    int &index = std::get<2>(tuple);
                    bool &finished = std::get<3>(tuple);

                    while (true) {
                        if (finished) {
                            current = apply(linq.next(), index++);
                            finished = false;
                        }
                        try {
                            return current.next();
                        }
                        catch (LinqEndException &) {
                            finished = true;
                        }
                    }
                }
            );
        }

        template<
            typename F,
            typename _TRet = typename std::result_of<F(T)>::type,
            typename _TRetVal = typename _TRet::value_type
        >
        Linq<std::tuple<Linq<S, T>, _TRet, int, bool>, _TRetVal> selectMany(F apply) const
        {
            return selectMany_i([apply](T value, int /*index*/) { return apply(value); });
        }

        template<
            typename F,
            typename _TKey = typename std::result_of<F(T)>::type,
            typename _TValue = Linq<std::tuple<Linq<S, T>, int>, T> // where(predicate)
        >
        Linq<std::tuple<Linq<S, T>, Linq<S, T>, std::unordered_set<_TKey> >, std::pair<_TKey, _TValue> > groupBy(F apply) const
        {
            return Linq<std::tuple<Linq<S, T>, Linq<S, T>, std::unordered_set<_TKey> >, std::pair<_TKey, _TValue> >(
                std::make_tuple(*this, *this, std::unordered_set<_TKey>()),
                [apply](std::tuple<Linq<S, T>, Linq<S, T>, std::unordered_set<_TKey> > &tuple){
                    Linq<S, T> &linq = std::get<0>(tuple);
                    Linq<S, T> &linqCopy = std::get<1>(tuple);
                    std::unordered_set<_TKey> &set = std::get<2>(tuple);

                    while (true) {
                        _TKey key = apply(linq.next());
                        if (set.insert(key).second) {
                            return std::make_pair(key, linqCopy.where([apply, key](T v){
                                return apply(v) == key;
                            }));
                        }
                    }
                }
            );
        }

        template<typename F, typename _TRet = typename std::result_of<F(T)>::type>
        Linq<std::tuple<Linq<S, T>, std::unordered_set<_TRet> >, T> distinct(F transform) const
        {
            return Linq<std::tuple<Linq<S, T>, std::unordered_set<_TRet> >, T>(
                std::make_tuple(*this, std::unordered_set<_TRet>()),
                [transform](std::tuple<Linq<S, T>, std::unordered_set<_TRet> > &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    std::unordered_set<_TRet> &set = std::get<1>(tuple);

                    while (true) {
                        T value = linq.next();
                        if (set.insert(transform(value)).second) {
                            return value;
                        }
                    }
                }
            );
        }

        Linq<std::tuple<Linq<S, T>, std::unordered_set<T> >, T> distinct() const
        {
            return distinct([](T value) { return value; });
        }

        template<typename F, typename _TIter = typename std::vector<T>::const_iterator>
        Linq<std::tuple<std::vector<T>, _TIter, bool>, T> orderBy(F transform) const
        {
            std::vector<T> items = toStdVector();
            std::sort(items.begin(), items.end(), [transform](const T &a, const T &b) {
                return transform(a) < transform(b);
            });

            return Linq<std::tuple<std::vector<T>, _TIter, bool>, T>(
                std::make_tuple(items, _TIter(), false),
                [](std::tuple<std::vector<T>, _TIter, bool> &tuple) {
                    std::vector<T> &vec = std::get<0>(tuple);
                    _TIter &it = std::get<1>(tuple);
                    bool &flag = std::get<2>(tuple);

                    if (!flag) {
                        flag = true;
                        it = vec.cbegin();
                    }
                    if (it == vec.cend()) {
                        throw LinqEndException();
                    }
                    return *(it++);
                }
            );
        }

        Linq<std::tuple<std::vector<T>, typename std::vector<T>::const_iterator, bool>, T> orderBy() const
        {
            return orderBy([](T value) { return value; });
        }

        template<typename _TIter = typename std::list<T>::const_reverse_iterator>
        Linq<std::tuple<std::list<T>, _TIter, bool>, T> reverse() const
        {
            return Linq<std::tuple<std::list<T>, _TIter, bool>, T>(
                std::make_tuple(toStdList(), _TIter(), false),
                [](std::tuple<std::list<T>, _TIter, bool> &tuple) {
                    std::list<T> &list = std::get<0>(tuple);
                    _TIter &it = std::get<1>(tuple);
                    bool &flag = std::get<2>(tuple);

                    if (!flag) {
                        flag = true;
                        it = list.crbegin();
                    }
                    if (it == list.crend()) {
                        throw LinqEndException();
                    }
                    return *(it++);
                }
            );
        }

        // Aggregators

        template<typename TRet>
        TRet aggregate(TRet start, std::function<TRet(TRet, T)> accumulate) const
        {
            Linq<S, T> linq = *this;
            try {
                while (true) {
                    start = accumulate(start, linq.next());
                }
            }
            catch (LinqEndException &) {}
            return start;
        }

        template<typename F, typename _TRet = typename std::result_of<F(T)>::type>
        _TRet sum(F transform) const
        {
            return aggregate<_TRet>(_TRet(), [transform](_TRet accumulator, T value) {
                return accumulator + transform(value);
            });
        }

        template<typename TRet = T>
        TRet sum() const
        {
            return sum([](T value) { return TRet(value); });
        }

        template<typename F, typename _TRet = typename std::result_of<F(T)>::type>
        _TRet avg(F transform) const
        {
            int count = 0;
            _TRet res = sum([transform, &count](T value) {
                count++;
                return transform(value);
            });
            return res / count;
        }

        template<typename TRet = T>
        TRet avg() const
        {
            return avg([](T value) { return TRet(value); });
        }

        int count() const
        {
            int index = 0;
            for_each([&index](T /*a*/) { index++; });
            return index;
        }

        int count(std::function<bool(T)> predicate) const
        {
            return where(predicate).count();
        }

        int count(const T &item) const
        {
            return count([item](T value) { return item == value; });
        }

        // Bool aggregators

        bool any(std::function<bool(T)> predicate) const
        {
            Linq<S, T> linq = *this;
            try {
                while (true) {
                    if (predicate(linq.next())) {
                        return true;
                    }
                }
            }
            catch (LinqEndException &) {}
            return false;
        }

        bool any() const
        {
            return any([](T value) { return static_cast<bool>(value); });
        }

        bool all(std::function<bool(T)> predicate) const
        {
            return !any([predicate](T value) { return !predicate(value); });
        }

        bool all() const
        {
            return all([](T value) { return static_cast<bool>(value); });
        }

        bool contains(const T &item) const
        {
            return any([&item](T value) { return value == item; });
        }

        // Election aggregators

        T elect(std::function<T(T, T)> accumulate) const
        {
            T result;
            for_each_i([accumulate, &result](T value, int i) {
                if (i == 0) {
                    result = value;
                } else {
                    result = accumulate(result, value);
                }
            });
            return result;
        }

        template<typename F>
        T max(F transform) const
        {
            return elect([transform](const T &a, const T &b) {
                return (transform(a) < transform(b)) ? b : a;
            });
        }

        T max() const
        {
            return max([](T value) { return value; });
        }

        template<typename F>
        T min(F transform) const
        {
            return elect([transform](const T &a, const T &b) {
                return (transform(a) < transform(b)) ? a : b;
            });
        }

        T min() const
        {
            return min([](T value) { return value; });
        }

        // Single object returners

        T elementAt(int index) const
        {
            return skip(index).next();
        }

        T first(std::function<bool(T)> predicate) const
        {
            return where(predicate).next();
        }

        T first() const
        {
            return Linq<S, T>(*this).next();
        }

        T firstOrDefault(std::function<bool(T)> predicate, T const& defaultValue = T()) const
        {
            try {
                return where(predicate).next();
            }
            catch (LinqEndException &) {}
            return defaultValue;
        }

        T firstOrDefault(T const& defaultValue = T()) const
        {
            try {
                return Linq<S, T>(*this).next();
            }
            catch (LinqEndException &) {}
            return defaultValue;
        }

        T last(std::function<bool(T)> predicate) const
        {
            T res;
            int index = -1;
            where(predicate).for_each_i([&res, &index](T value, int i) {
                res = value;
                index = i;
            });

            if (index == -1) {
                throw LinqEndException();
            }
            return res;
        }

        T last() const
        {
            return last([](T /*value*/) { return true; });
        }

        T lastOrDefault(std::function<bool(T)> predicate, T const& defaultValue = T()) const
        {
            T res = defaultValue;
            where(predicate).for_each([&res](T value) {
                res = value;
            });
            return res;
        }

        T lastOrDefault(T const& defaultValue = T()) const
        {
            return lastOrDefault([](T  /*value*/) { return true; }, defaultValue);
        }

        // Export to containers

        std::vector<T> toStdVector() const
        {
            std::vector<T> items;
            for_each([&items](T value) {
                items.push_back(value);
            });
            return items;
        }

        std::list<T> toStdList() const
        {
            std::list<T> items;
            for_each([&items](T value) {
                items.push_back(value);
            });
            return items;
        }

        std::deque<T> toStdDeque() const
        {
            std::deque<T> items;
            for_each([&items](T value) {
                items.push_back(value);
            });
            return items;
        }

        std::set<T> toStdSet() const
        {
            std::set<T> items;
            for_each([&items](T value) {
                items.insert(value);
            });
            return items;
        }

        std::unordered_set<T> toStdUnorderedSet() const
        {
            std::unordered_set<T> items;
            for_each([&items](T value) {
                items.insert(value);
            });
            return items;
        }

        // Bits and bytes

        Linq<std::tuple<Linq<S, T>, BytesDirection, T, int>, int> bytes(BytesDirection direction = BytesFirstToLast) const
        {
            return Linq<std::tuple<Linq<S, T>, BytesDirection, T, int>, int>(
                std::make_tuple(*this, direction, T(), sizeof(T)),
                [](std::tuple<Linq<S, T>, BytesDirection, T, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    BytesDirection &bytesDirection = std::get<1>(tuple);
                    T &value = std::get<2>(tuple);
                    int &index = std::get<3>(tuple);

                    if (index == sizeof(T)) {
                        value = linq.next();
                        index = 0;
                    }

                    unsigned char *ptr = reinterpret_cast<unsigned char *>(&value);

                    int byteIndex = index;
                    if (bytesDirection == BytesLastToFirst) {
                        byteIndex = sizeof(T) - 1 - byteIndex;
                    }

                    index++;
                    return ptr[byteIndex];
                }
            );
        }

        template<typename TRet>
        Linq<std::tuple<Linq<S, T>, BytesDirection, int>, TRet> unbytes(BytesDirection direction = BytesFirstToLast) const
        {
            return Linq<std::tuple<Linq<S, T>, BytesDirection, int>, TRet>(
                std::make_tuple(*this, direction, 0),
                [](std::tuple<Linq<S, T>, BytesDirection, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    BytesDirection &bytesDirection = std::get<1>(tuple);
                    // int &index = std::get<2>(tuple);

                    TRet value;
                    unsigned char *ptr = reinterpret_cast<unsigned char *>(&value);

                    for (int i = 0; i < sizeof(TRet); i++) {
                        int byteIndex = i;
                        if (bytesDirection == BytesLastToFirst) {
                            byteIndex = sizeof(TRet) - 1 - byteIndex;
                        }

                        ptr[byteIndex] = linq.next();
                    }

                    return value;
                }
            );
        }

        Linq<std::tuple<Linq<S, T>, BytesDirection, BitsDirection, T, int>, int> bits(BitsDirection bitsDir = BitsHighToLow, BytesDirection bytesDir = BytesFirstToLast) const
        {
            return Linq<std::tuple<Linq<S, T>, BytesDirection, BitsDirection, T, int>, int>(
                std::make_tuple(*this, bytesDir, bitsDir, T(), sizeof(T) * CHAR_BIT),
                [](std::tuple<Linq<S, T>, BytesDirection, BitsDirection, T, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    BytesDirection &bytesDirection = std::get<1>(tuple);
                    BitsDirection &bitsDirection = std::get<2>(tuple);
                    T &value = std::get<3>(tuple);
                    int &index = std::get<4>(tuple);

                    if (index == sizeof(T) * CHAR_BIT) {
                        value = linq.next();
                        index = 0;
                    }

                    unsigned char *ptr = reinterpret_cast<unsigned char *>(&value);

                    int byteIndex = index / CHAR_BIT;
                    if (bytesDirection == BytesLastToFirst) {
                        byteIndex = sizeof(T) - 1 - byteIndex;
                    }

                    int bitIndex = index % CHAR_BIT;
                    if (bitsDirection == BitsHighToLow) {
                        bitIndex = CHAR_BIT - 1 - bitIndex;
                    }

                    index++;
                    return (ptr[byteIndex] >> bitIndex) & 1;
                }
            );
        }

        template<typename TRet = unsigned char>
        Linq<std::tuple<Linq<S, T>, BytesDirection, BitsDirection, int>, TRet> unbits(BitsDirection bitsDir = BitsHighToLow, BytesDirection bytesDir = BytesFirstToLast) const
        {
            return Linq<std::tuple<Linq<S, T>, BytesDirection, BitsDirection, int>, TRet>(
                std::make_tuple(*this, bytesDir, bitsDir, 0),
                [](std::tuple<Linq<S, T>, BytesDirection, BitsDirection, int> &tuple) {
                    Linq<S, T> &linq = std::get<0>(tuple);
                    BytesDirection &bytesDirection = std::get<1>(tuple);
                    BitsDirection &bitsDirection = std::get<2>(tuple);
                    // int &index = std::get<3>(tuple);

                    TRet value = TRet();
                    unsigned char *ptr = reinterpret_cast<unsigned char *>(&value);

                    for (int i = 0; i < sizeof(TRet) * CHAR_BIT; i++) {
                        int byteIndex = i / CHAR_BIT;
                        if (bytesDirection == BytesLastToFirst) {
                            byteIndex = sizeof(TRet) - 1 - byteIndex;
                        }

                        int bitIndex = i % CHAR_BIT;
                        if (bitsDirection == BitsHighToLow) {
                            bitIndex = CHAR_BIT - 1 - bitIndex;
                        }

                        ptr[byteIndex] &= ~(1 << bitIndex);
                        ptr[byteIndex] |= bool(linq.next()) << bitIndex;
                    }

                    return value;
                }
            );
        }
    };

    template<typename S, typename T>
    std::ostream &operator<<(std::ostream &stream, Linq<S, T> linq)
    {
        try {
            while (true) {
                stream << linq.next() << ' ';
            }
        }
        catch (LinqEndException &) {}
        return stream;
    }

    ////////////////////////////////////////////////////////////////
    // Linq Creators
    ////////////////////////////////////////////////////////////////

    template<typename T>
    Linq<std::pair<T, T>, typename std::iterator_traits<T>::value_type> from(const T & begin, const T & end)
    {
        return Linq<std::pair<T, T>, typename std::iterator_traits<T>::value_type>(
            std::make_pair(begin, end),
            [](std::pair<T, T> &pair) {
                if (pair.first == pair.second) {
                    throw LinqEndException();
                }
                return *(pair.first++);
            }
        );
    }

    template<typename T>
    Linq<std::pair<T, T>, typename std::iterator_traits<T>::value_type> from(const T & it, int n)
    {
        return from(it, it + n);
    }

    template<typename T, int N>
    Linq<std::pair<const T *, const T *>, T> from(T (&array)[N])
    {
        return from((const T *)(&array), (const T *)(&array) + N);
    }

    template<template<class> class TV, typename TT>
    auto from(const TV<TT> & container)
        -> decltype(from(container.cbegin(), container.cend()))
    {
        return from(container.cbegin(), container.cend());
    }

    // std::list, std::vector, std::dequeue
    template<template<class, class> class TV, typename TT, typename TU>
    auto from(const TV<TT, TU> & container)
        -> decltype(from(container.cbegin(), container.cend()))
    {
        return from(container.cbegin(), container.cend());
    }

    // std::set
    template<template<class, class, class> class TV, typename TT, typename TS, typename TU>
    auto from(const TV<TT, TS, TU> & container)
        -> decltype(from(container.cbegin(), container.cend()))
    {
        return from(container.cbegin(), container.cend());
    }

    // std::map
    template<template<class, class, class, class> class TV, typename TK, typename TT, typename TS, typename TU>
    auto from(const TV<TK, TT, TS, TU> & container)
        -> decltype(from(container.cbegin(), container.cend()))
    {
        return from(container.cbegin(), container.cend());
    }

    // std::array
    template<template<class, size_t> class TV, typename TT, size_t TL>
    auto from(const TV<TT, TL> & container)
        -> decltype(from(container.cbegin(), container.cend()))
    {
        return from(container.cbegin(), container.cend());
    }

    template<typename T>
    Linq<std::pair<T, int>, T> repeat(const T & value, int count) {
        return Linq<std::pair<T, int>, T>(
            std::make_pair(value, count),
            [](std::pair<T, int> &pair) {
                if (pair.second > 0) {
                    pair.second--;
                    return pair.first;
                }
                throw LinqEndException();
            }
        );
    }

    template<typename T>
    Linq<std::tuple<T, T, T>, T> range(const T & start, const T & end, const T & step) {
        return Linq<std::tuple<T, T, T>, T>(
            std::make_tuple(start, end, step),
            [](std::tuple<T, T, T> &tuple) {
                T &start = std::get<0>(tuple);
                T &end = std::get<1>(tuple);
                T &step = std::get<2>(tuple);

                T value = start;
                if (value < end) {
                    start += step;
                    return value;
                }
                throw LinqEndException();
            }
        );
    }
}