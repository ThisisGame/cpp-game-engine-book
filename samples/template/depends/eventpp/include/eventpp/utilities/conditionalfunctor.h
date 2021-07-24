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

#ifndef CONDITIONALFUNCTOR_H_828958739581
#define CONDITIONALFUNCTOR_H_828958739581

namespace eventpp {

template <typename Func, typename Condition>
struct ConditionalFunctor
{
	ConditionalFunctor(Func f, Condition condition)
		: func(std::move(f)), condition(std::move(condition))
	{
	}

	template <typename ...A>
	void operator() (A &&...args) {
		if(condition(std::forward<A>(args)...)) {
			func(std::forward<A>(args)...);
		}
	}

	Func func;
	Condition condition;
};

template <typename Func, typename Condition>
ConditionalFunctor<Func, Condition> conditionalFunctor(Func func, Condition condition)
{
	return ConditionalFunctor<Func, Condition>(std::move(func), std::move(condition));
}


} //namespace eventpp

#endif

