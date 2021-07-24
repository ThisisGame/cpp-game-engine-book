// eventpp library
// Copyright (C) 2018 Wang Qi (wqking)
// Github: https://github.com/wqking/eventpp
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//   http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef HETERCALLBACKLIST_I_H
#define HETERCALLBACKLIST_I_H

#include "typeutil_i.h"

#include <tuple>

namespace eventpp {

/*
// We can't use std::tuple as HeterTuple because std::tuple is instantialized
// somewhere in the Map in HeterEventDispatcherBase, that causes compile error
// for that function type can't be declared.
template <typename ...Types>
using HeterTuple = std::tuple<Types...>;

template <typename T>
struct HeterTupleSize;

template <typename ...Types>
struct HeterTupleSize <HeterTuple <Types...> >
{
enum { value = std::tuple_size<HeterTuple <Types...> >::value };
};
*/

template <typename ...Types>
struct HeterTuple
{
};

template <typename T>
struct HeterTupleSize;

template <typename ...Types>
struct HeterTupleSize <HeterTuple <Types...> >
{
	enum { value = sizeof...(Types) };
};

namespace internal_ {

template <typename T>
struct FindPrototypeDefaultArgTransformer
{
	using type = T;
};

template <
	int N,
	typename PrototypeList_,
	typename Callable,
	template <typename> class ArgTransformer = FindPrototypeDefaultArgTransformer,
	int M = HeterTupleSize<PrototypeList_>::value,
	typename Enabled = void
>
struct FindPrototypeByCallableFromIndex;

template <
	int N,
	typename RT,
	typename ...Args,
	typename ...Others,
	typename Callable,
	template <typename> class ArgTransformer,
	int M
>
struct FindPrototypeByCallableFromIndex <
	N,
	HeterTuple<RT (Args...), Others...>,
	Callable,
	ArgTransformer,
	M,
	typename std::enable_if<(N < M)>::type
>
{
	enum { canInvoke = CanInvoke<Callable, typename ArgTransformer<Args>::type...>::value };

	using Prototype = typename std::conditional<
		canInvoke,
		RT (Args...),
		typename FindPrototypeByCallableFromIndex<N + 1, HeterTuple<Others...>, Callable, ArgTransformer, M>::Prototype
	>::type;

	using ArgsTuple = typename std::conditional<
		canInvoke,
		std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...>,
		typename FindPrototypeByCallableFromIndex<N + 1, HeterTuple<Others...>, Callable, ArgTransformer, M>::ArgsTuple
	>::type;

	enum {
		index = canInvoke
			? N
			: FindPrototypeByCallableFromIndex<N + 1, HeterTuple<Others...>, Callable, ArgTransformer, M>::index
	};
};

template <
	int N,
	typename PrototypeList_,
	typename Callable,
	template <typename> class ArgTransformer,
	int M
>
struct FindPrototypeByCallableFromIndex <
	N,
	PrototypeList_,
	Callable,
	ArgTransformer,
	M,
	typename std::enable_if<(N >= M)>::type
>
{
	using Prototype = void;

	using ArgsTuple = std::tuple<>;

	enum {
		index = -1
	};
};

template <typename PrototypeList_, typename Callable, template <typename> class ArgTransformer = FindPrototypeDefaultArgTransformer>
struct FindPrototypeByCallable : public FindPrototypeByCallableFromIndex <0, PrototypeList_, Callable, ArgTransformer>
{
};

template <int N, typename PrototypeList_, typename ...InArgs>
struct FindPrototypeByArgsFromIndex;

template <int N, typename RT, typename ...Args, typename ...Others, typename ...InArgs>
struct FindPrototypeByArgsFromIndex <N, HeterTuple<RT (Args...), Others...>, InArgs...>
{
	enum { canInvoke = CanInvoke<RT (Args...), InArgs...>::value };

	using Prototype = typename std::conditional<
		canInvoke,
		RT (Args...),
		typename FindPrototypeByArgsFromIndex<N + 1, HeterTuple<Others...>, InArgs...>::Prototype
	>::type;

	using ArgsTuple = typename std::conditional<
		canInvoke,
		std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...>,
		typename FindPrototypeByArgsFromIndex<N + 1, HeterTuple<Others...>, InArgs...>::ArgsTuple
	>::type;

	enum {
		index = canInvoke
		? N
		: FindPrototypeByArgsFromIndex<N + 1, HeterTuple<Others...>, InArgs...>::index
	};
};

template <int N, typename ...InArgs>
struct FindPrototypeByArgsFromIndex <N, HeterTuple<>, InArgs...>
{
	using Prototype = void;

	using ArgsTuple = std::tuple<>;

	enum { index = -1 };
};

template <typename PrototypeList_, typename ...InArgs>
struct FindPrototypeByArgs : public FindPrototypeByArgsFromIndex <0, PrototypeList_, InArgs...>
{
};

template <int I, int N, typename PrototypeList_>
struct FindPrototypeByIndexHelper;

template <int I, int N, typename RT, typename ...Args, typename ...Others>
struct FindPrototypeByIndexHelper <I, N, HeterTuple<RT (Args...), Others...> >
{
	using NextType = FindPrototypeByIndexHelper<I + 1, N, HeterTuple<Others...> >;
	using Prototype = typename NextType::Prototype;

	using ArgsTuple = typename NextType::ArgsTuple;

	enum { index = NextType::index };
};

template <int N, typename RT, typename ...Args, typename ...Others>
struct FindPrototypeByIndexHelper <N, N, HeterTuple<RT (Args...), Others...> >
{
	using Prototype = RT (Args...);

	using ArgsTuple = HeterTuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...>;

	enum { index = N };
};

template <int I, int N>
struct FindPrototypeByIndexHelper <I, N, HeterTuple<>>
{
	using Prototype = void;

	using ArgsTuple = HeterTuple<>;

	enum { index = -1 };
};

template <typename PrototypeList_, int N>
struct FindPrototypeByIndex : public FindPrototypeByIndexHelper <0, N, PrototypeList_>
{
};

template <typename PrototypeList_, template <typename ...> class Record>
struct GetCallablePrototypeMaxSize;

template <typename RT, typename ...Args, typename ...Others, template <typename ...> class Record>
struct GetCallablePrototypeMaxSize <HeterTuple<RT (Args...), Others...>, Record>
{
	enum {
		my = sizeof(Record<Args...>),
		other = GetCallablePrototypeMaxSize<HeterTuple<Others...>, Record>::value
	};
	enum { value = (my > other ? (int)my : (int)other) };
};

template <template <typename ...> class Record>
struct GetCallablePrototypeMaxSize <HeterTuple<>, Record>
{
	enum { value = 1 }; // set minimum size to 1 instead of 0
};

template <typename List, typename Item>
struct PrependHeterTuple;

template <typename Item, typename ...Types>
struct PrependHeterTuple <HeterTuple<Types...>, Item>
{
	using Type = HeterTuple<Item, Types...>;
};

template <typename List, typename Replacement>
struct ReplaceReturnTypeList;

template <typename Replacement, typename RT, typename ...Args, typename ...Others>
struct ReplaceReturnTypeList <HeterTuple<RT (Args...), Others...>, Replacement>
{
	using Type = typename PrependHeterTuple<
		typename ReplaceReturnTypeList<HeterTuple<Others...>, Replacement>::Type,
		Replacement (Args...)
	>::Type;
};

template <typename Replacement>
struct ReplaceReturnTypeList <HeterTuple<>, Replacement>
{
	using Type = HeterTuple<>;
};

template <typename F, template <typename> class T>
struct TransformArgumentsList;

template <template <typename> class T, typename RT, typename ...Args, typename ...Others>
struct TransformArgumentsList <HeterTuple<RT (Args...), Others...>, T>
{
	using Type = typename PrependHeterTuple<
		typename TransformArgumentsList<HeterTuple<Others...>, T>::Type,
		RT (typename T<Args>::type...)
	>::Type;
};

template <template <typename> class T>
struct TransformArgumentsList <HeterTuple<>, T>
{
	using Type = HeterTuple<>;
};


} //namespace internal_


} //namespace eventpp

#endif
