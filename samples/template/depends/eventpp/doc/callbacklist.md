# Class CallbackList reference

<!--begintoc-->
## Table Of Contents

* [Description](#a2_1)
* [API reference](#a2_2)
  * [Header](#a3_1)
  * [Template parameters](#a3_2)
  * [Public types](#a3_3)
  * [Member functions](#a3_4)
* [Nested callback safety](#a2_3)
* [Time complexities](#a2_4)
* [Internal data structure](#a2_5)
<!--endtoc-->

<a id="a2_1"></a>
## Description

CallbackList is the fundamental class in eventpp. The other classes EventDispatcher and EventQueue are built on CallbackList.  

CallbackList holds a list of callbacks. At the time of the call, CallbackList simply invokes each callback one by one. Consider CallbackList as the signal/slot system in Qt, or the callback function pointer in some Windows APIs (such as lpCompletionRoutine in `ReadFileEx`).  
The *callback* can be any callback target -- functions, pointers to functions, , pointers to member functions, lambda expressions, and function objects.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/callbacklist.h

<a id="a3_2"></a>
### Template parameters

```c++
template <
	typename Prototype,
	typename Policies = DefaultPolicies
>
class CallbackList;
```
`Prototype`:  the callback prototype. It's C++ function type such as `void(int, std::string, const MyClass *)`.  
`Policies`: the policies to configure and extend the callback list. The default value is `DefaultPolicies`. See [document of policies](policies.md) for details.  

<a id="a3_3"></a>
### Public types

`Handle`: the handle type returned by `append`, `prepend` and `insert`. A handle can be used to insert a callback or remove a callback. To check if a `Handle` is empty, convert it to boolean, *false* is empty. `Handle` is copyable.  
`Callback`: the callback storage type.

<a id="a3_4"></a>
### Member functions

#### constructors

```c++
CallbackList() noexcept;
CallbackList(const CallbackList & other);
CallbackList(CallbackList && other) noexcept;
CallbackList & operator = (const CallbackList & other);
CallbackList & operator = (CallbackList && other) noexcept;
```

CallbackList can be copied, moved,  assigned, and move assigned.

#### empty

```c++
bool empty() const;
```
Return true if the callback list is empty.  
Note: in multi threading, this function returning true doesn't guarantee that the list is empty. The list may immediately become non-empty after the function returns true, and vice versa.

#### bool casting operator

```c++
operator bool() const;
```
Return true if the callback list is not empty.  
This operator allows a CallbackList instance be used in condition statement.

#### append

```c++
Handle append(const Callback & callback);
```  
Add the *callback* to the callback list.  
The callback is added to the end of the callback list.  
Return a handle that represents the callback. The handle can be used to remove this callback or to insert additional callbacks before this callback.  
If `append` is called in another callback during the invoking of the callback list, the new callback is guaranteed not to be triggered during the same callback list invoking.  
The time complexity is O(1).

#### prepend

```c++
Handle prepend(const Callback & callback);
```  
Add the *callback* to the callback list.  
The callback is added to the beginning of the callback list.  
Return a handle that represents the callback. The handle can be used to remove this callback or to insert additional callbacks before this callback.  
If `prepend` is called in another callback during the invoking of the callback list, the new callback is guaranteed not to be triggered during the same callback list invoking.  
The time complexity is O(1).

#### insert

```c++
Handle insert(const Callback & callback, const Handle before);
```  
Insert the *callback* to the callback list before the callback handle *before*. If *before* is not found, *callback* is added at the end of the callback list.  
Return a handle that represents the callback. The handle can be used to remove this callback or to insert additional callbacks before this callback.  
If `insert` is called in another callback during the invoking of the callback list, the new callback is guaranteed not to be triggered during the same callback list invoking.  
The time complexity is O(1).  

#### remove
```c++
bool remove(const Handle handle);
```  
Remove the callback *handle* from the callback list.  
Return true if the callback is removed successfully, false if the callback is not found.  
The time complexity is O(1).  

#### forEach

```c++
template <typename Func>  
void forEach(Func && func) const;
```  
Apply `func` to all callbacks.  
The `func` can be one of the two prototypes:  
```c++
AnyReturnType func(const CallbackList::Handle &, const CallbackList::Callback &);
AnyReturnType func(const CallbackList::Callback &);
```
Note: the `func` can remove any callbacks, or add other callbacks, safely.

#### forEachIf

```c++
template <typename Func>  
bool forEachIf(Func && func) const;
```  
Apply `func` to all callbacks. `func` must return a boolean value, and if the return value is false, forEachIf stops the looping immediately.  
Return `true` if all callbacks are invoked, or `event` is not found, `false` if `func` returns `false`.

#### invoking operator

```c++
void operator() (Args ...args) const;
```  
Invoke each callbacks in the callback list.  
The callbacks are called with arguments `args`.  
The callbacks are called in the thread same as the callee of `operator()`.

<a id="a2_3"></a>
## Nested callback safety
1. If a callback adds another callback to the callback list during a invoking, the new callback is guaranteed not to be triggered within the same invoking. This is guaranteed by an unsigned 64 bits integer counter. This rule will be broken is the counter is overflowed to zero in a invoking, but this rule will continue working on the subsequence invoking.  
2. Any callbacks that are removed during a invoking are guaranteed not triggered.  
3. All above points are not true in multiple threading. That's to say, if one thread is invoking a callback list, the other thread add or remove a callback, the added or removed callback may be called during the invoking.


<a id="a2_4"></a>
## Time complexities
- `append`: O(1)
- `prepend`: O(1)
- `insert`: O(1)
- `remove`: O(1)

<a id="a2_5"></a>
## Internal data structure

CallbackList uses doubly linked list to manage the callbacks.  
Each node is linked by a shared pointer. Using shared pointer allows nodes to be removed during iterating.  
