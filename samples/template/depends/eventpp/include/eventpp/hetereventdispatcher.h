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

#ifndef HETEREVENTDISPATCHER_H_127766658555
#define HETEREVENTDISPATCHER_H_127766658555

#include "eventdispatcher.h"
#include "hetercallbacklist.h"
#include "mixins/mixinheterfilter.h"
#include "mixins/mixinfilter.h"

#include <array>
#include <functional>
#include <type_traits>
#include <mutex>
#include <memory>
#include <tuple>

namespace eventpp {

namespace internal_ {

template <
	typename EventType_,
	typename PrototypeList_,
	typename Policies_,
	typename MixinRoot_
>
class HeterEventDispatcherBase
{
protected:
	using ThisType = HeterEventDispatcherBase<
		EventType_,
		PrototypeList_,
		Policies_,
		MixinRoot_
	>;
	using MixinRoot = typename std::conditional<
		std::is_same<MixinRoot_, void>::value,
		ThisType,
		MixinRoot_
	>::type;
	using Policies = Policies_;

	using Threading = typename SelectThreading<Policies, HasTypeThreading<Policies_>::value>::Type;

	using ArgumentPassingMode = typename SelectArgumentPassingMode<
		Policies_,
		HasTypeArgumentPassingMode<Policies_>::value,
		ArgumentPassingExcludeEvent
	>::Type;

	static_assert(! std::is_same<ArgumentPassingMode, ArgumentPassingAutoDetect>::value,
		"ArgumentPassingMode can't be ArgumentPassingAutoDetect in heterogeneous dispatcher.");

	using CallbackList_ = HeterCallbackList<PrototypeList_, Policies_>;

	using Map = typename SelectMap<
		EventType_,
		CallbackList_,
		Policies_,
		HasTemplateMap<Policies_>::value
	>::Type;

	using Mixins = typename internal_::SelectMixins<
		Policies_,
		internal_::HasTypeMixins<Policies_>::value
	>::Type;

public:
	using PrototypeList = PrototypeList_;
	using Handle = typename CallbackList_::Handle;
	using Event = EventType_;
	using Mutex = typename Threading::Mutex;

public:
	HeterEventDispatcherBase()
		:
		eventCallbackListMap(),
		listenerMutex()
	{
	}

	HeterEventDispatcherBase(const HeterEventDispatcherBase & other)
		:
		eventCallbackListMap(other.eventCallbackListMap),
		listenerMutex()
	{
	}

	HeterEventDispatcherBase(HeterEventDispatcherBase && other) noexcept
		:
		eventCallbackListMap(std::move(other.eventCallbackListMap)),
		listenerMutex()
	{
	}

	HeterEventDispatcherBase & operator = (const HeterEventDispatcherBase & other)
	{
		eventCallbackListMap = other.eventCallbackListMap;
		return *this;
	}

	HeterEventDispatcherBase & operator = (HeterEventDispatcherBase && other) noexcept
	{
		eventCallbackListMap = std::move(other.eventCallbackListMap);
		return *this;
	}

	void swap(HeterEventDispatcherBase & other) noexcept {
		using std::swap;

		swap(eventCallbackListMap, other.eventCallbackListMap);
	}

	template <typename C>
	Handle appendListener(const Event & event, const C & callback)
	{
		std::lock_guard<Mutex> lockGuard(listenerMutex);

		return eventCallbackListMap[event].append(callback);
	}

	template <typename C>
	Handle prependListener(const Event & event, const C & callback)
	{
		std::lock_guard<Mutex> lockGuard(listenerMutex);

		return eventCallbackListMap[event].prepend(callback);
	}

	template <typename C>
	Handle insertListener(const Event & event, const C & callback, const Handle & before)
	{
		std::lock_guard<Mutex> lockGuard(listenerMutex);

		return eventCallbackListMap[event].insert(callback, before);
	}

	bool removeListener(const Event & event, const Handle handle)
	{
		CallbackList_ * callableList = doFindCallableList(event);
		if(callableList) {
			return callableList->remove(handle);
		}

		return false;
	}

	template <typename Prototype, typename Func>
	void forEach(const Event & event, Func && func) const
	{
		const CallbackList_ * callableList = doFindCallableList(event);
		if(callableList) {
			callableList->template forEach<Prototype>(std::forward<Func>(func));
		}
	}

	template <typename Prototype, typename Func>
	bool forEachIf(const Event & event, Func && func) const
	{
		const CallbackList_ * callableList = doFindCallableList(event);
		if (callableList) {
			return callableList->template forEachIf<Prototype>(std::forward<Func>(func));
		}

		return true;
	}

	template <typename T, typename ...Args>
	void dispatch(T && first, Args && ...args) const
	{
		doDispatch<ArgumentPassingMode>(std::forward<T>(first), std::forward<Args>(args)...);
	}

	// Bypass any getEvent policy. The first argument is the event type.
	// Most used for internal purpose.
	template <typename ...Args>
	void directDispatch(const Event & e, Args && ...args) const
	{
		if(! internal_::ForEachMixins<MixinRoot, Mixins, DoMixinBeforeDispatch>::forEach(
			this, typename std::add_lvalue_reference<Args>::type(args)...)) {
			return;
		}

		const CallbackList_ * callableList = doFindCallableList(e);
		if(callableList) {
			(*callableList)(std::forward<Args>(args)...);
		}
	}

protected:
	template <typename ArgumentMode, typename T, typename ...Args>
	auto doDispatch(T && first, Args && ...args) const
		-> typename std::enable_if<std::is_same<ArgumentMode, ArgumentPassingIncludeEvent>::value>::type
	{
		if(! internal_::ForEachMixins<MixinRoot, Mixins, DoMixinBeforeDispatch>::forEach(
			this,
			typename std::add_lvalue_reference<T>::type(first),
			typename std::add_lvalue_reference<Args>::type(args)...)
		) {
			return;
		}

		using GetEvent = typename SelectGetEvent<Policies_, EventType_, HasFunctionGetEvent<Policies_, T &&, Args...>::value>::Type;
		const auto e = GetEvent::getEvent(std::forward<T>(first), args...);
		const CallbackList_ * callableList = doFindCallableList(e);
		if(callableList) {
			(*callableList)(std::forward<T>(first), std::forward<Args>(args)...);
		}
	}

	template <typename ArgumentMode, typename T, typename ...Args>
	auto doDispatch(T && first, Args && ...args) const
		-> typename std::enable_if<std::is_same<ArgumentMode, ArgumentPassingExcludeEvent>::value>::type
	{
		if(! internal_::ForEachMixins<MixinRoot, Mixins, DoMixinBeforeDispatch>::forEach(
			this,
			typename std::add_lvalue_reference<Args>::type(args)...)
		) {
			return;
		}

		using GetEvent = typename SelectGetEvent<Policies_, EventType_, HasFunctionGetEvent<Policies_, T &&, Args...>::value>::Type;
		const auto e = GetEvent::getEvent(std::forward<T>(first), args...);
		const CallbackList_ * callableList = doFindCallableList(e);
		if(callableList) {
			(*callableList)(std::forward<Args>(args)...);
		}
	}

	const CallbackList_ * doFindCallableList(const Event & e) const
	{
		return doFindCallableListHelper(this, e);
	}

	CallbackList_ * doFindCallableList(const Event & e)
	{
		return doFindCallableListHelper(this, e);
	}

private:
	// template helper to avoid code duplication in doFindCallableList
	template <typename T>
	static auto doFindCallableListHelper(T * self, const Event & e)
		-> typename std::conditional<std::is_const<T>::value, const CallbackList_ *, CallbackList_ *>::type
	{
		std::lock_guard<Mutex> lockGuard(self->listenerMutex);

		auto it = self->eventCallbackListMap.find(e);
		if(it != self->eventCallbackListMap.end()) {
			return &it->second;
		}
		else {
			return nullptr;
		}

		return 0;
	}

private:
	// Mixin related
	struct DoMixinBeforeDispatch
	{
		template <typename T, typename Self, typename ...A>
		static auto forEach(const Self * self, A && ...args)
			-> typename std::enable_if<HasFunctionMixinBeforeDispatch<T, A...>::value, bool>::type {
			return static_cast<const T *>(self)->mixinBeforeDispatch(std::forward<A>(args)...);
		}

		template <typename T, typename Self, typename ...A>
		static auto forEach(const Self * self, A && ...args)
			-> typename std::enable_if<! HasFunctionMixinBeforeDispatch<T, A...>::value, bool>::type {
			return true;
		}
	};

private:
	Map eventCallbackListMap;
	mutable Mutex listenerMutex;
};

} //namespace internal_


template <
	typename Event_,
	typename PrototypeList_,
	typename Policies_ = DefaultPolicies
>
class HeterEventDispatcher : public internal_::InheritMixins<
		internal_::HeterEventDispatcherBase<Event_, PrototypeList_, Policies_, void>,
		typename internal_::SelectMixins<Policies_, internal_::HasTypeMixins<Policies_>::value >::Type
	>::Type, public TagEventDispatcher, public TagHeterEventDispatcher
{
private:
	using super = typename internal_::InheritMixins<
		internal_::HeterEventDispatcherBase<Event_, PrototypeList_, Policies_, void>,
		typename internal_::SelectMixins<Policies_, internal_::HasTypeMixins<Policies_>::value >::Type
	>::Type;

public:
	using super::super;

	friend void swap(HeterEventDispatcher & first, HeterEventDispatcher & second) noexcept {
		first.swap(second);
	}
};


} //namespace eventpp


#endif

