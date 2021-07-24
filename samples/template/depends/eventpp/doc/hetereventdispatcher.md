# Class HeterEventDispatcher reference

<!--begintoc-->
## Table Of Contents

* [Description](#a2_1)
* [API reference](#a2_2)
  * [Header](#a3_1)
  * [Template parameters](#a3_2)
  * [Public types](#a3_3)
  * [Member functions](#a3_4)
<!--endtoc-->

<a id="a2_1"></a>
## Description

HeterEventDispatcher is something like std::map<EventType, HeterCallbackList>.

HeterEventDispatcher holds a map of `<EventType, HeterCallbackList>` pairs. On dispatching, HeterEventDispatcher finds the HeterCallbackList of the event type, then invoke the callback list. The invocation is always synchronous. The listeners are triggered when `HeterEventDispatcher::dispatch` is called.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/hetereventdispatcher.h

<a id="a3_2"></a>
### Template parameters

```c++
template <
	typename Event,
	typename PrototypeList,
	typename Policies = DefaultPolicies
>
class HeterEventDispatcher;
```
`Event`: the *event type*. The type used to identify the event. Events with same type are the same event. The event type must be able to be used as the key in `std::map` or `std::unordered_map`, so it must be either comparable with `operator <` or has specialization of `std::hash`.  
`Prototype`:  a list of function types in `eventpp::HeterTuple`, such as `eventpp::HeterTuple<void (), void (std::string), void (int, int)>`.  
`Policies`: the policies to configure and extend the dispatcher. The default value is `DefaultPolicies`. See [document of policies](policies.md) for details.  

<a id="a3_3"></a>
### Public types

`Handle`: the handle type returned by appendListener, prependListener and insertListener. A handle can be used to insert a listener or remove a listener. To check if a `Handle` is empty, convert it to boolean, *false* is empty. `Handle` is copyable.  
`Event`: the event type.  

<a id="a3_4"></a>
### Member functions

#### constructors

```c++
HeterEventDispatcher();
HeterEventDispatcher(const HeterEventDispatcher & other);
HeterEventDispatcher(HeterEventDispatcher && other) noexcept;
HeterEventDispatcher & operator = (const HeterEventDispatcher & other);
HeterEventDispatcher & operator = (HeterEventDispatcher && other) noexcept;
```

HeterEventDispatcher can be copied, moved,  assigned, and move assigned.

#### appendListener

```c++
template <typename C>
Handle appendListener(const Event & event, const C & callback);
```  
Add the *callback* to the dispatcher to listen to *event*.  
The listener is added to the end of the listener list.  
The callback type `C` must be specified in `PrototypeList`.  
Return a handle which represents the listener. The handle can be used to remove this listener or insert other listener before this listener.  
If `appendListener` is called in another listener during a dispatching, the new listener is guaranteed not triggered during the same dispatching.  
If the same callback is added twice, it results duplicated listeners.  
The time complexity is O(1).

#### prependListener

```c++
template <typename C>
Handle prependListener(const Event & event, const C & callback);
```  
Add the *callback* to the dispatcher to listen to *event*.  
The listener is added to the beginning of the listener list.  
The callback type `C` must be specified in `PrototypeList`.  
Return a handle which represents the listener. The handle can be used to remove this listener or insert other listener before this listener.  
If `prependListener` is called in another listener during a dispatching, the new listener is guaranteed not triggered during the same dispatching.  
The time complexity is O(1).

#### insertListener

```c++
template <typename C>
Handle insertListener(const Event & event, const C & callback, const Handle & before);
```  
Insert the *callback* to the dispatcher to listen to *event* before the listener handle *before*. If *before* is not found, *callback* is added at the end of the listener list.  
The callback type `C` must be specified in `PrototypeList`.  
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
template <typename Prototype, typename Func>
void forEach(const Event & event, Func && func) const;
```  
Apply `func` to all callbacks which has the `Prototype`.  
The `func` can be one of the two prototypes:  
```c++
AnyReturnType func(const HeterEventDispatcher::Handle & handle, const std::function<Prototype> & callback);
AnyReturnType func(const std::function<Prototype> & callback);
```
**Note**: the `func` can remove any listeners, or add other listeners, safely.

#### forEachIf

```c++
template <typename Prototype, typename Func>
bool forEachIf(const Event & event, Func && func) const;
```  
Apply `func` to all listeners of `event`. `func` must return a boolean value, and if the return value is false, forEachIf stops the looping immediately.  
Return `true` if all listeners are invoked, or `event` is not found, `false` if `func` returns `false`.

#### dispatch

```c++
template <typename T, typename ...Args>
void dispatch(T && first, Args && ...args) const
```  
Dispatch an event. The event type is deducted from the arguments of `dispatch`.  
Invoke each callbacks that can be called with `Args` in the callback list.  
The listeners are called with arguments `args`.  
The function is synchronous. The listeners are called in the thread same as the caller of `dispatch`.
