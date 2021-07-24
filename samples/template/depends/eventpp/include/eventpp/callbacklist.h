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

#ifndef CALLBACKLIST_H_588722158669
#define CALLBACKLIST_H_588722158669

#include "eventpolicies.h"
#include "internal/typeutil_i.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>

namespace eventpp {

namespace internal_ {

template <
	typename Prototype,
	typename PoliciesType
>
class CallbackListBase;

template <
	typename PoliciesType,
	typename ReturnType, typename ...Args
>
class CallbackListBase<
	ReturnType (Args...),
	PoliciesType
>
{
private:
	using Policies = PoliciesType;

	using Threading = typename SelectThreading<Policies, HasTypeThreading<Policies>::value>::Type;

	using Callback_ = typename SelectCallback<
		Policies,
		HasTypeCallback<Policies>::value,
		std::function<ReturnType (Args...)>
	>::Type;

	using CanContinueInvoking = typename SelectCanContinueInvoking<
		Policies, HasFunctionCanContinueInvoking<Policies, Args...>::value
	>::Type;

	struct Node;
	using NodePtr = std::shared_ptr<Node>;

	struct Node
	{
		using Counter = unsigned int;

		Node(const Callback_ & callback, const Counter counter)
			: callback(callback), counter(counter)
		{
		}

		NodePtr previous;
		NodePtr next;
		Callback_ callback;
		Counter counter;
	};

	class Handle_ : public std::weak_ptr<Node>
	{
	private:
		using super = std::weak_ptr<Node>;

	public:
		using super::super;

		operator bool () const noexcept {
			return ! this->expired();
		}
	};

	using Counter = typename Node::Counter;
	enum : Counter {
		removedCounter = 0
	};

public:
	using Callback = Callback_;
	using Handle = Handle_;
	using Mutex = typename Threading::Mutex;

public:
	CallbackListBase() noexcept
		:
			head(),
			tail(),
			mutex(),
			currentCounter(0)
	{
	}

	CallbackListBase(const CallbackListBase & other)
		: CallbackListBase()
	{
		cloneFrom(other.head);
	}

	CallbackListBase(CallbackListBase && other) noexcept
		: CallbackListBase()
	{
		swap(other);
	}

	// If we use pass by value idiom and omit the 'this' check,
	// when assigning to self there is a deep copy which is inefficient.
	CallbackListBase & operator = (const CallbackListBase & other) {
		if(this != &other) {
			CallbackListBase copied(other);
			swap(copied);
		}
		return *this;
	}

	CallbackListBase & operator = (CallbackListBase && other) noexcept {
		if(this != &other) {
			doFreeAllNodes();

			head = std::move(other.head);
			tail = std::move(other.tail);
			currentCounter = other.currentCounter.load();
		}
		return *this;
	}

	~CallbackListBase()	{
		// Don't lock mutex here since it may throw exception

		doFreeAllNodes();
	}
	
	void swap(CallbackListBase & other) noexcept {
		using std::swap;
		
		swap(head, other.head);
		swap(tail, other.tail);

		const auto value = currentCounter.load();
		currentCounter.exchange(other.currentCounter.load());
		other.currentCounter.exchange(value);
	}

	bool empty() const {
		// Don't lock the mutex for performance reason.
		// !head still works even when the underlying raw pointer is garbled (for other thread is writting to head)
		// And empty() doesn't guarantee the list is still empty after the function returned.
		//std::lock_guard<Mutex> lockGuard(mutex);

		return ! head;
	}

	operator bool() const {
		return ! empty();
	}

	Handle append(const Callback & callback)
	{
		NodePtr node(doAllocateNode(callback));

		std::lock_guard<Mutex> lockGuard(mutex);

		if(head) {
			node->previous = tail;
			tail->next = node;
			tail = node;
		}
		else {
			head = node;
			tail = node;
		}

		return Handle(node);
	}

	Handle prepend(const Callback & callback)
	{
		NodePtr node(doAllocateNode(callback));

		std::lock_guard<Mutex> lockGuard(mutex);

		if(head) {
			node->next = head;
			head->previous = node;
			head = node;
		}
		else {
			head = node;
			tail = node;
		}

		return Handle(node);
	}

	Handle insert(const Callback & callback, const Handle & before)
	{
		NodePtr beforeNode = before.lock();
		if(beforeNode) {
			NodePtr node(doAllocateNode(callback));

			std::lock_guard<Mutex> lockGuard(mutex);

			doInsert(node, beforeNode);

			return Handle(node);
		}

		return append(callback);
	}

	bool remove(const Handle & handle)
	{
		std::lock_guard<Mutex> lockGuard(mutex);
		auto node = handle.lock();
		if(node) {
			doFreeNode(node);
			return true;
		}

		return false;
	}

	template <typename Func>
	void forEach(Func && func) const
	{
		doForEachIf([&func, this](NodePtr & node) -> bool {
			doForEachInvoke<void>(func, node);
			return true;
		});
	}

	template <typename Func>
	bool forEachIf(Func && func) const
	{
		return doForEachIf([&func, this](NodePtr & node) -> bool {
			return doForEachInvoke<bool>(func, node);
		});
	}

#if !defined(__GNUC__) || __GNUC__ >= 5
	void operator() (Args ...args) const
	{
		// We can't use std::forward here, because if we use std::forward,
		// for arg that is passed by value, and the callback prototype accepts it by value,
		// std::forward will move it and may cause the original value invalid.
		// That happens on any value-to-value passing, no matter the callback moves it or not.
		forEachIf([&args...](Callback & callback) -> bool {
			callback(args...);
			return CanContinueInvoking::canContinueInvoking(args...);
		});
	}
#else
	// This is a patch version for GCC 4. It inlines the unrolled doForEachIf.
	// GCC 4.8.3 doesn't supporting parameter pack catpure in lambda, see,
	// https://github.com/wqking/eventpp/issues/19
	// This is a compromised patch for GCC 4, it may be not maintained or updated unless there are bugs.
	// We don't use the patch as main code because the patch generates longer code, and duplicated with doForEachIf.
	void operator() (Args ...args) const
	{
		NodePtr node;

		{
			std::lock_guard<Mutex> lockGuard(mutex);
			node = head;
		}

		const Counter counter = currentCounter.load(std::memory_order_acquire);

		while(node) {
			if(node->counter != removedCounter && counter >= node->counter) {
				node->callback(args...);
				if(! CanContinueInvoking::canContinueInvoking(args...)) {
					break;
				}
			}

			{
				std::lock_guard<Mutex> lockGuard(mutex);
				node = node->next;
			}
		}
	}
#endif

private:
	template <typename F>
	bool doForEachIf(F && f) const
	{
		NodePtr node;

		{
			std::lock_guard<Mutex> lockGuard(mutex);
			node = head;
		}

		const Counter counter = currentCounter.load(std::memory_order_acquire);

		while(node) {
			if(node->counter != removedCounter && counter >= node->counter) {
				if(! f(node)) {
					return false;
				}
			}

			{
				std::lock_guard<Mutex> lockGuard(mutex);
				node = node->next;
			}
		}

		return true;
	}

	template <typename RT, typename Func>
	auto doForEachInvoke(Func && func, NodePtr & node) const
		-> typename std::enable_if<CanInvoke<Func, Handle, Callback &>::value, RT>::type
	{
		return func(Handle(node), node->callback);
	}

	template <typename RT, typename Func>
	auto doForEachInvoke(Func && func, NodePtr & node) const
		-> typename std::enable_if<CanInvoke<Func, Callback &>::value, RT>::type
	{
		return func(node->callback);
	}

	void doInsert(NodePtr & node, NodePtr & beforeNode)
	{
		node->previous = beforeNode->previous;
		node->next = beforeNode;
		if(beforeNode->previous) {
			beforeNode->previous->next = node;
		}
		beforeNode->previous = node;

		if(beforeNode == head) {
			head = node;
		}
	}
	
	NodePtr doAllocateNode(const Callback & callback)
	{
		return std::make_shared<Node>(callback, getNextCounter());
	}
	
	void doFreeNode(NodePtr & node)
	{
		if(node->next) {
			node->next->previous = node->previous;
		}
		if(node->previous) {
			node->previous->next = node->next;
		}

		// Mark it as deleted, this must be before the assignment of head and tail below,
		// because node can be a reference to head or tail, and after the assignment, node
		// can be null pointer.
		node->counter = removedCounter;

		if(head == node) {
			head = node->next;
		}
		if(tail == node) {
			tail = node->previous;
		}

		// don't modify node->previous or node->next
		// because node may be still used in a loop.
	}

	void doFreeAllNodes() {
		NodePtr node = head;
		head.reset();
		while(node) {
			NodePtr next = node->next;
			node->previous.reset();
			node->next.reset();
			node = next;
		}
		node.reset();
	}

	Counter getNextCounter()
	{
		Counter result = ++currentCounter;;
		if(result == 0) { // overflow, let's reset all nodes' counters.
			{
				std::lock_guard<Mutex> lockGuard(mutex);
				NodePtr node = head;
				while(node) {
					node->counter = 1;
					node = node->next;
				}
			}
			result = ++currentCounter;
		}

		return result;
	}
	
	void cloneFrom(const NodePtr & fromHead) {
		NodePtr fromNode(fromHead);
		NodePtr node;
		const Counter counter = getNextCounter();
		while(fromNode) {
			const NodePtr nextNode(std::make_shared<Node>(fromNode->callback, counter));

			nextNode->previous = node;

			if(node) {
				node->next = nextNode;
			}
			else {
				node = nextNode;
				head = node;
			}
		
			node = nextNode;
			fromNode = fromNode->next;
		}

		tail = node;
	}

private:
	NodePtr head;
	NodePtr tail;
	mutable Mutex mutex;
	typename Threading::template Atomic<Counter> currentCounter;

};


} //namespace internal_


template <
	typename Prototype_,
	typename Policies_ = DefaultPolicies
>
class CallbackList : public internal_::CallbackListBase<Prototype_, Policies_>, public TagCallbackList
{
private:
	using super = internal_::CallbackListBase<Prototype_, Policies_>;
	
public:
	using super::super;
	
	friend void swap(CallbackList & first, CallbackList & second) noexcept {
		first.swap(second);
	}
};


} //namespace eventpp


#endif
