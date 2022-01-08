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

#ifndef MIXINFILTER_H_713231680355
#define MIXINFILTER_H_713231680355

#include "../callbacklist.h"
#include "../internal/typeutil_i.h"

#include <functional>
#include <type_traits>

namespace eventpp {

template <typename Base>
class MixinFilter : public Base
{
private:
	using super = Base;

	using BoolReferencePrototype = typename internal_::ReplaceReturnType<
		typename internal_::TransformArguments<
			typename super::Prototype,
			std::add_lvalue_reference
		>::Type,
		bool
	>::Type;

	using Filter = std::function<BoolReferencePrototype>;
	using FilterList = CallbackList<BoolReferencePrototype>;

public:
	using FilterHandle = typename FilterList::Handle;

public:
	FilterHandle appendFilter(const Filter & filter)
	{
		return filterList.append(filter);
	}

	bool removeFilter(const FilterHandle & filterHandle)
	{
		return filterList.remove(filterHandle);
	}

	template <typename ...Args>
	bool mixinBeforeDispatch(Args && ...args) const {
		if(! filterList.empty()) {
			if(! filterList.forEachIf([&args...](typename FilterList::Callback & callback) {
					return callback(args...);
				})
			) {
				return false;
			}
		}

		return true;
	}

private:
	FilterList filterList;
};


} //namespace eventpp


#endif

