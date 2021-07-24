# Class EventDispatcher reference

<!--begintoc-->
## Table Of Contents

* [Description](#a2_1)
* [API reference](#a2_2)
  * [Header](#a3_1)
  * [Template parameters](#a3_2)
  * [Public types](#a3_3)
  * [Member functions](#a3_4)
* [Nested listener safety](#a2_3)
* [Time complexities](#a2_4)
* [Internal data structure](#a2_5)
<!--endtoc-->

<a id="a2_1"></a>
## Description

EventDispatcher is something like std::map<EventType, CallbackList>.

EventDispatcher holds a map of `<EventType, CallbackList>` pairs. On dispatching, EventDispatcher finds the CallbackList of the event type, then invoke the callback list. The invocation is always synchronous. The listeners are triggered when `EventDispatcher::dispatch` is called.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/eventdispatcher.h

<a id="a3_2"></a>
### Template parameters

```c++
template <
	typename Event,
	typename Prototype,
	typename Policies = DefaultPolicies
>
class EventDispatcher;
```
`Event`: the *event type*. The type used to identify the event. Events with same type are the same event. The event type must be able to be used as the key in `std::map` or `std::unordered_map`, so it must be either comparable with `operator <` or has specialization of `std::hash`.  
`Prototype`: the listener prototype. It's C++ function type such as `void(int, std::string, const MyClass *)`.  
`Policies`: the policies to configure and extend the dispatcher. The default value is `DefaultPolicies`. See [document of policies](policies.md) for details.  

<a id="a3_3"></a>
### Public types

`Handle`: the handle type returned by appendListener, prependListener and insertListener. A handle can be used to insert a listener or remove a listener. To check if a `Handle` is empty, convert it to boolean, *false* is empty. `Handle` is copyable.  
`Callback`: the callback storage type.  
`Event`: the event type.  

<a id="a3_4"></a>
### Member functions

#### constructors

```c++
EventDispatcher();
EventDispatcher(const EventDispatcher & other);
EventDispatcher(EventDispatcher && other) noexcept;
EventDispatcher & operator = (const EventDispatcher & other);
EventDispatcher & operator = (EventDispatcher && other) noexcept;
```

EventDispatcher can be copied, moved,  assigned, and move assigned.

#### appendListener

```c++
Handle appendListener(const Event & event, const Callback & callback);
```  
Add the *callback* to the dispatcher to listen to *event*.  
The listener is added to the end of the listener list.  
Return a handle which represents the listener. The handle can be used to remove this listener or insert other listener before this listener.  
If `appendListener` is called in another listener during a dispatching, the new listener is guaranteed not triggered during the same dispatching.  
If the same callback is added twice, it results duplicated listeners.  
The time complexity is O(1).

#### prependListener

```c++
Handle prependListener(const Event & event, const Callback & callback);
```  
Add the *callback* to the dispatcher to listen to *event*.  
The listener is added to the beginning of the listener list.  
Return a handle which represents the listener. The handle can be used to remove this listener or insert other listener before this listener.  
If `prependListener` is called in another listener during a dispatching, the new listener is guaranteed not triggered during the same dispatching.  
The time complexity is O(1).

#### insertListener

```c++
Handle insertListener(const Event & event, const Callback & callback, const Handle before);
```  
Insert the *callback* to the dispatcher to listen to *event* before the listener handle *before*. If *before* is not found, *callback* is added at the end of the listener list.  
Return a handle which represents the listener. The handle can be used to remove this listener or insert other listener before this listener.  
If `insertListener` is called in another listener during a dispatching, the new listener is guaranteed not triggered during the same dispatching.  
The time complexity is O(1).  

#### removeListener

```c++
bool removeListener(const Event & event, const Handle handle);
```  
Remove the listener *handle* which listens to *event* from the dispatcher.  
Return true if the listener is removed successfully, false if the listener is not found.  
The time complexity is O(1).  

#### forEach

```c++
template <typename Func>  
void forEach(const Event & event, Func && func);
```  
Apply `func` to all listeners of `event`.  
The `func` can be one of the two prototypes:  
```c++
AnyReturnType func(const EventDispatcher::Handle &, const EventDispatcher::Callback &);
AnyReturnType func(const EventDispatcher::Callback &);
```
**Note**: the `func` can remove any listeners, or add other listeners, safely.

#### forEachIf

```c++
template <typename Func>  
bool forEachIf(const Event & event, Func && func);
```  
Apply `func` to all listeners of `event`. `func` must return a boolean value, and if the return value is false, forEachIf stops the looping immediately.  
Return `true` if all listeners are invoked, or `event` is not found, `false` if `func` returns `false`.

#### dispatch

```c++
void dispatch(Args ...args);  

template <typename T>  
void dispatch(T && first, Args ...args);
```  
Dispatch an event. The event type is deducted from the arguments of `dispatch`.  
In both overloads, the listeners are called with arguments `args`.  
The function is synchronous. The listeners are called in the thread same as the caller of `dispatch`.

The two overloaded functions have similar but slightly difference. How to use them depends on the `ArgumentPassingMode` policy. Please reference the [document of policies](policies.md) for more information.

<a id="a2_3"></a>
## Nested listener safety
1. If a listener adds another listener of the same event to the dispatcher during a dispatching, the new listener is guaranteed not to be triggered within the same dispatching. This is guaranteed by an unsigned 64 bits integer counter. This rule will be broken is the counter is overflowed to zero in a dispatching, but this rule will continue working on the subsequence dispatching.  
2. Any listeners that are removed during a dispatching are guaranteed not triggered.  
3. All above points are not true in multiple threading. That's to say, if one thread is invoking a callback list, the other thread add or remove a callback, the added or removed callback may be triggered during the invoking.

<a id="a2_4"></a>
## Time complexities
The time complexities being discussed here is about when operating on the listener in the underlying list, and `n` is the number of listeners. It doesn't include the event searching in the underlying `std::map` which is always O(log n).
- `appendListener`: O(1)
- `prependListener`: O(1)
- `insertListener`: O(1)
- `removeListener`: O(1)

<a id="a2_5"></a>
## Internal data structure

EventDispatcher uses [CallbackList](callbacklist.md) to manage the listener callbacks.  
