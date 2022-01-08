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

// Include the head
#include "eventpp/utilities/argumentadapter.h"

#include "eventpp/utilities/conditionalfunctor.h"
#include "eventpp/eventdispatcher.h"
#include "tutorial.h"

#include <iostream>

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

TEST_CASE("ArgumentAdapter tutorial 1, basic")
{
	std::cout << std::endl << "ArgumentAdapter tutorial 1, basic" << std::endl;

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

TEST_CASE("ArgumentAdapter tutorial 2, arguments with std::shared_ptr")
{
	std::cout << std::endl << "ArgumentAdapter tutorial 2, arguments with std::shared_ptr" << std::endl;

	// Note the argument can't be any reference to std::shared_ptr, such as 'const std::shared_ptr<Event> &',
	// because eventpp::argumentAdapter uses std::static_pointer_cast to cast the pointer and it doesn't
	// work on reference.
	eventpp::EventDispatcher<EventType, void(std::shared_ptr<Event>)> eventDispatcher;

	// This can't compile because a 'std::shared_ptr<Event>' can be passed to 'std::shared_ptr<MouseEvent>'
	//eventDispatcher.appendListener(mouseEventId, [](std::shared_ptr<MouseEvent> e) {});

	// This compiles. eventpp::argumentAdapter creates a functor object that static_cast 
	// 'std::shared_ptr<Event>' to 'std::shared_ptr<MouseEvent>' automatically.
	eventDispatcher.appendListener(
		EventType::mouse,
		eventpp::argumentAdapter<void(std::shared_ptr<MouseEvent>)>([](std::shared_ptr<MouseEvent> e) {
			std::cout << "Received MouseEvent as std::shared_ptr, x=" << e->getX() << " y=" << e->getY() << std::endl;
		})
	);

	eventDispatcher.dispatch(EventType::mouse, std::make_shared<MouseEvent>(3, 5));
}

TEST_CASE("ArgumentAdapter tutorial 3, conditional adapter")
{
	std::cout << std::endl << "ArgumentAdapter tutorial 3, conditional adapter" << std::endl;

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

