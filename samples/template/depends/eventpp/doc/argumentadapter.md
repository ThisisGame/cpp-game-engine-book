# Argument adapter reference

## Description

The header file `eventpp/utilities/argumentadapter.h` contains utilities that can cast pass-in argument types to the types of the functioning being called. It's as if the argument types are casted using `static_cast` for most types or `static_pointer_cast` for shared pointers.  

For example,  

```c++
struct Event
{
};

struct MouseEvent : public Event
{
};

eventpp::EventDispatcher<int, void (const Event &)> dispatcher;

// Below line won't compile because the listener parameter `const Event &` can't be converted to `const MouseEvent &` explicitly.
//dispatcher.appendListener(ON_MOUSE_DOWN, [](const MouseEvent &) {});

// This line works.
dispatcher.appendListener(ON_MOUSE_DOWN, eventpp::argumentAdapter<void(const MouseEvent &)>([](const MouseEvent &) {}));
```

## Header

eventpp/utilities/argumentadapter.h

## API reference

```c++
template <template parameters>
ArgumentAdapter<template parameters> argumentAdapter(Func func);
```

Function `argumentAdapter` receives a function `func`, and return a functor object of `ArgumentAdapter`. `ArgumentAdapter` has a function invoking operator that can cast the arguments to match the parameter types of `func`. The return value of `argumentAdapter` can be passed to CallbackList, EventDispatcher, or EventQueue.   
If `func` is a `std::function`, or a pointer to free function, `argumentAdapter` can deduce the parameter types of func, then `argumentAdapter` can be called without any template parameter.  
If `func` is a functor object that `argumentAdapter` can't deduce the parameter types, `argumentAdapter` needs a template parameter which is the prototype of `func`.  
`ArgumentAdapter` converts argument types using `static_cast`. For `std::shared_ptr`, `std::static_pointer_cast` is used. If `static_cast` or `std::static_pointer_cast` can't convert the types, compile errors are issued.  
Caveat: Sucessful type casting doesn't mean correct. For example (pseudo code),  

```c++
class A;
class B : public A;
class C : public A;
eventpp::EventDispatcher<int, void (const A &)> eventDispatcher;
eventDispatcher.appendListener(3, eventpp::argumentAdapter<void(const B &)>([](const B & e) {}));
eventDispatcher.dispatch(3, C());
```

The code can compile successfully, but the listener will receive a C object which is wrongly casted to B, that's a serious problem that most likely will crash the program.
To avoid such mistake, be very careful that the correct types are passed to the corresponding listener. It's usually safe that in an event system, each event type has its own event class, such as a `mouseDown` event has `MouseDown` event class.  
Or you can use [conditionalFunctor](conditionalfunctor.md) to check in advance if the event type matches the desired class. The example2 in below example code shows how to use conditionalFunctor.  

Below is the example code to demonstrate how to use `argumentAdapter`. There are full compile-able example code in file 'tests/tutorial/tutorial_argumentadapter.cpp '.  

```c++
// In the tutorials here, we define an event class hierarchy, Event is the base class.
// The callback prototype in EventDispatcher is reference or pointer to Event,
// then we should only be able to add listeners that only accept reference or pointer to Event,
// not derived class such as MouseEvent.
// But with argumentAdapter, the listeners can accept reference or pointer to MouseEvent,
// and argumentAdapter converts any reference or pointer to Event to MouseEvent automatically, as
// long as object pointed to the reference or pointer is a MouseEvent.

// Define the event types
enum class EventType
{
	// for MouseEvent
	mouse,

	// for KeyEvent
	key,

	// for MessageEvent
	message,

	// For either MouseEvent or KeyEvent, we use this type to demonstrate
	// how to use conditionalFunctor
	input,
};

class Event
{
public:
	Event() {
	}

	// Make the Event polymorphism so we can use dynamic_cast to detect
	// if it's a MouseEvent or KeyEvent
	virtual ~Event() {
	}

};

class MouseEvent : public Event
{
public:
	MouseEvent(const int x, const int y)
		: x(x), y(y)
	{
	}

	int getX() const { return x; }
	int getY() const { return y; }

private:
	int x;
	int y;
};

class KeyEvent : public Event
{
public:
	explicit KeyEvent(const int key)
		: key(key)
	{
	}

	int getKey() const { return key; }

private:
	int key;
};

class MessageEvent : public Event
{
public:
	explicit MessageEvent(const std::string & message)
		: message(message) {
	}

	std::string getMessage() const { return message; }

private:
	std::string message;
};

// A free function that will be added as listener later.
// argumentAdapter works on all types of callables, include but not limited to,
// lambda, functor object, std::function, free function, etc.
void tutorialArgumentAdapterFreeFunction(const MouseEvent & e)
{
	std::cout << "Received MouseEvent in free function, x=" << e.getX() << " y=" << e.getY() << std::endl;
}

void example1()
{
	eventpp::EventDispatcher<EventType, void (const Event &)> eventDispatcher;

	// callback 1 -- lambda, or any functor object

	// This can't compile because a 'const Event &' can be passed to 'const MouseEvent &'
	//eventDispatcher.appendListener(mouseEventId, [](const MouseEvent & e) {});

	// This compiles. eventpp::argumentAdapter creates a functor object that static_cast 
	// 'const Event &' to 'const MouseEvent &' automatically.
	// Note we need to pass the function type to eventpp::argumentAdapter because the lambda
	// doesn't have any function type information and eventpp::argumentAdapter can't deduce
	// the type. This rule also applies to other functor object.
	eventDispatcher.appendListener(
		EventType::mouse,
		eventpp::argumentAdapter<void(const MouseEvent &)>([](const MouseEvent & e) {
			std::cout << "Received MouseEvent in lambda, x=" << e.getX() << " y=" << e.getY() << std::endl;
		})
	);
	eventDispatcher.appendListener(
		EventType::message,
		eventpp::argumentAdapter<void(const MessageEvent &)>([](const MessageEvent & e) {
			std::cout << "Received MessageEvent in lambda, message=" << e.getMessage() << std::endl;
		})
	);

	// callback 2 -- std::function
	// We don't need to pass the function type to eventpp::argumentAdapter because it can
	// deduce the type from the std::function
	eventDispatcher.appendListener(
		EventType::key,
		eventpp::argumentAdapter(std::function<void(const KeyEvent &)>([](const KeyEvent & e) {
			std::cout << "Received KeyEvent in std::function, key=" << e.getKey() << std::endl;
		}))
	);

	// callback 3 -- free function
	// We don't need to pass the function type to eventpp::argumentAdapter because it can
	// deduce the type from the free function
	eventDispatcher.appendListener(
		EventType::mouse,
		eventpp::argumentAdapter(tutorialArgumentAdapterFreeFunction)
	);

	eventDispatcher.dispatch(EventType::mouse, MouseEvent(3, 5));
	eventDispatcher.dispatch(EventType::key, KeyEvent(255));
	eventDispatcher.dispatch(EventType::message, MessageEvent("Hello, argumentAdapter"));
	// In syntax we can dispatch KeyEvent under EventType::mouse, in our case,
	// the EventType::mouse listener casts KeyEvent to MouseEvent, which is invalid object,
	// and the listener will either use garbled data, or crash.
	//eventDispatcher.dispatch(EventType::mouse, KeyEvent(255));
}

void example2()
{
eventpp::EventDispatcher<EventType, void(const Event &)> eventDispatcher;

// Here we add two listener of MouseEvent and KeyEvent under the same event type 'input'.
// We use eventpp::conditionalFunctor to determine whether the Event matches the expected
// event type.

// listener 1
eventDispatcher.appendListener(
	EventType::input,
	eventpp::conditionalFunctor(
		eventpp::argumentAdapter<void(const MouseEvent &)>([](const MouseEvent & e) {
			std::cout << "Received MouseEvent in conditional tutorial, x=" << e.getX() << " y=" << e.getY() << std::endl;
		}),
		// This lambda is the condition. We use dynamic_cast to check if the event is desired.
		// This is for demonstration purpose, in production you may use a better way than dynamic_cast.
		[](const Event & e) { return dynamic_cast<const MouseEvent *>(&e) != nullptr; }
	)
);
// listener 2
eventDispatcher.appendListener(
	EventType::input,
	eventpp::conditionalFunctor(
		eventpp::argumentAdapter<void(const KeyEvent &)>([](const KeyEvent & e) {
			std::cout << "Received KeyEvent in conditional tutorial, key=" << e.getKey() << std::endl;
		}),
		[](const Event & e) { return dynamic_cast<const KeyEvent *>(&e) != nullptr; }
	)
);

// listener 1 will receive this event, listener 2 will not.
eventDispatcher.dispatch(EventType::input, MouseEvent(3, 8));

// listener 2 will receive this event, listener 1 will not.
eventDispatcher.dispatch(EventType::input, KeyEvent(99));
}

```
