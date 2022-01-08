# Tutorials of EventDispatcher

<!--toc-->

## Tutorials

Note if you are going to try the tutorial code, you'd better test the code under the tests/unittest. The sample code in the document may be out of date and not compilable.

### Tutorial 1 -- Basic usage

**Code**  
```c++
// The namespace is eventpp
// The first template parameter int is the event type,
// the event type can be any type such as std::string, int, etc.
// The second is the prototype of the listener.
eventpp::EventDispatcher<int, void ()> dispatcher;

// Add a listener. As the type of dispatcher,
// here 3 and 5 is the event type,
// []() {} is the listener.
// Lambda is not required, any function or std::function
// or whatever function object with the required prototype is fine.
dispatcher.appendListener(3, []() {
	std::cout << "Got event 3." << std::endl;
});
dispatcher.appendListener(5, []() {
	std::cout << "Got event 5." << std::endl;
});
dispatcher.appendListener(5, []() {
	std::cout << "Got another event 5." << std::endl;
});

// Dispatch the events, the first argument is always the event type.
dispatcher.dispatch(3);
dispatcher.dispatch(5);
```

**Output**  
> Got event 3.  
> Got event 5.  
> Got another event 5.  

**Remarks**  
First let's define a dispatcher.
```c++
eventpp::EventDispatcher<int, void ()> dispatcher;
```
class EventDispatcher takes two template arguments. The first argument is the *event type*, here is `int`. The second is the *prototype* of the listener.  
The *event type* must be able to use as the key of `std::map`, that's to say, it must support `operator <`.  
The *prototype* is C++ function type, such as `void (int)`, `void (const std::string &, const MyClass &, int, bool)`.  

Now let's add a listener.  
```c++
dispatcher.appendListener(3, []() {
	std::cout << "Got event 3." << std::endl;
});
```
Function `appendListener` takes at least two arguments. The first argument is the *event* of type *event type*, here is `int`. The second is the *callback*.  
The *callback* can be any callback target -- functions, pointers to functions, , pointers to member functions, lambda expressions, and function objects. It must be able to be called with the *prototype* declared in `dispatcher`.  
In the tutorial, we also add two listeners for event 5.  

Now let's dispatch some event.
```c++
dispatcher.dispatch(3);
dispatcher.dispatch(5);
```
Here we dispatched two events, one is event 3, the other is event 5.  
During the dispatching, all listeners of that event will be invoked one by one in the order of they were added.

### Tutorial 2 -- Listener with parameters

**Code**  
```c++
// The listener has two parameters.
eventpp::EventDispatcher<int, void (const std::string &, const bool)> dispatcher;

dispatcher.appendListener(3, [](const std::string & s, const bool b) {
	std::cout << std::boolalpha << "Got event 3, s is " << s << " b is " << b << std::endl;
});
// The listener prototype doesn't need to be exactly same as the dispatcher.
// It would be find as long as the arguments is compatible with the dispatcher.
dispatcher.appendListener(5, [](std::string s, int b) {
	std::cout << std::boolalpha << "Got event 5, s is " << s << " b is " << b << std::endl;
});
dispatcher.appendListener(5, [](const std::string & s, const bool b) {
	std::cout << std::boolalpha << "Got another event 5, s is " << s << " b is " << b << std::endl;
});

// Dispatch the events, the first argument is always the event type.
dispatcher.dispatch(3, "Hello", true);
dispatcher.dispatch(5, "World", false);
```

**Output**  
> Got event 3, s is Hello b is true  
> Got event 5, s is World b is false  
> Got another event 5, s is World b is false  

**Remarks**  
Now the dispatcher callback prototype takes two parameters, `const std::string &` and `const bool`.  
The listener's prototype is not required to be same as the dispatcher, it's fine as long as the prototype is compatible with the dispatcher. See the second listener, `[](std::string s, int b)`, its prototype is not same as the dispatcher.

### Tutorial 3 -- Customized event struct

**Code**  
```c++
// Define an Event to hold all parameters.
struct MyEvent {
	int type;
	std::string message;
	int param;
};

// Define policies to let the dispatcher knows how to
// extract the event type.
struct MyEventPolicies
{
	static int getEvent(const MyEvent & e, bool /*b*/) {
		return e.type;
	}
};

// Pass MyEventPolicies as the third template argument of EventDispatcher.
// Note: the first template argument is the event type type int, not MyEvent.
eventpp::EventDispatcher<
	int,
	void (const MyEvent &, bool),
	MyEventPolicies
> dispatcher;

// Add a listener.
// Note: the first argument is the event type of type int, not MyEvent.
dispatcher.appendListener(3, [](const MyEvent & e, bool b) {
	std::cout
		<< std::boolalpha
		<< "Got event 3" << std::endl
		<< "Event::type is " << e.type << std::endl
		<< "Event::message is " << e.message << std::endl
		<< "Event::param is " << e.param << std::endl
		<< "b is " << b << std::endl
	;
});

// Dispatch the event.
// The first argument is Event.
dispatcher.dispatch(MyEvent { 3, "Hello world", 38 }, true);
```

**Output**  

> Got event 3  
> Event::type is 3  
> Event::message is Hello world  
> Event::param is 38  
> b is true  

**Remarks**
A common situation is an Event class is defined as the base class, all other events derive from Event, and the actual event type is a data member of Event (think QEvent in Qt). To let EventDispatcher knows how to get the event type from class Event, policies (the third template parameter) is used.  
