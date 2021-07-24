# Event maker macros reference

## Overview

`eventpp/eventmaker.h` contains several macros to generate event classes.  
Without the macros, we may need to repeat the same code structure on lots of event classes.  
For example,  

```c++
// The base event
class Event
{
};

class EventMouseDown : public Event
{
public:
	int getX() const;
	int getY() const;
	int getButton() const;
	
private:
	int x;
	int y;
	int button;
};

class EventKeyDown : public Event
{
public:
	int getKey() const;
	
private:
	int key;
};

```

It's not rare that there are dozens of or even hundreds of event classes in a system, and we need to repeat the same code structure many times, which is boring and error prone.  

The macros in eventmaker.h can reduce the work significantly.

## Header

eventpp/eventmaker.h

## Macro EVENTPP_MAKE_EVENT
```c++
#define EVENTPP_MAKE_EVENT(className, baseClassName, baseClassArgs, ...)
```

EVENTPP_MAKE_EVENT declares an event class.  

**className**: the event class name.  
**baseClassName**: the class name that the event inherits from. The macro requires a base class. If the class name contains comma (,), such as a template class, it should be enclosed within parenthesis. Examples, `Event`, `(AnotherEvent<int, char>)`.  
**baseClassArgs**: the arguments passed to the base class. If the arguments contain comma (,), it should be enclosed within parenthesis. If there is no arguments for the base class, `()` can be used.  
**The variadic arguments (...)**: the tuples of member data type and getter/setter function name. A tuple is elements in a pair of parenthesis. A tuple can contain 2 or 3 elements, the first element is the data type, the second element is the getter function name, and the optional third element is the setter function name. Some tuples examples: `(int, getX)`, `(std::string, getText, setText)`.  
Note: the variadic arguments can't be empty. If the even class doesn't have data member, use `EVENTPP_MAKE_EMPTY_EVENT` instead.

The macro always generates a default constructor and a constructor that takes all data members and initialize them.  

Code examples:  
```c++
EVENTPP_MAKE_EVENT(
	EventDraw, Event, EventType::draw,
	(std::string, getText, setText), (int, getX), (double, getSize)
);
```
Generates class like (in pseudo code),
```c++
class EventDraw : public Event
{
public:
	EventDraw(const std::string & text, const int x, const double size)
		: Event(EventType::draw), text(text), x(x), size(size)
	{
	}
	
	const std::string & getText() const {
		return text;
	}
	
	void setText(const std::string & value) {
		text = value;
	}
	
	const int getX() const {
		return x;
	}
	
	const double getSize() const {
		return size;
	}

private:
	std::string text;
	int x;
	double size;
};

```

```c++
EVENTPP_MAKE_EVENT(EventDraw, (Event<int, char>), EventType::draw,
	(std::string, getText, setText), (int, getX), (double, getSize)
);
```
Similar to above example, except the base class is `Event<int, char>`.  

```c++
EVENTPP_MAKE_EVENT(EventDraw, (Event<int, char>), (EventType::draw, 5),
	(std::string, getText, setText), (int, getX), (double, getSize)
);
```
Similar to above example, except the base class is constructed with `(EventType::draw, 5)` instead of `(EventType::draw)`.  


## Macro EVENTPP_MAKE_EMPTY_EVENT
```c++
#define EVENTPP_MAKE_EMPTY_EVENT(className, baseClassName, baseClassArgs)
```

Similar with `EVENTPP_MAKE_EVENT`, but EVENTPP_MAKE_EMPTY_EVENT declares event class which doesn't have any data member.
