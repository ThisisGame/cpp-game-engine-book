# Tutorials of EventQueue

<!--toc-->

## Tutorials

Note if you are going to try the tutorial code, you'd better test the code under the tests/unittest. The sample code in the document may be out of date and not compilable.

### Tutorial 1 -- Basic usage

**Code**  
```c++
eventpp::EventQueue<int, void (const std::string &, std::unique_ptr<int> &)> queue;

queue.appendListener(3, [](const std::string & s, std::unique_ptr<int> & n) {
	std::cout << "Got event 3, s is " << s << " n is " << *n << std::endl;
});
// The listener prototype doesn't need to be exactly same as the dispatcher.
// It would be find as long as the arguments is compatible with the dispatcher.
queue.appendListener(5, [](std::string s, const std::unique_ptr<int> & n) {
	std::cout << "Got event 5, s is " << s << " n is " << *n << std::endl;
});
queue.appendListener(5, [](const std::string & s, std::unique_ptr<int> & n) {
	std::cout << "Got another event 5, s is " << s << " n is " << *n << std::endl;
});

// Enqueue the events, the first argument is always the event type.
// The listeners are not triggered during enqueue.
queue.enqueue(3, "Hello", std::unique_ptr<int>(new int(38)));
queue.enqueue(5, "World", std::unique_ptr<int>(new int(58)));

// Process the event queue, dispatch all queued events.
queue.process();
```

**Output**  
> Got event 3, s is Hello n is 38  
> Got event 5, s is World n is 58  
> Got another event 5, s is World n is 58  

**Remarks**  
`EventDispatcher<>::dispatch()` invokes the listeners synchronously. Sometimes an asynchronous event queue is more useful (think about Windows message queue, or an event queue in a game). EventQueue supports such kind of event queue.  
`EventQueue<>::enqueue()` puts an event to the queue. Its parameters are exactly same as `dispatch`.  
`EventQueue<>::process()` must be called to dispatch the queued events.  
A typical use case is in a GUI application, each components call `EventQueue<>::enqueue()` to post the events, then the main event loop calls `EventQueue<>::process()` to dispatch the events.  
`EventQueue` supports non-copyable object as the event arguments, such as the unique pointer in the tutorial.


### Tutorial 2 -- multiple threading

**Code**  
```c++
using EQ = eventpp::EventQueue<int, void (int)>;
EQ queue;

constexpr int stopEvent = 1;
constexpr int otherEvent = 2;

// Start a thread to process the event queue.
// All listeners are invoked in that thread.
std::thread thread([stopEvent, otherEvent, &queue]() {
	volatile bool shouldStop = false;
	queue.appendListener(stopEvent, [&shouldStop](int) {
		shouldStop = true;
	});
	queue.appendListener(otherEvent, [](const int index) {
		std::cout << "Got event, index is " << index << std::endl;
	});

	while(! shouldStop) {
		queue.wait();

		queue.process();
	}
});

// Enqueue an event from the main thread. After sleeping for 10 milliseconds,
// the event should have be processed by the other thread.
queue.enqueue(otherEvent, 1);
std::this_thread::sleep_for(std::chrono::milliseconds(10));
std::cout << "Should have triggered event with index = 1" << std::endl;

queue.enqueue(otherEvent, 2);
std::this_thread::sleep_for(std::chrono::milliseconds(10));
std::cout << "Should have triggered event with index = 2" << std::endl;

{
	// EventQueue::DisableQueueNotify is a RAII class that
	// disables waking up any waiting threads.
	// So no events should be triggered in this code block.
	// DisableQueueNotify is useful when adding lots of events at the same time
	// and only want to wake up the waiting threads after all events are added.
	EQ::DisableQueueNotify disableNotify(&queue);

	queue.enqueue(otherEvent, 10);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	std::cout << "Should NOT trigger event with index = 10" << std::endl;
	
	queue.enqueue(otherEvent, 11);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	std::cout << "Should NOT trigger event with index = 11" << std::endl;
}
// The DisableQueueNotify object is destroyed here, and has resumed
// waking up waiting threads. So the events should be triggered.
std::this_thread::sleep_for(std::chrono::milliseconds(10));
std::cout << "Should have triggered events with index = 10 and 11" << std::endl;

queue.enqueue(stopEvent, 1);
thread.join();
```

**Output**  
> Got event, index is 1  
> Should have triggered event with index = 1  
> Got event, index is 2  
> Should have triggered event with index = 2  
> Should NOT trigger event with index = 10  
> Should NOT trigger event with index = 11  
> Got event, index is 10  
> Got event, index is 11  
> Should have triggered events with index = 10 and 11  

**Remarks**  
