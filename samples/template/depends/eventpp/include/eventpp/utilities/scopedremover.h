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

#ifndef SCOPEDREMOVER_H_695291515513
#define SCOPEDREMOVER_H_695291515513

#include "../eventpolicies.h"

#include <memory>
#include <vector>

namespace eventpp {

template <typename DispatcherType, typename Enabled = void>
class ScopedRemover;

template <typename DispatcherType>
class ScopedRemover <
		DispatcherType,
		typename std::enable_if<std::is_base_of<TagEventDispatcher, DispatcherType>::value>::type
	>
{
private:
	struct Item
	{
		typename DispatcherType::Event event;
		typename DispatcherType::Handle handle;
	};
	
public:
	ScopedRemover()
		: dispatcher(nullptr)
	{
	}

	explicit ScopedRemover(DispatcherType & dispatcher)
		: dispatcher(&dispatcher)
	{
	}
	
	~ScopedRemover()
	{
		reset();
	}
	
	void reset()
	{
		if(dispatcher != nullptr) {
			for(const auto & item : itemList) {
				dispatcher->removeListener(item.event, item.handle);
			}
		}
		
		std::unique_lock<std::mutex> lock(itemListMutex);

		itemList.clear();
	}
	
	void setDispatcher(DispatcherType & dispatcher)
	{
		if(this->dispatcher != &dispatcher) {
			this->dispatcher = &dispatcher;
			
			std::unique_lock<std::mutex> lock(itemListMutex);
			itemList.clear();
		}
	}
	
	template <typename Callback>
	typename DispatcherType::Handle appendListener(
			const typename DispatcherType::Event & event,
			const Callback & listener
		)
	{
		Item item {
			event,
			dispatcher->appendListener(event, listener)
		};

		{
			std::unique_lock<std::mutex> lock(itemListMutex);
			itemList.push_back(item);
		}

		return item.handle;
	}

	template <typename Callback>
	typename DispatcherType::Handle prependListener(
			const typename DispatcherType::Event & event,
			const Callback & listener
		)
	{
		Item item {
			event,
			dispatcher->prependListener(event, listener)
		};
		
		{
			std::unique_lock<std::mutex> lock(itemListMutex);
			itemList.push_back(item);
		}
		
		return item.handle;
	}

	template <typename Callback>
	typename DispatcherType::Handle insertListener(
			const typename DispatcherType::Event & event,
			const Callback & listener,
			const typename DispatcherType::Handle & before
		)
	{
		Item item {
			event,
			dispatcher->insertListener(event, listener, before)
		};
		
		{
			std::unique_lock<std::mutex> lock(itemListMutex);
			itemList.push_back(item);
		}
		
		return item.handle;
	}

private:
	DispatcherType * dispatcher;
	std::vector<Item> itemList;
	typename DispatcherType::Mutex itemListMutex;
};

template <typename CallbackListType>
class ScopedRemover <
		CallbackListType,
		typename std::enable_if<std::is_base_of<TagCallbackList, CallbackListType>::value>::type
	>
{
private:
	struct Item
	{
		typename CallbackListType::Handle handle;
	};
	
public:
	ScopedRemover()
		: callbackList(nullptr)
	{
	}

	explicit ScopedRemover(CallbackListType & callbackList)
		: callbackList(&callbackList)
	{
	}
	
	~ScopedRemover()
	{
		reset();
	}
	
	void reset()
	{
		if(callbackList != nullptr) {
			for(const auto & item : itemList) {
				callbackList->remove(item.handle);
			}
		}
		itemList.clear();
	}
	
	void setCallbackList(CallbackListType & callbackList)
	{
		if(this->callbackList != &callbackList) {
			reset();
			this->callbackList = &callbackList;
		}
	}
	
	template <typename Callback>
	typename CallbackListType::Handle append(
			const Callback & callback
		)
	{
		Item item {
			callbackList->append(callback)
		};
		itemList.push_back(item);
		return item.handle;
	}

	template <typename Callback>
	typename CallbackListType::Handle prepend(
			const Callback & callback
		)
	{
		Item item {
			callbackList->prepend(callback)
		};
		itemList.push_back(item);
		return item.handle;
	}

	template <typename Callback>
	typename CallbackListType::Handle insert(
			const Callback & callback,
			const typename CallbackListType::Handle & before
		)
	{
		Item item {
			callbackList->insert(callback, before)
		};
		itemList.push_back(item);
		return item.handle;
	}

private:
	CallbackListType * callbackList;
	std::vector<Item> itemList;
};


} //namespace eventpp

#endif

