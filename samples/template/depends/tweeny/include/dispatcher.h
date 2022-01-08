/*
 This file is part of the Tweeny library.

 Copyright (c) 2016-2021 Leonardo Guilherme Lucena de Freitas
 Copyright (c) 2016 Guilherme R. Costa

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* This file contains code to help call a function applying a tuple as its arguments.
 * This code is private and not documented. */

#ifndef TWEENY_DISPATCHER_H
#define TWEENY_DISPATCHER_H

#include <tuple>

namespace tweeny {
    namespace detail {
        template<int ...> struct seq { };
        template<int N, int ...S> struct gens : gens<N - 1, N - 1, S...> { };
        template<int ...S> struct gens<0, S...> {
            typedef seq<S...> type;
        };

        template<typename R, typename Func, typename TupleType, int ...S>
        R dispatch(Func && f, TupleType && args, seq<S...>) {
           return f(std::get<S>(args) ...);
        }

        template<typename R, typename Func, typename... Ts>
        R call(Func && f, const std::tuple<Ts...> & args) {
            return dispatch<R>(f, args, typename gens<sizeof...(Ts)>::type());
        }
    }
}

#endif //TWEENY_DISPATCHER_H
