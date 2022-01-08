# Class CounterRemover reference

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

CounterRemover is a utility class that automatically removes listeners after the listeners are triggered for certain times.  
CounterRemover is a pure functional class. After the member functions in CounterRemover are invoked, the CounterRemover object can be destroyed safely.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/utilities/counterremover.h

<a id="a3_2"></a>
### Template parameters

```c++
template <typename DispatcherType>
class CounterRemover;
```

`DispatcherType` can be CallbackList, EventDispatcher, EventQueue, HeterCallbackList, HeterEventDispatcher, or HeterEventQueue.

<a id="a3_3"></a>
### Member functions

```c++
explicit CounterRemover(DispatcherType & dispatcher);
```

Constructs an instance of CounterRemover.

**Member functions for EventDispatcher and EventQueue**
```c++
typename DispatcherType::Handle appendListener(
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener,
		const int triggerCount = 1
	);

typename DispatcherType::Handle prependListener(
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener,
		const int triggerCount = 1
	);

typename DispatcherType::Handle insertListener(
		const typename DispatcherType::Event & event,
		const typename DispatcherType::Callback & listener,
		const typename DispatcherType::Handle & before,
		const int triggerCount = 1
	);
```

**Member functions for CallbackList**
```c++
typename CallbackListType::Handle append(
		const typename CallbackListType::Callback & listener,
		const int triggerCount = 1
	);

typename CallbackListType::Handle prepend(
		const typename CallbackListType::Callback & listener,
		const int triggerCount = 1
	);

typename CallbackListType::Handle insert(
		const typename CallbackListType::Callback & listener,
		const typename CallbackListType::Handle & before,
		const int triggerCount = 1
	);
```

The member functions have the same names with the corresponding underlying class (CallbackList, EventDispatcher, or EventQueue), and also have the same parameters except there is one more parameter, `triggerCount`. `triggerCount` is decreased by one on each trigger, and when `triggerCount` is zero or negative, the listener will be removed.  
The default value of `triggerCount` is 1, that means the listener is removed after the first trigger, which is one shot listener.

<a id="a3_4"></a>
### Free functions

```c++
template <typename DispatcherType>
CounterRemover<DispatcherType> counterRemover(DispatcherType & dispatcher);
```

Since CounterRemover takes one template parameter and it's verbose to instantiate its instance, the function `counterRemover` is used to construct an instance of CounterRemover via the deduced argument.

<a id="a3_5"></a>
### Sample code

```c++
#include "eventpp/utilities/counterremover.h"
#include "eventpp/eventdispatcher.h"

eventpp::EventDispatcher<int, void ()> dispatcher;
constexpr int event = 3;

dispatcher.appendListener(event, []() {
	// listener A
});

// Note the CounterRemover instance returned by counterRemover is invoked
// prependListener and destroyed immediately.
eventpp::counterRemover(dispatcher).prependListener(event, []() {
	// listener B
});
auto handle = eventpp::counterRemover(dispatcher).appendListener(event, []() {
	// listener C
}, 2);
eventpp::counterRemover(dispatcher).insertListener(event, []() {
	// listener D
}, handle, 3);

dispatcher.dispatch(event);
// All listeners were triggered.
// Listener B was removed.

dispatcher.dispatch(event);
// Listeners A, C, D were triggered.
// Listener C was removed.

dispatcher.dispatch(event);
// Listeners A, D were triggered.
// Listener D was removed.

dispatcher.dispatch(event);
// Listeners A was triggered.

```
