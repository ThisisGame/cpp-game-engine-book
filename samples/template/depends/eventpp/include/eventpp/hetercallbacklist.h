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

#ifndef HETERCALLBACKLIST_H_966730936077
#define HETERCALLBACKLIST_H_966730936077

#include "internal/hetercallbacklist_i.h"
#include "callbacklist.h"

#include <array>
#include <functional>
#include <type_traits>
#include <mutex>
#include <memory>
#include <tuple>

namespace eventpp {

namespace internal_ {

template <
	typename PrototypeList_,
	typename Policies_
>
class HeterCallbackListBase
{
protected:
	struct HeterHandle_
	{
		int index;
		std::weak_ptr<void> homoHandle;

		operator bool () const noexcept {
			return (bool)homoHandle;
		}
	};

	struct UnderlyingPoliciesType_
	{
	};

	class HomoCallbackListTypeBase
	{
	public:
		virtual bool empty() = 0;
		virtual bool doRemove(const HeterHandle_ & handle) = 0;
		virtual std::shared_ptr<HomoCallbackListTypeBase> doClone() = 0;
	};

	template <typename T>
	class HomoCallbackListType : public CallbackList<T, UnderlyingPoliciesType_>, public HomoCallbackListTypeBase
	{
	private:
		using super = CallbackList<T, UnderlyingPoliciesType_>;

	public:
		virtual bool empty() override {
			return super::empty();
		}

		virtual bool doRemove(const HeterHandle_ & handle) override {
			auto sp = handle.homoHandle.lock();
			if(! sp) {
				return false;
			}
			return this->remove(typename super::Handle(std::static_pointer_cast<typename super::Handle::element_type>(sp)));
		}

		virtual std::shared_ptr<HomoCallbackListTypeBase> doClone() override {
			return std::make_shared<HomoCallbackListType<T> >(*this);
		}
	};

	using Policies = Policies_;
	using Threading = typename SelectThreading<Policies, HasTypeThreading<Policies>::value>::Type;

	using PrototypeList = PrototypeList_;

public:
	using Handle = HeterHandle_;
	using Mutex = typename Threading::Mutex;

public:
	HeterCallbackListBase()
		:
			callbackListList(),
			callbackListListMutex()
	{
	}

	HeterCallbackListBase(const HeterCallbackListBase & other)
		:
			callbackListList(),
			callbackListListMutex()
	{
		for(size_t i = 0; i < callbackListList.size(); ++i) {
			if(other.callbackListList[i]) {
				callbackListList[i] = other.callbackListList[i]->doClone();
			}
		}
	}

	HeterCallbackListBase(HeterCallbackListBase && other) noexcept
		:
			callbackListList(std::move(other.callbackListList)),
			callbackListListMutex()
	{
	}

	// If we use pass by value idiom and omit the 'this' check,
	// when assigning to self there is a deep copy which is inefficient.
	HeterCallbackListBase & operator = (const HeterCallbackListBase & other) noexcept
	{
		if(this != &other) {
			HeterCallbackListBase copied(other);
			swap(copied);
		}
		return *this;
	}

	HeterCallbackListBase & operator = (HeterCallbackListBase && other) noexcept
	{
		if(this != &other) {
			for(size_t i = 0; i < callbackListList.size(); ++i) {
				callbackListList[i] = std::move(other.callbackListList[i]);
			}
		}

		return *this;
	}

	void swap(HeterCallbackListBase & other) noexcept {
		using std::swap;

		swap(callbackListList, other.callbackListList);
	}

	bool empty() const {
		for(const auto & callbackList : callbackListList) {
			if(callbackList && ! callbackList->empty()) {
				return false;
			}
		}

		return true;
	}

	operator bool() const {
		return ! empty();
	}

	template <typename C>
	Handle append(const C & callback)
	{
		using PrototypeInfo = FindPrototypeByCallable<PrototypeList_, C>;
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		auto callbackList= doGetCallbackList<PrototypeInfo>();
		return Handle {
			PrototypeInfo::index,
			callbackList->append(callback)
		};
	}

	template <typename C>
	Handle prepend(const C & callback)
	{
		using PrototypeInfo = FindPrototypeByCallable<PrototypeList_, C>;
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		auto callbackList= doGetCallbackList<PrototypeInfo>();
		return Handle {
			PrototypeInfo::index,
			callbackList->prepend(callback)
		};
	}

	template <typename C>
	Handle insert(const C & callback, const Handle & before)
	{
		using PrototypeInfo = FindPrototypeByCallable<PrototypeList_, C>;
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		auto callbackList= doGetCallbackList<PrototypeInfo>();

		if(before.index != PrototypeInfo::index) {
			return Handle {
				PrototypeInfo::index,
				callbackList->append(callback)
			};
		}

		using UnderlyingHandle = typename decltype(callbackList)::element_type::Handle;

		return Handle {
			PrototypeInfo::index,
			callbackList->insert(
				callback,
				UnderlyingHandle(std::static_pointer_cast<typename UnderlyingHandle::element_type>(before.homoHandle.lock()))
			)
		};
	}

	bool remove(const Handle & handle)
	{
		auto callbackList = callbackListList[handle.index];
		if(callbackList) {
			return callbackList->doRemove(handle);
		}

		return false;
	}

	template <typename Prototype, typename Func>
	void forEach(Func && func) const
	{
		using PrototypeInfo = FindPrototypeByCallable<PrototypeList, Prototype>;
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		auto callbackList= doGetCallbackList<PrototypeInfo>();
		using CL = typename decltype(callbackList)::element_type;
		callbackList->forEach([this, &func](const typename CL::Handle & handle, const typename CL::Callback & callback) {
			doForEachInvoke<void, PrototypeInfo::index>(func, handle, callback);
		});
	}

	template <typename Prototype, typename Func>
	bool forEachIf(Func && func) const
	{
		using PrototypeInfo = FindPrototypeByCallable<PrototypeList, Prototype>;
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		auto callbackList= doGetCallbackList<PrototypeInfo>();
		using CL = typename decltype(callbackList)::element_type;
		return callbackList->forEachIf([this, &func](const typename CL::Handle & handle, const typename CL::Callback & callback) {
			return doForEachInvoke<bool, PrototypeInfo::index>(func, handle, callback);
		});
	}

	template <typename ...Args>
	void operator() (Args && ...args) const
	{
		using PrototypeInfo = FindPrototypeByArgs<PrototypeList, Args...>;
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		auto callbackList= doGetCallbackList<PrototypeInfo>();
		(*callbackList)(std::forward<Args>(args)...);
	}

private:
	template <typename RT, int PrototypeIndex, typename Func, typename H, typename CL>
	auto doForEachInvoke(Func && func, const H & handle, CL && callback) const
		-> typename std::enable_if<CanInvoke<Func, Handle, CL>::value, RT>::type
	{
		return func(Handle { PrototypeIndex, handle }, callback);
	}

	template <typename RT, int PrototypeIndex, typename Func, typename H, typename CL>
	auto doForEachInvoke(Func && func, const H & /*handle*/, CL && callback) const
		-> typename std::enable_if<CanInvoke<Func, CL>::value, RT>::type
	{
		return func(callback);
	}

	template <typename PrototypeInfo>
	auto doGetCallbackList() const
		-> std::shared_ptr<HomoCallbackListType<typename PrototypeInfo::Prototype> >
	{
		static_assert(PrototypeInfo::index >= 0, "Can't find invoker for the given argument types.");

		if(! callbackListList[PrototypeInfo::index]) {
			std::lock_guard<Mutex> lockGuard(callbackListListMutex);

			if(! callbackListList[PrototypeInfo::index]) {
				callbackListList[PrototypeInfo::index] = std::make_shared<HomoCallbackListType<typename PrototypeInfo::Prototype> >();
			}
		}

		return std::static_pointer_cast<HomoCallbackListType<typename PrototypeInfo::Prototype> >(callbackListList[PrototypeInfo::index]);
	}

private:
	// the postfix 'ListList' is not good, but it's better to use consistent naming convention.
	mutable std::array<std::shared_ptr<HomoCallbackListTypeBase>, HeterTupleSize<PrototypeList_>::value> callbackListList;
	mutable Mutex callbackListListMutex;
};


} //namespace internal_

template <
	typename PrototypeList_,
	typename Policies_ = DefaultPolicies
>
class HeterCallbackList : public internal_::HeterCallbackListBase<PrototypeList_, Policies_>, public TagCallbackList, public TagHeterCallbackList
{
private:
	using super = internal_::HeterCallbackListBase<PrototypeList_, Policies_>;

public:
	using super::super;

	friend void swap(HeterCallbackList & first, HeterCallbackList & second) noexcept {
		first.swap(second);
	}
};


} //namespace eventpp

#endif
