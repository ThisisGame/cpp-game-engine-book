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

#include "test.h"
#include "eventpp/utilities/eventmaker.h"

enum class EventType
{
	draw,
	update,
	message,
	keyDown,
	keyUp
};

class Event
{
public:
	explicit Event(const EventType type) : type(type) {}
	virtual ~Event() {}

	EventType getType() const {
		return type;
	}

private:
	EventType type;
};

EVENTPP_MAKE_EVENT(EventDraw, Event, EventType::draw, (std::string, getText, setText), (int, getX), (double, getSize));

TEST_CASE("eventmake, simple EventDraw")
{
	EventDraw e("Hello", 98, 3.5);
	REQUIRE(e.getType() == EventType::draw);
	REQUIRE(e.getText() == "Hello");
	REQUIRE(e.getX() == 98);
	REQUIRE(e.getSize() == 3.5);

	e.setText("world");
	REQUIRE(e.getText() == "world");
}

template <EventType eventType>
EVENTPP_MAKE_EVENT(EventKey, Event, eventType, (int, getKey));

TEST_CASE("eventmake, templated EventKey")
{
	EventKey<EventType::keyUp> eventKeyUp(0);
	REQUIRE(eventKeyUp.getType() == EventType::keyUp);
}

template <int A, int B>
class TemplatedEvent
{
public:
	TemplatedEvent(const EventType type, const int c) : type(type), c(c) {}
	virtual ~TemplatedEvent() {}

	EventType getType() const {
		return type;
	}

	int getA() const {
		return A;
	}

	int getB() const {
		return B;
	}

	int getC() const {
		return c;
	}

private:
	EventType type;
	int c;
};

EVENTPP_MAKE_EVENT(EventTemplatedDraw, (TemplatedEvent<3, 8>), (EventType::draw, 9), (std::string, getText, setText), (int, getX), (double, getSize));
TEST_CASE("eventmake, TemplatedEvent")
{
	EventTemplatedDraw e("Hello", 98, 3.5);
	
	REQUIRE(e.getA() == 3);
	REQUIRE(e.getB() == 8);
	REQUIRE(e.getC() == 9);

	REQUIRE(e.getType() == EventType::draw);
	REQUIRE(e.getText() == "Hello");
	REQUIRE(e.getX() == 98);
	REQUIRE(e.getSize() == 3.5);

	e.setText("world");
	REQUIRE(e.getText() == "world");
}

