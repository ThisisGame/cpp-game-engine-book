# Mixins

<!--begintoc-->
## Table Of Contents

* [Introduction](#a2_1)
* [Define a mixin](#a2_2)
* [Inject(enable) mixins to EventDispatcher](#a2_3)
* [Optional interceptor points](#a2_4)
* [MixinFilter](#a2_5)
  * [Public type](#a3_1)
  * [Functions](#a3_2)
  * [Sample code for MixinFilter](#a3_3)
<!--endtoc-->

<a id="a2_1"></a>
## Introduction

A mixin is used to inject code in the EventDispatcher/EventQueue inheritance hierarchy to extend the functionalities. In this document we will use EventDispatcher as the example, the usage for EventQueue is exactly the same.  
The original inheritance hierarchy of EventDispatcher looks like,  
```
EventDispatcher <- EventDispatcherBase
```
`EventDispatcher` is an empty class. It inherits all functions and data from `EventDispatcherBase`.

After two mixin A and B are injected, the hierarchy looks like,  
```
EventDispatcher <- MixinA <- MixinB <- EventDispatcherBase
```
The mixins can use all public and protected members (types, functions and data) in EventDispatcherBase. All public members in the mixins are visible and usable by the user.

<a id="a2_2"></a>
## Define a mixin

A mixin is a template class with one template argument. The mixin must inherit from it's template argument.  
A typical mixin should look like,  
```
template <typename Base>
class MyMixin : public Base
{
};
```

<a id="a2_3"></a>
## Inject(enable) mixins to EventDispatcher

To enable mixins, add them to the `Mixins` type in the policies class. For example, to enable `MixinFilter`, define the dispatcher as,   
```c++
struct MyPolicies {
	using Mixins = eventpp::MixinList<eventpp::MixinFilter>;
};
eventpp::EventDispatcher<int, void (), MyPolicies> dispatcher;
```

If there are multiple mixins, such as `using Mixins = eventpp::MixinList<MixinA, MixinB, MixinC>`, then the inheritance hierarchy looks like,  
```
EventDispatcher <- MixinA <- MixinB <- MixinC <- EventDispatcherBase
```
The front mixin is the lowest in the hierarchy.

<a id="a2_4"></a>
## Optional interceptor points

A mixin can have special named functions that are called at certain point. The special functions must be public.  
Currently there is only one special function,  
```c++
template <typename ...Args>
bool mixinBeforeDispatch(Args && ...args) const;
```
`mixinBeforeDispatch` is called before any event is dispatched in both EventDispatcher and EventQueue. It receives the arguments passed to EventDispatcher::dispatch, except that all arguments are passed as lvalue reference, no matter whether they are reference in the callback prototype (of course we can't modify a reference to const). So the function can modify the arguments, then the listeners will see the modified values.  
The function returns `true` to continue the dispatch, `false` will stop any further dispatching.  
For multiple mixins, this function is called in the order of they appearing in MixinList in the policies class.

<a id="a2_5"></a>
## MixinFilter

MixinFilter allows all events are filtered or modified before dispatching.

`MixinFilter::appendFilter(filter)` adds an event filter to the dispatcher. The `filter` receives the arguments which types are the callback prototype with lvalue reference.  

The event filters are invoked for all events, and invoked before any listeners are invoked.  
The event filters can modify the arguments since the arguments are passed as lvalue reference, no matter whether they are reference in the callback prototype (of course we can't modify a reference to const).  

Below table shows the cases of how event filters receive the arguments.

|Argument type in callback prototype |Argument type received by filter |Can filter modify the argument? | Comment |
|-----|-----|:-----:|-----|
|int, const int |int &, int & |Yes |The constness of the value is discarded|
|int &, std::string & |int &, std::string & |Yes ||
|const int &, const int *|const int &, const int * & |No |The constness of the reference/pointer must be respected|

Event filter is a powerful and useful technology, below is some sample use cases, though the real world use cases are unlimited.  

1, Capture and block all interested events. For example, in a GUI window system, all windows can receive mouse events. However, when a window is under mouse dragging, only the window under dragging should receive the mouse events even when the mouse is moving on other window. So when the dragging starts, the window can add a filter. The filter redirects all mouse events to the window and prevent other listeners from the mouse events, and bypass all other events.  

2, Setup catch-all event listener. For example, in a phone book system, the system sends events based on the actions, such as adding a phone number, remove a phone number, look up a phone number, etc. A module may be only interested in special area code of a phone number, not the actions. One approach is the module can listen to all possible events (add, remove, look up), but this is very fragile -- how about a new action event is added and the module forgets to listen on it? The better approach is the module add a filter and check the area code in the filter.

<a id="a3_1"></a>
### Public type

`FilterHandle`: the handle type returned by appendFilter. A filter handle can be used to remove a filter. To check if a `FilterHandle` is empty, convert it to boolean, *false* is empty. `FilterHandle` is copyable.  

<a id="a3_2"></a>
### Functions

```c++
FilterHandle appendFilter(const Filter & filter);
```
Add the *filter* to the dispatcher.  
Return a handle which can be used in removeFilter.

```c++
bool removeFilter(const FilterHandle & filterHandle);
```
Remove a filter from the dispatcher.  
Return true if the filter is removed successfully.

<a id="a3_3"></a>
### Sample code for MixinFilter

**Code**  
```c++
struct MyPolicies {
	using Mixins = eventpp::MixinList<eventpp::MixinFilter>;
};
eventpp::EventDispatcher<int, void (int e, int i, std::string), MyPolicies> dispatcher;

dispatcher.appendListener(3, [](const int e, const int i, const std::string & s) {
	std::cout
		<< "Got event 3, i was 1 but actural is " << i
		<< " s was Hello but actural is " << s
		<< std::endl
	;
});
dispatcher.appendListener(5, [](const int e, const int i, const std::string & s) {
	std::cout << "Shout not got event 5" << std::endl;
});

// Add three event filters.

// The first filter modifies the input arguments to other values, then the subsequence filters
// and listeners will see the modified values.
dispatcher.appendFilter([](const int e, int & i, std::string & s) -> bool {
	std::cout << "Filter 1, e is " << e << " passed in i is " << i << " s is " << s << std::endl;
	i = 38;
	s = "Hi";
	std::cout << "Filter 1, changed i is " << i << " s is " << s << std::endl;
	return true;
});

// The second filter filters out all event of 5. So no listeners on event 5 can be triggered.
// The third filter is not invoked on event 5 also.
dispatcher.appendFilter([](const int e, int & i, std::string & s) -> bool {
	std::cout << "Filter 2, e is " << e << " passed in i is " << i << " s is " << s << std::endl;
	if(e == 5) {
		return false;
	}
	return true;
});

// The third filter just prints the input arguments.
dispatcher.appendFilter([](const int e, int & i, std::string & s) -> bool {
	std::cout << "Filter 3, e is " << e << " passed in i is " << i << " s is " << s << std::endl;
	return true;
});

// Dispatch the events, the first argument is always the event type.
dispatcher.dispatch(3, 1, "Hello");
dispatcher.dispatch(5, 2, "World");
```

**Output**  
> Filter 1, e is 3 passed in i is 1 s is Hello  
> Filter 1, changed i is 38 s is Hi  
> Filter 2, e is 3 passed in i is 38 s is Hi  
> Filter 3, e is 3 passed in i is 38 s is Hi  
> Got event 3, i was 1 but actural is 38 s was Hello but actural is Hi  
> Filter 1, e is 5 passed in i is 2 s is World  
> Filter 1, changed i is 38 s is Hi  
> Filter 2, e is 5 passed in i is 38 s is Hi  

**Remarks**  
