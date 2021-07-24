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

#ifndef TEST_H
#define TEST_H

#include "../catch.hpp"

template <typename Callable, typename ReturnType = void>
struct EraseArgs1
{
	template <typename C>
	explicit EraseArgs1(const C & callable) : callable(callable)
	{
	}

	template <typename First, typename ...Args>
	ReturnType operator() (First &&, Args && ...args)
	{
		callable(std::forward(args)...);
	}

	Callable callable;
};

template <typename Callable>
EraseArgs1<Callable> eraseArgs1(const Callable & callable)
{
	return EraseArgs1<Callable>(callable);
}

template <typename T>
bool checkAllWeakPtrAreFreed(const T & nodeList)
{
	for(const auto & node : nodeList) {
		if(node.lock()) {
			return false;
		}
	}

	return true;
}

// Can be converted from int implicitly
struct FromInt
{
	FromInt() : value(0) {}
	FromInt(const int value) : value(value) {}

	int value;
};

// Can convert to int implicitly
struct ToInt
{
	ToInt() : value(0) {}
	explicit ToInt(const int value) : value(value) {}

	operator int() const { return value; }

	int value;
};

// return 0 -- no order, 1 -- ascend -1 -- descend
template <typename Iterator>
int detectDataListOrder(Iterator from, Iterator to)
{
	if(from == to) {
		return 0;
	}
	auto previous = *from;
	++from;
	bool ascend = false;
	bool descend = false;
	while(from != to) {
		auto current = *from;
		++from;
		if(current > previous) {
			ascend = true;
		}
		if(current < previous) {
			descend = true;
		}
		if(ascend && descend) {
			return 0;
		}
		previous = current;
	}
	if(ascend) {
		return 1;
	}
	if(descend) {
		return -1;
	}
	return 0;
}

#endif
