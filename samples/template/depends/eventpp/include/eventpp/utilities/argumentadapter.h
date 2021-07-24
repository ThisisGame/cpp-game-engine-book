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

#ifndef ARGUMENTADAPTER_H_566280692673
#define ARGUMENTADAPTER_H_566280692673

#include <memory>

namespace eventpp {

namespace adapter_internal_ {

template <typename T>
struct StaticCast
{
	template <typename U>
	static T cast(U && value)
	{
		return static_cast<T>(value);
	}
};

template <typename T>
struct StaticCast<std::shared_ptr<T> >
{
	template <typename U>
	static std::shared_ptr<T> cast(U && value)
	{
		return std::static_pointer_cast<T>(value);
	}
};

template <typename T>
struct IsSharedPtr
{
	enum { value = false };
};

template <typename T>
struct IsSharedPtr<std::shared_ptr<T> >
{
	enum { value = true };
};

template <typename ...Args>
struct IsAnySharedPtr
{
	enum { value = false };
};

template <typename First, typename ...Others>
struct IsAnySharedPtr <First, Others...>
{
	enum { value = IsSharedPtr<First>::value
		|| IsAnySharedPtr<Others...>::value };
};

} //namespace adapter_internal_

template <typename Func, typename Prototype, typename IsSharedPtr = void>
struct ArgumentAdapter;

template <typename Func, typename R, typename ...Args>
struct ArgumentAdapter <
		Func,
		R(Args...),
		typename std::enable_if<! adapter_internal_::IsAnySharedPtr<Args...>::value>::type
	>
{
	explicit ArgumentAdapter(Func f)
		: func(std::move(f))
	{
	}

	template <typename ...A>
	void operator() (A &&...args) {
		func(std::forward<Args>(static_cast<Args>(args))...);
	}

	Func func;
};

template <typename Func, typename R, typename ...Args>
struct ArgumentAdapter <
		Func,
		R(Args...),
		typename std::enable_if<adapter_internal_::IsAnySharedPtr<Args...>::value>::type
	>
{
	explicit ArgumentAdapter(Func f)
		: func(std::move(f))
	{
	}

	template <typename ...A>
	void operator() (A &&...args) {
		func(std::forward<Args>(adapter_internal_::StaticCast<Args>::cast(args))...);
	}

	Func func;
};

template <template <typename> class Func, typename R, typename ...Args>
ArgumentAdapter<Func<R(Args...)>, R(Args...)> argumentAdapter(Func<R(Args...)> func)
{
	return ArgumentAdapter<Func<R(Args...)>, R(Args...)>(std::move(func));
}

template <typename Prototype, typename Func>
ArgumentAdapter<Func, Prototype> argumentAdapter(Func func)
{
	return ArgumentAdapter<Func, Prototype>(std::move(func));
}

template <typename R, typename ...Args>
ArgumentAdapter<R(*)(Args...), R(Args...)> argumentAdapter(R(*func)(Args...))
{
	return ArgumentAdapter<R(*)(Args...), R(Args...)>(std::move(func));
}


} //namespace eventpp


#endif

