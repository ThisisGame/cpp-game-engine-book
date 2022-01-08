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

#ifndef TYPEUTIL_I_H
#define TYPEUTIL_I_H

#include <utility>
#include <tuple>

namespace eventpp {

namespace internal_ {

template <typename F, template <typename> class T>
struct TransformArguments;

template <template <typename> class T, typename RT, typename ...Args>
struct TransformArguments <RT (Args...), T>
{
	using Type = RT (typename T<Args>::type...);
};

template <typename F, typename Replacement>
struct ReplaceReturnType;

template <typename Replacement, typename RT, typename ...Args>
struct ReplaceReturnType <RT (Args...), Replacement>
{
	using Type = Replacement (Args...);
};

template <int N, int M>
struct IntToConstantHelper
{
	template <typename C, typename ...Args>
	static auto find(const int index, C && c, Args && ...args)
		-> decltype(std::declval<C>().template operator()<0>(std::declval<Args>()...))
	{
		if(N == index) {
			return c.template operator()<N>(std::forward<Args>(args)...);
		}
		else {
			return IntToConstantHelper<N + 1, M>::find(index, std::forward<C>(c), std::forward<Args>(args)...);
		}
	}
};

template <int M>
struct IntToConstantHelper <M, M>
{
	template <typename C, typename ...Args>
	static auto find(const int /*index*/, C && c, Args && ...args)
		-> decltype(std::declval<C>().template operator()<0>(std::declval<Args>()...))
	{
		return decltype(c.template operator()<0>(std::forward<Args>(args)...))();
	}
};

template <int M, typename C, typename ...Args>
auto intToConstant(const int index, C && c, Args && ...args)
	-> decltype(std::declval<C>().template operator()<0>(std::declval<Args>()...))
{
	return IntToConstantHelper<0, M>::find(index, std::forward<C>(c), std::forward<Args>(args)...);
}

template <typename F, typename ...Args>
struct CanInvoke
{
	template <typename U, typename ...X>
	static auto invoke(int) -> decltype(std::declval<U>()(std::declval<X>()...), std::true_type());

	template <typename U, typename ...X>
	static auto invoke(...) -> std::false_type;

	enum {
		value = !! decltype(invoke<F, Args...>(0))()
	};
};

template <typename T>
struct ShiftTuple;

template <typename A, typename ...Args>
struct ShiftTuple <std::tuple<A, Args...> >
{
	using Type = std::tuple<Args...>;
};

template <>
struct ShiftTuple <std::tuple<> >
{
	using Type = std::tuple<>;
};

// for compile time debug
template<typename T>
void printTypeInCompileTime(T * = 0)
{
	static_assert(std::is_same<T, int>::value && ! std::is_same<T, int>::value, "The error shows the type name.");
}
template<int N>
void printIntInCompileTime()
{
	int n = 0;
	switch(n) {
	case N:
	case N:
		break;
	};
}

} //namespace internal_

} //namespace eventpp

#endif

