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

#ifndef MIXINHETERFILTER_H_990158796753
#define MIXINHETERFILTER_H_990158796753

#include "../hetercallbacklist.h"
#include "../internal/typeutil_i.h"

#include <functional>
#include <type_traits>

namespace eventpp {

template <typename Base>
class MixinHeterFilter : public Base
{
private:
	using super = Base;

	using BoolReferencePrototypeList = typename internal_::ReplaceReturnTypeList<
		typename internal_::TransformArgumentsList<
			typename super::PrototypeList,
			std::add_lvalue_reference
		>::Type,
		bool
	>::Type;

	using FilterList = HeterCallbackList<BoolReferencePrototypeList>;

public:
	using FilterHandle = typename FilterList::Handle;

public:
	template <typename Callback>
	FilterHandle appendFilter(const Callback & filter)
	{
		return filterList.append(filter);
	}

	bool removeFilter(const FilterHandle & filterHandle)
	{
		return filterList.remove(filterHandle);
	}

	template <typename ...Args>
	bool mixinBeforeDispatch(Args && ...args) const {
		if(! filterList.template forEachIf<void (Args...)>([&args...](const typename std::function<bool (Args...)> & callback) -> bool {
			return callback(std::forward<Args>(args)...);
		})
			) {
			return false;
		}

		return true;
	}

private:
	FilterList filterList;
};


} //namespace eventpp


#endif

