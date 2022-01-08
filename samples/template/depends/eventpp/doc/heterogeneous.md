# Overview of heterogeneous classes

## Description

'CallbackList', 'EventDispatcher', and 'EventQueue' are homogeneous. All listeners must have the same prototype. For example,

```c++
eventpp::EventDispatcher<int, void ()> dispatcher;
dispatcher.appendListener(3, []() {}); // OK
dispatcher.appendListener(3, [](std::string) {}); // wrong, can't listen for void(std::string)
```

There are heterogeneous counterparts, 'HeterCallbackList', 'HeterEventDispatcher', and 'HeterEventQueue'. These classes allow listeners having different prototypes. For example,

```c++
eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (std::string)> > dispatcher;
dispatcher.appendListener(3, []() {}); // OK
dispatcher.appendListener(3, [](std::string) {}); // OK
```

## Usage

### Headers

eventpp/hetercallbacklist.h  
eventpp/hetereventdispatcher.h  
eventpp/hetereventqueue.h  

### Template parameters

```c++
template <
	typename PrototypeList,
	typename Policies = DefaultPolicies
>
class HeterCallbackList;

template <
	typename Event,
	typename PrototypeList,
	typename Policies = DefaultPolicies
>
class HeterEventDispatcher;

template <
	typename Event,
	typename PrototypeList,
	typename Policies = DefaultPolicies
>
class HeterEventQueue;
```

For comparison, below are the template parameters for the homogeneous counterparts

```c++
template <
	typename Prototype,
	typename Policies = DefaultPolicies
>
class CallbackList;

template <
	typename Event,
	typename Prototype,
	typename Policies = DefaultPolicies
>
class EventDispatcher;

template <
	typename Event,
	typename Prototype,
	typename Policies = DefaultPolicies
>
class EventQueue;
```

The only difference is the `Prototype` in homo-classes becomes `PrototypeList` in heter-classes.  
In the homo-classes, `Prototype` is a single function type such as `void ()`.  
In the heter-classes, `PrototypeList` is a list of function types in `eventpp::HeterTuple`, such as `eventpp::HeterTuple<void (), void (std::string), void (int, int)>`.  
Note: Ideally it would be better to use `std::tuple` instead of `eventpp::HeterTuple`, but the problem is that the tuple is instantiated in HeterEventDispatcher which cause compile error that function type can't be instantiated.
