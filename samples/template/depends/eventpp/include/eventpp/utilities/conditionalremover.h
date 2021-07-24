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

#ifndef CONDITIONALREMOVER_H_882115092280
#define CONDITIONALREMOVER_H_882115092280

#include "../eventpolicies.h"

#include <memory>

namespace eventpp {

template <typename DispatcherType, typename Enabled = void>
class ConditionalRemover;

template <typename DispatcherType>
class ConditionalRemover <
		DispatcherType,
		typename std::enable_if<std::is_base_of<TagEventDispatcher, DispatcherType>::value>::type
	>
{
private:
	template <typename Callback, typename Condition>
	struct ItemByCondition
	{
		struct Data
		{
			Condition shouldRemove;
			DispatcherType & dispatcher;
			typename DispatcherType::Event event;
			Callback listener;
			typename DispatcherType::Handle handle;
		};

		template <typename ...Args>
		auto operator() (Args && ...args) const
			-> typename std::enable_if<internal_::CanInvoke<Condition, Args...>::value>::type {
			if(data->shouldRemove(std::forward<Args>(args)...)) {
				data->dispatcher.removeListener(data->event, data->handle);
			}
			data->listener(std::forward<Args>(args)...);
		}

		template <typename ...Args>
		auto operator() (Args && ...args) const
			-> typename std::enable_if<! internal_::CanInvoke<Condition, Args...>::value>::type {
			if(data->shouldRemove()) {
				data->dispatcher.removeListener(data->event, data->handle);
			}
			data->listener(std::forward<Args>(args)...);
		}

		std::shared_ptr<Data> data;
	};

public:
	explicit ConditionalRemover(DispatcherType & dispatcher)
		: dispatcher(dispatcher)
	{
	}
	
	template <typename Callback, typename Condition>
	typename DispatcherType::Handle appendListener(
			const typename DispatcherType::Event & event,
			const Callback & listener,
			const Condition & condition
		)
	{
		using Wrapper = ItemByCondition<Callback, Condition>;
		auto data = std::make_shared<typename Wrapper::Data>(typename Wrapper::Data {
			condition, dispatcher, event, listener, typename DispatcherType::Handle()
		});
		data->handle = dispatcher.appendListener(event, Wrapper{data});
		return data->handle;
	}

	template <typename Callback, typename Condition>
	typename DispatcherType::Handle prependListener(
			const typename DispatcherType::Event & event,
			const Callback & listener,
			const Condition & condition
		)
	{
		using Wrapper = ItemByCondition<Callback, Condition>;
		auto data = std::make_shared<typename Wrapper::Data>(typename Wrapper::Data {
			condition, dispatcher, event, listener, typename DispatcherType::Handle()
		});
		data->handle = dispatcher.prependListener(event, Wrapper{data});
		return data->handle;
	}

	template <typename Callback, typename Condition>
	typename DispatcherType::Handle insertListener(
			const typename DispatcherType::Event & event,
			const Callback & listener,
			const typename DispatcherType::Handle & before,
			const Condition & condition
		)
	{
		using Wrapper = ItemByCondition<Callback, Condition>;
		auto data = std::make_shared<typename Wrapper::Data>(typename Wrapper::Data {
			condition, dispatcher, event, listener, typename DispatcherType::Handle()
		});
		data->handle = dispatcher.insertListener(event, Wrapper{data}, before);
		return data->handle;
	}

private:
	DispatcherType & dispatcher;
};

template <typename CallbackListType>
class ConditionalRemover <
		CallbackListType,
		typename std::enable_if<std::is_base_of<TagCallbackList, CallbackListType>::value>::type
	>
{
private:
	template <typename Callback, typename Condition>
	struct ItemByCondition
	{
		struct Data
		{
			Condition shouldRemove;
			CallbackListType & callbackList;
			Callback listener;
			typename CallbackListType::Handle handle;
		};

		template <typename ...Args>
		auto operator() (Args && ...args) const
			-> typename std::enable_if<internal_::CanInvoke<Condition, Args...>::value>::type const {
			if(data->shouldRemove(std::forward<Args>(args)...)) {
				data->callbackList.remove(data->handle);
			}
			data->listener(std::forward<Args>(args)...);
		}

		template <typename ...Args>
		auto operator() (Args && ...args) const
			-> typename std::enable_if<! internal_::CanInvoke<Condition, Args...>::value>::type {
			if(data->shouldRemove()) {
				data->callbackList.remove(data->handle);
			}
			data->listener(std::forward<Args>(args)...);
		}
		
		std::shared_ptr<Data> data;
	};

public:
	explicit ConditionalRemover(CallbackListType & callbackList)
		: callbackList(callbackList)
	{
	}
	
	template <typename Callback, typename Condition>
	typename CallbackListType::Handle append(
			const Callback & listener,
			const Condition & condition
		)
	{
		using Wrapper = ItemByCondition<Callback, Condition>;
		auto data = std::make_shared<typename Wrapper::Data>(typename Wrapper::Data {
			condition, callbackList, listener, typename CallbackListType::Handle()
		});
		data->handle = callbackList.append(Wrapper{data});
		return data->handle;
	}

	template <typename Callback, typename Condition>
	typename CallbackListType::Handle prepend(
			const Callback & listener,
			const Condition & condition
		)
	{
		using Wrapper = ItemByCondition<Callback, Condition>;
		auto data = std::make_shared<typename Wrapper::Data>(typename Wrapper::Data {
			condition, callbackList, listener, typename CallbackListType::Handle()
		});
		data->handle = callbackList.prepend(Wrapper{data});
		return data->handle;
	}

	template <typename Callback, typename Condition>
	typename CallbackListType::Handle insert(
			const Callback & listener,
			const typename CallbackListType::Handle & before,
			const Condition & condition
		)
	{
		using Wrapper = ItemByCondition<Callback, Condition>;
		auto data = std::make_shared<typename Wrapper::Data>(typename Wrapper::Data {
			condition, callbackList, listener, typename CallbackListType::Handle()
		});
		data->handle = callbackList.insert(Wrapper{data}, before);
		return data->handle;
	}

private:
	CallbackListType & callbackList;
};

template <typename DispatcherType>
ConditionalRemover<DispatcherType> conditionalRemover(DispatcherType & dispatcher)
{
	return ConditionalRemover<DispatcherType>(dispatcher);
}


} //namespace eventpp

#endif

