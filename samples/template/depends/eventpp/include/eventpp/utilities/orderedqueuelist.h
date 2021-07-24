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

#ifndef ORDEREDQUEUELIST_H_862875671799
#define ORDEREDQUEUELIST_H_862875671799

#include "../eventpolicies.h"

#include <memory>
#include <list>
#include <algorithm>

namespace eventpp {

struct OrderedQueueListCompare
{
	template <typename T>
	bool operator() (const T & a, const T & b) const {
		return a.event < b.event;
	}
};

template <typename T, typename Compare = OrderedQueueListCompare>
class OrderedQueueList : private std::list<T>
{
private:
	using super = std::list<T>;

public:
	using iterator = typename super::iterator;
	using const_iterator = typename super::const_iterator;
	using super::empty;
	using super::begin;
	using super::end;
	using super::swap;
	using super::emplace_back;

	void splice(const_iterator pos, OrderedQueueList & other) {
		super::splice(pos, other);
		doSort();
	}

	void splice(const_iterator pos, OrderedQueueList & other, const_iterator it) {
		super::splice(pos, other, it);
		doSort();
	}

private:
	void doSort() {
		auto compare = Compare();
		this->sort([compare](const T & a, const T & b) {
			// a and b may be empty if they are recycled to free list.
			if(a.empty()) {
				if(b.empty()) {
					return false;
				}
				return true;
			}
			else if(b.empty()) {
				return false;
			}

			return compare(a.get(), b.get());
		});
	}
};


} //namespace eventpp

#endif

