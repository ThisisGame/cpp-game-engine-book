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

#ifndef EVENTOPTIONS_H_730367862613
#define EVENTOPTIONS_H_730367862613

#include "internal/typeutil_i.h"

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <map>
#include <unordered_map>
#include <list>

namespace eventpp {

struct TagHomo {};
struct TagCallbackList : public TagHomo {};
struct TagEventDispatcher : public TagHomo {};
struct TagEventQueue : public TagHomo {};

struct TagHeter {};
struct TagHeterCallbackList : public TagHeter {};
struct TagHeterEventDispatcher : public TagHeter {};
struct TagHeterEventQueue : public TagHeter {};

struct SpinLock
{
public:
	void lock() {
		while(locked.test_and_set(std::memory_order_acquire)) {
		}
	}

	void unlock() {
		locked.clear(std::memory_order_release);
	}
	
private:
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
};

template <
	typename Mutex_,
	template <typename > class Atomic_ = std::atomic,
	typename ConditionVariable_ = std::condition_variable
>
struct GeneralThreading
{
	using Mutex = Mutex_;

	template <typename T>
	using Atomic = Atomic_<T>;

	using ConditionVariable = ConditionVariable_;
};

struct MultipleThreading
{
	using Mutex = std::mutex;

	template <typename T>
	using Atomic = std::atomic<T>;

	using ConditionVariable = std::condition_variable;
};

struct SingleThreading
{
	struct Mutex
	{
		void lock() {}
		void unlock() {}
	};
	
	template <typename T>
	struct Atomic
	{
		Atomic() noexcept = default;
		constexpr Atomic(T desired) noexcept
			: value(desired)
		{
		}

		void store(T desired, std::memory_order /*order*/ = std::memory_order_seq_cst) noexcept
		{
			value = desired;
		}
		
		T load(std::memory_order /*order*/ = std::memory_order_seq_cst) const noexcept
		{
			return value;
		}
		
		T operator ++ () noexcept
		{
			return ++value;
		}

		T operator -- () noexcept
		{
			return --value;
		}

		T value;
	};

	struct ConditionVariable
	{
		void notify_one() noexcept
		{
		}
		
		template <class Predicate>
		void wait(std::unique_lock<std::mutex> & /*lock*/, Predicate /*pred*/)
		{
		}
		
		template <class Rep, class Period, class Predicate>
		bool wait_for(std::unique_lock<std::mutex> & /*lock*/,
				const std::chrono::duration<Rep, Period> & /*rel_time*/,
				Predicate /*pred*/
			)
		{
			return true;
		}
	};
};

struct ArgumentPassingAutoDetect
{
	enum {
		canIncludeEventType = true,
		canExcludeEventType = true
	};
};

struct ArgumentPassingIncludeEvent
{
	enum {
		canIncludeEventType = true,
		canExcludeEventType = false
	};
};

struct ArgumentPassingExcludeEvent
{
	enum {
		canIncludeEventType = false,
		canExcludeEventType = true
	};
};

struct DefaultPolicies
{
};

template <template <typename> class ...Mixins>
struct MixinList
{
};

#include "internal/eventpolicies_i.h"


} //namespace eventpp


#endif
