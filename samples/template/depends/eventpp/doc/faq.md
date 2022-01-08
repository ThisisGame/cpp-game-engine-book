# Frequently Asked Questions

<!--begintoc-->
## Table Of Contents

* [Why can't rvalue reference be used as callback prototype in EventDispatcher and CallbackList? Such as CallbackList<void (int &&)>](#a2_1)
* [Can the callback prototype have return value? Such as CallbackList<std::string (const std::string &, int)>?](#a2_2)
* [Why can't callback prototype be function pointer such as CallbackList<void (*)()>?](#a2_3)
* [Why aren't there APIs to remove listeners directly from an EventDispatcher? Why do we have to remove by handle?](#a2_4)
* [Isn't CallbackList equivalent to std::vector<Callback>? It's simple for me to use std::vector<Callback> directly.](#a2_5)
* [I want to inherit my class from EventDispatcher, but EventDispatcher's destructor is not virtual?](#a2_6)
* [How to automatically remove listeners when certain object is destroyed (aka auto disconnection)?](#a2_7)
* [How to process all EventQueue instances in a single main loop?](#a2_8)
* [How to integrate EventQueue with boost::asio::io_service?](#a2_9)
<!--endtoc-->

<a id="a2_1"></a>
## Why can't rvalue reference be used as callback prototype in EventDispatcher and CallbackList? Such as CallbackList<void (int &&)>

```c++
eventpp::CallbackList<void(std::string &&)> callbackList;
callbackList("Hello"); // compile error
```

The above code doesn't compile. This is intended design and not a bug.  
A rvalue reference `std::string &&` means the argument can be moved by the callback and become invalid (or empty). Keep in mind CallbackList invokes many callbacks one by one. So what happens if the first callback moves the argument and the other callbacks get empty value? In above code example, that means the first callback sees the value "Hello" and moves it, then the other callbacks will see empty string, not "Hello"!  
To avoid such potential bugs, rvalue reference is forbidden deliberately.

<a id="a2_2"></a>
## Can the callback prototype have return value? Such as CallbackList<std::string (const std::string &, int)>?

Yes you can, but both EventDispatcher and CallbackList just discard the return value. It's not efficient nor useful to return value from EventDispatcher and CallbackList.

<a id="a2_3"></a>
## Why can't callback prototype be function pointer such as CallbackList<void (*)()>?

It's rather easy to support function pointer, but it's year 2018 at the time written, and there is proposal for C++20 standard already, so let's use modern C++ features. Stick with function type `void ()` instead of function pointer `void (*)()`.

<a id="a2_4"></a>
## Why aren't there APIs to remove listeners directly from an EventDispatcher? Why do we have to remove by handle?

Both `EventDispatcher::removeListener(const Event & event, const Handle handle)` and `CallbackList::remove(const Handle handle)` requires the handle of a listener is passed in. So why can't we pass the listener object directly? The reason is, it's not guaranteed that the underlying callback storage is comparable while removing a listener object requires the comparable ability. Indeed the default callback storage, `std::function` is not comparable.  
If we use some customized callback storage and we are sure it's comparable, there is free functions 'removeListener' in [utility APIs](eventutil.md).

<a id="a2_5"></a>
## Isn't CallbackList equivalent to std::vector<Callback>? It's simple for me to use std::vector<Callback> directly.

`CallbackList` works like a `std::vector<Callback>`. But one common usage is to implement one-shot callback that a callback removes itself from the callback list when it's invoked. In such case a simple `std::vector<Callback>` will bang and crash.  
With `CallbackList` a callback can be removed at any time, even when the callback list is under invoking.

<a id="a2_6"></a>
## I want to inherit my class from EventDispatcher, but EventDispatcher's destructor is not virtual?

It's intended not to use any virtual functions in eventpp to avoid bloating the code size. New class can still inherit from EventDispatcher, as long as the object is not deleted via a pointer to EventDispatcher, which will cause resource leak. If you need to delete object via pointer to base class, make your own base class that inherits from EventDispatcher, and make the base class destructor virtual.  
For example,  

```c++
class MyEventDispatcher : public EventDispatcher<blah blah>
{
public:
	virtual ~MyEventDispatcher();
};

class MyClass : public MyEventDispatcher
{
}

MyEventDispatcher * myObject = new MyClass();
delete myObject;
```

<a id="a2_7"></a>
## How to automatically remove listeners when certain object is destroyed (aka auto disconnection)?  

[Use utility class ScopedRemover](scopedremover.md)

<a id="a2_8"></a>
## How to process all EventQueue instances in a single main loop?  

It's common to have a single main loop in a GUI or game application, and there are various EventQueue instances in the system. How to process all the EventQueue instances? Let's see some pseudo code first,  

```c++

// Here mainLoopTasks is global for simplify, in real application it can be in some object and passed around
eventpp::CallbackList<void ()> mainLoopTasks;

void mainLoop()
{
	for(;;) {
		// Do any stuff in the loop
		
		mainLoopTasks();
	}
}

class MyEventQueue : public eventpp::EventQueue<blah blah>
{
public:
	MyEventQueue()
	{
		mainLoopTasks.append([this]() {
			process();
		});
	}
};
```

The idea is, the main loop invoke a callback list in each loop, and each event queue registers its process to the callback list.

<a id="a2_9"></a>
## How to integrate EventQueue with boost::asio::io_service?  

A common use case is there are multiple threads that executing boost::asio::io_service::run(). To integrate EventQueue with boost asio, we need to replace `run()` with `poll()` to avoid blocking. So a typical thread will look like,  

```c++
boost::asio::io_service ioService;
eventpp::CallbackList<void ()> mainLoopTasks;

void threadMain()
{
	while(! stopped) {
		ioService.poll();
		mainLoopTasks();
		sleepSomeTime();
	}
	
	ioService.run();
	mainLoopTasks();
}

class MyEventQueue : public eventpp::EventQueue<blah blah>
{
public:
	MyEventQueue()
	{
		mainLoopTasks.append([this]() {
			process();
		});
	}
};
```

Note that after the while loop is finished, the ioService is still run and mainLoopTasks is still invoked, that's to clean up any remaining tasks.
