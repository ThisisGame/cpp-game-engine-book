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

#ifndef HETEREVENTQUEUE_H_210892536510
#define HETEREVENTQUEUE_H_210892536510

#include "hetereventdispatcher.h"

// temp, should move the internal code in eventqueue to separated file
#include "eventqueue.h"

namespace eventpp {

namespace internal_ {

template <
	typename EventType_,
	typename PrototypeList_,
	typename Policies_
>
class HeterEventQueueBase : public HeterEventDispatcherBase<
		EventType_,
		PrototypeList_,
		Policies_,
		HeterEventQueueBase <
			EventType_,
			PrototypeList_,
			Policies_
		>
	>
{
private:
	using super = HeterEventDispatcherBase<
		EventType_,
		PrototypeList_,
		Policies_,
		HeterEventQueueBase <
			EventType_,
			PrototypeList_,
			Policies_
		>
	>;

	using Policies = typename super::Policies;
	using Threading = typename super::Threading;
	using ConditionVariable = typename Threading::ConditionVariable;

	struct QueuedItemBase;
	using ItemDispatcher = void (*)(const HeterEventQueueBase *, const QueuedItemBase &);

	struct QueuedItemBase
	{
		QueuedItemBase(const int callableIndex, const EventType_ & event, const ItemDispatcher dispatcher)
			: callableIndex(callableIndex), event(event), dispatcher(dispatcher)
		{
		}

		int callableIndex;
		EventType_ event;
		ItemDispatcher dispatcher;
	};

	template <typename T>
	struct QueuedItem : public QueuedItemBase
	{
		QueuedItem(const int callableIndex, const EventType_ & event, const ItemDispatcher dispatcher, T && arguments)
			: QueuedItemBase(callableIndex, event, dispatcher), arguments(std::move(arguments))
		{
		}

		T arguments;
	};

	template <typename ...Args>
	using QueuedItemSizer = QueuedItem<std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> >;

	using BufferedQueuedItem = BufferedUnion<GetCallablePrototypeMaxSize<PrototypeList_, QueuedItemSizer>::value>;

	using BufferedItemList = std::list<BufferedQueuedItem>;

	using PrototypeList = typename super::PrototypeList;

	using ArgumentPassingMode = typename super::ArgumentPassingMode;

public:
	using Event = typename super::Event;
	using Handle = typename super::Handle;
	using Mutex = typename super::Mutex;

public:
	HeterEventQueueBase()
		:
		super(),
		queueListConditionVariable(),
		queueEmptyCounter(0),
		queueNotifyCounter(0),
		queueListMutex(),
		queueList(),
		freeListMutex(),
		freeList()
	{
	}

	HeterEventQueueBase(const HeterEventQueueBase & other)
		: super(other)
	{
	}

	HeterEventQueueBase(HeterEventQueueBase && other) noexcept
		: super(std::move(other))
	{
	}

	HeterEventQueueBase & operator = (const HeterEventQueueBase & other)
	{
		super::operator = (other);
		return *this;
	}

	HeterEventQueueBase & operator = (HeterEventQueueBase && other) noexcept
	{
		super::operator = (std::move(other));
		return *this;
	}

	template <typename T, typename ...Args>
	void enqueue(T && first, Args && ...args)
	{
		doEnqueue<ArgumentPassingMode>(std::forward<T>(first), std::forward<Args>(args)...);
	}

	bool emptyQueue() const
	{
		return queueList.empty() && (queueEmptyCounter.load(std::memory_order_acquire) == 0);
	}

	void clearEvents()
	{
		if(! queueList.empty()) {
			BufferedItemList tempList;

			{
				std::lock_guard<Mutex> queueListLock(queueListMutex);
				std::swap(queueList, tempList);
			}

			if(! tempList.empty()) {
				for(auto & item : tempList) {
					item.clear();
				}

				std::lock_guard<Mutex> queueListLock(freeListMutex);
				freeList.splice(freeList.end(), tempList);
			}
		}
	}

	bool process()
	{
		if(! queueList.empty()) {
			BufferedItemList tempList;

			// Use a counter to tell the queue list is not empty during processing
			// even though queueList is swapped to empty.
			CounterGuard<decltype(queueEmptyCounter)> counterGuard(queueEmptyCounter);

			{
				std::lock_guard<Mutex> queueListLock(queueListMutex);
				std::swap(queueList, tempList);
			}

			if(! tempList.empty()) {
				for(auto & item : tempList) {
					doDispatchQueuedEvent(item.template get<QueuedItemBase>());
					item.clear();
				}

				std::lock_guard<Mutex> queueListLock(freeListMutex);
				freeList.splice(freeList.end(), tempList);

				return true;
			}
		}

		return false;
	}

	bool processOne()
	{
		if(! queueList.empty()) {
			BufferedItemList tempList;

			// Use a counter to tell the queue list is not empty during processing
			// even though queueList is swapped to empty.
			CounterGuard<decltype(queueEmptyCounter)> counterGuard(queueEmptyCounter);

			{
				std::lock_guard<Mutex> queueListLock(queueListMutex);
				if(! queueList.empty()) {
					tempList.splice(tempList.end(), queueList, queueList.begin());
				}
			}

			if(! tempList.empty()) {
				auto & item = tempList.front();
				doDispatchQueuedEvent(item.template get<QueuedItemBase>());
				item.clear();

				std::lock_guard<Mutex> queueListLock(freeListMutex);
				freeList.splice(freeList.end(), tempList);

				return true;
			}
		}

		return false;
	}

	template <typename F>
	bool processIf(F && func)
	{
		if(queueList.empty()) {
			return false;
		}

		using PrototypeInfo = FindPrototypeByCallable<PrototypeList, F>;
		return doProcessIf<PrototypeInfo>(std::forward<F>(func));
	}

	void wait() const
	{
		std::unique_lock<Mutex> queueListLock(queueListMutex);
		queueListConditionVariable.wait(queueListLock, [this]() -> bool {
			return doCanProcess();
		});
	}

	template <class Rep, class Period>
	bool waitFor(const std::chrono::duration<Rep, Period> & duration) const
	{
		std::unique_lock<Mutex> queueListLock(queueListMutex);
		return queueListConditionVariable.wait_for(queueListLock, duration, [this]() -> bool {
			return doCanProcess();
		});
	}

	using super::dispatch;

private:
	bool doCanProcess() const
	{
		return ! emptyQueue() && doCanNotifyQueueAvailable();
	}

	bool doCanNotifyQueueAvailable() const
	{
		return queueNotifyCounter.load(std::memory_order_acquire) == 0;
	}

	void doDispatchQueuedEvent(const QueuedItemBase & item)
	{
		item.dispatcher(this, item);
	}

	template <typename PrototypeInfo>
	static void doDispatchItem(const HeterEventQueueBase * self, const QueuedItemBase & baseItem)
	{
		const auto & item = static_cast<const QueuedItem<typename PrototypeInfo::ArgsTuple> &>(baseItem);
		self->doDispatchQueuedItem<PrototypeInfo>(
			item,
			typename MakeIndexSequence<std::tuple_size<typename PrototypeInfo::ArgsTuple>::value>::Type()
		);
	}

	template <typename PrototypeInfo, typename T, size_t ...Indexes>
	void doDispatchQueuedItem(T && item, IndexSequence<Indexes...>) const
	{
		this->directDispatch(item.event, std::get<Indexes>(item.arguments)...);
	}

	template <typename PrototypeInfo, typename F>
	auto doProcessIf(F && func)
		-> typename std::enable_if<(PrototypeInfo::index >= 0), bool>::type
	{
		BufferedItemList tempList;
		BufferedItemList idleList;

		// Use a counter to tell the queue list is not empty during processing
		// even though queueList is swapped to empty.
		CounterGuard<decltype(queueEmptyCounter)> counterGuard(queueEmptyCounter);

		{
			std::lock_guard<Mutex> queueListLock(queueListMutex);
			std::swap(queueList, tempList);
		}

		if(! tempList.empty()) {
			for(auto it = tempList.begin(); it != tempList.end(); ) {
				using ArgsTuple = typename PrototypeInfo::ArgsTuple;
				auto item = it->template get<QueuedItem<ArgsTuple> >();

				if(item.callableIndex != PrototypeInfo::index) {
					++it;
					continue;
				}
				if(doInvokeFuncWithQueuedEvent(
					func,
					item,
					typename MakeIndexSequence<std::tuple_size<ArgsTuple>::value>::Type())
					) {
					doDispatchQueuedEvent(item);
					it->clear();

					auto tempIt = it;
					++it;
					idleList.splice(idleList.end(), tempList, tempIt);
				}
				else {
					++it;
				}
			}

			if (! tempList.empty()) {
				std::lock_guard<Mutex> queueListLock(queueListMutex);
				queueList.splice(queueList.begin(), tempList);
			}

			if(! idleList.empty()) {
				std::lock_guard<Mutex> queueListLock(freeListMutex);
				freeList.splice(freeList.end(), idleList);

				return true;
			}
		}

		using NextPrototypeInfo = FindPrototypeByCallableFromIndex<PrototypeInfo::index + 1, PrototypeList, F>;
		if(doProcessIf<NextPrototypeInfo>(std::forward<F>(func))) {
			return true;
		}

		return false;
	}

	template <typename PrototypeInfo, typename F>
	auto doProcessIf(F && /*func*/)
		-> typename std::enable_if<(PrototypeInfo::index < 0), bool>::type
	{
		return false;
	}

	template <typename F, typename T, size_t ...Indexes>
	bool doInvokeFuncWithQueuedEvent(F && func, T && item, IndexSequence<Indexes...>) const
	{
		return doInvokeFuncWithQueuedEventHelper(std::forward<F>(func), std::get<Indexes>(item.arguments)...);
	}

	template <typename F, typename ...Args>
	bool doInvokeFuncWithQueuedEventHelper(F && func, Args && ...args) const
	{
		return func(std::forward<Args>(args)...);
	}

	template <typename ArgumentMode, typename T, typename ...Args>
	auto doEnqueue(T && first, Args && ...args)
		-> typename std::enable_if<std::is_same<ArgumentMode, ArgumentPassingIncludeEvent>::value>::type
	{
		using GetEvent = typename SelectGetEvent<Policies_, EventType_, HasFunctionGetEvent<Policies_, T &&, Args...>::value>::Type;
		using PrototypeInfo = FindPrototypeByArgs<PrototypeList_, T, Args...>;
		using QueuedItemType = QueuedItem<typename PrototypeInfo::ArgsTuple>;

		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");
		static_assert(std::tuple_size<typename PrototypeInfo::ArgsTuple>::value == 1 + sizeof...(Args), "Arguments count mismatch.");

		doEnqueueItem(QueuedItemType(
			PrototypeInfo::index,
			GetEvent::getEvent(std::forward<T>(first), args...),
			&HeterEventQueueBase::doDispatchItem<PrototypeInfo>,
			typename PrototypeInfo::ArgsTuple(std::forward<T>(first), std::forward<Args>(args)...)
		));

		if(doCanProcess()) {
			queueListConditionVariable.notify_one();
		}
	}

	template <typename ArgumentMode, typename T, typename ...Args>
	auto doEnqueue(T && first, Args && ...args)
		-> typename std::enable_if<std::is_same<ArgumentMode, ArgumentPassingExcludeEvent>::value>::type
	{
		using GetEvent = typename SelectGetEvent<Policies_, EventType_, HasFunctionGetEvent<Policies_, T &&, Args...>::value>::Type;
		using PrototypeInfo = FindPrototypeByArgs<PrototypeList_, Args...>;
		using QueuedItemType = QueuedItem<typename PrototypeInfo::ArgsTuple>;

		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");
		static_assert(std::tuple_size<typename PrototypeInfo::ArgsTuple>::value == sizeof...(Args), "Arguments count mismatch.");

		doEnqueueItem(QueuedItemType(
			PrototypeInfo::index,
			GetEvent::getEvent(std::forward<T>(first), args...),
			&HeterEventQueueBase::doDispatchItem<PrototypeInfo>,
			typename PrototypeInfo::ArgsTuple(std::forward<Args>(args)...)
		));

		if(doCanProcess()) {
			queueListConditionVariable.notify_one();
		}
	}

	template <typename T>
	void doEnqueueItem(T && item)
	{
		BufferedItemList tempList;
		if(! freeList.empty()) {
			{
				std::lock_guard<Mutex> queueListLock(freeListMutex);
				if(! freeList.empty()) {
					tempList.splice(tempList.end(), freeList, freeList.begin());
				}
			}
		}

		if(tempList.empty()) {
			tempList.emplace_back();
		}

		auto it = tempList.begin();
		it->set(std::move(item));

		std::lock_guard<Mutex> queueListLock(queueListMutex);
		queueList.splice(queueList.end(), tempList, it);
	}

private:
	mutable ConditionVariable queueListConditionVariable;
	typename Threading::template Atomic<int> queueEmptyCounter;
	typename Threading::template Atomic<int> queueNotifyCounter;
	mutable Mutex queueListMutex;
	BufferedItemList queueList;
	Mutex freeListMutex;
	BufferedItemList freeList;
};



} //namespace internal_

template <
	typename Event_,
	typename PrototypeList_,
	typename Policies_ = DefaultPolicies
>
class HeterEventQueue : public internal_::InheritMixins<
		internal_::HeterEventQueueBase<Event_, PrototypeList_, Policies_>,
		typename internal_::SelectMixins<Policies_, internal_::HasTypeMixins<Policies_>::value >::Type
	>::Type, public TagEventDispatcher, public TagEventQueue, public TagHeterEventDispatcher, public TagHeterEventQueue
{
private:
	using super = typename internal_::InheritMixins<
		internal_::HeterEventQueueBase<Event_, PrototypeList_, Policies_>,
		typename internal_::SelectMixins<Policies_, internal_::HasTypeMixins<Policies_>::value >::Type
	>::Type;

public:
	using super::super;
};


} //namespace eventpp


#endif

