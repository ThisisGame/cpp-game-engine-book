# Class ConditionalRemover reference

<!--begintoc-->
## Table Of Contents

* [Description](#a2_1)
* [API reference](#a2_2)
  * [Header](#a3_1)
  * [Template parameters](#a3_2)
  * [Member functions](#a3_3)
  * [Free functions](#a3_4)
  * [Sample code](#a3_5)
<!--endtoc-->

<a id="a2_1"></a>
## Description

ConditionalRemover is a utility class that automatically removes listeners after the listeners are triggered and certain condition is satisfied.  
ConditionalRemover is a pure functional class. After the member functions in ConditionalRemover are invoked, the ConditionalRemover object can be destroyed safely.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/utilities/conditionalremover.h

<a id="a3_2"></a>
### Template parameters

```c++
template <typename DispatcherType>
class ConditionalRemover;
```

`DispatcherType` can be CallbackList, EventDispatcher, EventQueue, HeterCallbackList, HeterEventDispatcher, or HeterEventQueue.

<a id="a3_3"></a>
### Member functions

```c++
explicit ConditionalRemover(DispatcherType & dispatcher);
```

Constructs an instance of ConditionalRemover.

**Member functions for EventDispatcher and EventQueue**
```c++
template <typename Condition>
typename DispatcherType::Handle appendListener(
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener,
		const Condition & condition
	);

template <typename Condition>
typename DispatcherType::Handle prependListener(
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener,
		const Condition & condition
	);

template <typename Condition>
typename DispatcherType::Handle insertListener(
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener,
		const typename DispatcherType::Handle & before,
		const Condition & condition
	);
```

**Member functions for CallbackList**
```c++
template <typename Condition>
typename CallbackListType::Handle append(
		const typename CallbackListType::Callback & listener,
		const Condition & condition
	);

template <typename Condition>
typename CallbackListType::Handle prepend(
		const typename CallbackListType::Callback & listener,
		const Condition & condition
	);

template <typename Condition>
typename CallbackListType::Handle insert(
		const typename CallbackListType::Callback & listener,
		const typename CallbackListType::Handle & before,
		const Condition & condition
	);
```

The member functions have the same names with the corresponding underlying class (CallbackList, EventDispatcher, or EventQueue), and also have the same parameters except there is one more parameter, `condition`. `condition` is a predicate function that returns a bool value. It's invoked after each trigger, if it returns true, the listener will be removed.  
`condition` can have two kinds of prototype,  

```
bool condition()
```
This `condition` doesn't receive any arguments.

```
bool condition(Args ...args)
```
This `condition` receives the arguments that passed to the listener.

<a id="a3_4"></a>
### Free functions

```c++
template <typename DispatcherType>
ConditionalRemover<DispatcherType> conditionalRemover(DispatcherType & dispatcher);
```

Since ConditionalRemover takes one template parameter and it's verbose to instantiate its instance, the function `conditionalRemover` is used to construct an instance of ConditionalRemover via the deduced argument.

<a id="a3_5"></a>
### Sample code

```c++
#include "eventpp/utilities/conditionalRemover.h"
#include "eventpp/eventdispatcher.h"

eventpp::EventDispatcher<int, void ()> dispatcher;
constexpr int event = 3;

dispatcher.appendListener(event, []() {
	// listener A
});

// Note the ConditionalRemover instance returned by conditionalRemover is invoked
// prependListener and destroyed immediately.
std::string removeWho;
eventpp::conditionalRemover(dispatcher).prependListener(event, [&dataList]() {
	// listener B
}, [&removeWho]() -> bool {
	return removeWho == "removeB";
});
auto handle = eventpp::conditionalRemover(dispatcher).appendListener(event, [&dataList]() {
	// listener C
}, [&removeWho]() -> bool {
	return removeWho == "removeC";
});
eventpp::conditionalRemover(dispatcher).insertListener(event, [&dataList]() {
	// listener D
}, handle, [&removeWho]() -> bool {
	return removeWho == "removeD";
});

dispatcher.dispatch(event);
// No listeners were removed since no conditions were met.

removeWho = "removeB";
dispatcher.dispatch(event);
// All listeners were triggered.
// Listener B was removed.

removeWho = "removeC";
dispatcher.dispatch(event);
// Listeners A, C, D were triggered.
// Listener C was removed.

removeWho = "removeD";
dispatcher.dispatch(event);
// Listeners A, D were triggered.
// Listener D was removed.

dispatcher.dispatch(event);
// Listeners A was triggered.

```
