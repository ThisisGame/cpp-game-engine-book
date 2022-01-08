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

#ifndef EVENTUTIL_H_802615012766
#define EVENTUTIL_H_802615012766

#include "../eventpolicies.h"

#include <memory>

namespace eventpp {

template <typename DispatcherType>
bool removeListener(
		DispatcherType & dispatcher,
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener
	)
{
	bool found = false;
	dispatcher.forEachIf(
		event,
		[&found, &dispatcher, &event, &listener](
				const typename DispatcherType::Handle & handle,
				const typename DispatcherType::Callback & item
			) -> bool {
			if(item == listener) {
				found = true;
				dispatcher.removeListener(event, handle);
				return false;
			}
			else {
				return true;
			}
		}
	);
	
	return found;
}

template <typename CallbackListType>
bool removeListener(
		CallbackListType & callbackList,
		const typename CallbackListType::Callback & callback
	)
{
	bool found = false;
	callbackList.forEachIf(
		[&found, &callbackList, &callback](
				const typename CallbackListType::Handle & handle,
				const typename CallbackListType::Callback & item
			) -> bool {
			if(item == callback) {
				found = true;
				callbackList.remove(handle);
				return false;
			}
			else {
				return true;
			}
		}
	);
	
	return found;
}

template <typename DispatcherType>
bool hasListener(
	DispatcherType & dispatcher,
	const typename DispatcherType::Event & event,
	const typename DispatcherType::Callback & listener
)
{
	bool found = false;
	dispatcher.forEachIf(
		event,
		[&found, &listener](
			const typename DispatcherType::Handle & /*handle*/,
			const typename DispatcherType::Callback & item
			) -> bool {
			if(item == listener) {
				found = true;
				return false;
			}
			else {
				return true;
			}
		}
	);

	return found;
}

template <typename DispatcherType>
bool hasAnyListener(
	DispatcherType & dispatcher,
	const typename DispatcherType::Event & event
)
{
	bool found = false;
	dispatcher.forEachIf(
		event,
		[&found](
			const typename DispatcherType::Handle & /*handle*/,
			const typename DispatcherType::Callback & /*item*/
			) -> bool {
		found = true;
		return false;
	}
	);

	return found;
}

template <typename CallbackListType>
bool hasListener(
	CallbackListType & callbackList,
	const typename CallbackListType::Callback & callback
)
{
	bool found = false;
	callbackList.forEachIf(
		[&found, &callback](
			const typename CallbackListType::Handle & /*handle*/,
			const typename CallbackListType::Callback & item
			) -> bool {
			if(item == callback) {
				found = true;
				return false;
			}
			else {
				return true;
			}
		}
	);

	return found;
}

template <typename CallbackListType>
bool hasAnyListener(
	CallbackListType & callbackList
)
{
	bool found = false;
	callbackList.forEachIf(
		[&found](
			const typename CallbackListType::Handle & /*handle*/,
			const typename CallbackListType::Callback & /*item*/
			) -> bool {
			found = true;
			return false;
		}
	);

	return found;
}


} //namespace eventpp

#endif

