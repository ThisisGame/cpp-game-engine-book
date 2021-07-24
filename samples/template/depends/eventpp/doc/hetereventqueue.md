# Class HeterEventQueue reference

<!--begintoc-->
## Table Of Contents

* [Description](#a2_1)
* [API reference](#a2_2)
  * [Header](#a3_1)
  * [Template parameters](#a3_2)
  * [Public types](#a3_3)
  * [Member functions](#a3_4)
  * [Inner class HeterEventQueue::DisableQueueNotify](#a3_5)
<!--endtoc-->

<a id="a2_1"></a>
## Description

HeterEventQueue includes all features of HeterEventDispatcher and adds event queue features. Note: HeterEventQueue doesn't inherit from HeterEventDispatcher, don't try to cast HeterEventQueue to HeterEventDispatcher.  
HeterEventQueue is asynchronous. Events are cached in the queue when `HeterEventQueue::enqueue` is called, and dispatched later when `HeterEventQueue::process` is called.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/eventqueue.h

<a id="a3_2"></a>
### Template parameters

```c++
template <
	typename Event,
	typename PrototypeList,
	typename Policies = DefaultPolicies
>
class HeterEventQueue;
```

HeterEventQueue has the exactly same template parameters with EventDispatcher. Please reference [HeterEventDispatcher document](hetereventdispatcher.md) for details.

<a id="a3_3"></a>
### Public types

<a id="a3_4"></a>
### Member functions

#### constructors

```c++
HeterEventQueue();
HeterEventQueue(const HeterEventQueue & other);
HeterEventQueue(HeterEventQueue && other) noexcept;
HeterEventQueue & operator = (const HeterEventQueue & other);
HeterEventQueue & operator = (HeterEventQueue && other) noexcept;
```

EventDispatcher can be copied, moved,  assigned, and move assigned.  
Note: the queued events are not copied, moved, assigned, or move assigned, only the listeners are performed these operations.

#### enqueue

```c++
template <typename T, typename ...Args>
void enqueue(T && first, Args && ...args);
```  
Put an event into the event queue. The event type is deducted from the arguments of `enqueue`.  
All copyable arguments are copied to internal data structure. All non-copyable but movable arguments are moved.  
HeterEventQueue requires the arguments either copyable or movable.  
If an argument is a reference to a base class and a derived object is passed in, only the base object will be stored and the derived object is lost. Usually shared pointer should be used in such situation.  
If an argument is a pointer, only the pointer will be stored. The object it points to must be available until the event is processed.  
`enqueue` wakes up any threads that are blocked by `wait` or `waitFor`.  
The time complexity is O(1).  

#### process

```c++
bool process();
```  
Process the event queue. All events in the event queue are dispatched once and then removed from the queue.  
The function returns true if any events were processed, false if no event was processed.  
The listeners are called in the thread same as the caller of `process`.  
Any new events added to the queue during `process()` are not dispatched during current `process()`.  
`process()` is efficient in single thread event processing, it processes all events in the queue in current thread. To process events from multiple threads efficiently, use `processOne()`.  
Note: if `process()` is called from multiple threads simultaneously, the events in the event queue are guaranteed dispatched only once.  

#### processOne

```c++
bool processOne();
```  
Process one event in the event queue. The first event in the event queue is dispatched once and then removed from the queue.  
The function returns true if one event was processed, false if no event was processed.  
The listener is called in the thread same as the caller of `processOne`.  
Any new events added to the queue during `processOne()` are not dispatched during current `processOne()`.  
If there are multiple threads processing events, `processOne()` is more efficient than `process()` because it can split the events processing to different threads. However, if there is only one thread processing events, 'process()' is more efficient.  
Note: if `processOne()` is called from multiple threads simultaneously, the events in the event queue are guaranteed dispatched only once.  

#### processIf

```c++
template <typename F>
bool processIf(F && func);
```
Process the event queue. Before processing an event, the event is passed to `func` and the event will be processed only if `func` returns true.  
`func` takes exactly the same arguments as `HeterEventQueue::enqueue`, and returns a boolean value.  
`processIf` returns true if any event was dispatched, false if no event was dispatched.  
`processIf` has some good use scenarios:  
1. Process certain events in certain thread. For example, in a GUI application, the UI related events may be only desired to processed in the main thread.  
2. Process the events until certain time. For example, in a game engine, the event process may be limited to only several milliseconds, the remaining events will be process in next game loop.  

#### emptyQueue

```c++
bool emptyQueue() const;
```
Return true if there is no any event in the event queue, false if there are any events in the event queue.  
Note: in multiple threading environment, the empty state may change immediately after the function returns.  
Note: don't write loop as `while(! eventQueue.emptyQueue()) {}`. It's dead loop since the compiler will inline the code and the change of empty state is never seen by the loop. The safe approach is `while(eventQueue.waitFor(std::chrono::nanoseconds(0))) ;`.  

#### clearEvents

```c++
void clearEvents();
```
Clear all queued events without dispatching them.  
This is useful to clear any references such as shared pointer in the queued events to avoid cyclic reference.

#### wait

```c++
void wait() const;
```
`wait` causes the current thread to block until there is new event arrives in the queue.  
Note: though `wait` has work around with spurious wakeup internally, the queue is not guaranteed not empty after `wait` returns.  
`wait` is useful when a thread processes the event queue. A sampel usage is,
```c++
for(;;) {
	eventQueue.wait();
	eventQueue.process();
}
```
The code works event if it doesn't `wait`, but doing that will waste CPU power resource.

#### waitFor

```c++
template <class Rep, class Period>
bool waitFor(const std::chrono::duration<Rep, Period> & duration) const;
```
Wait for no longer than *duration* time out.  
Return true if the queue is not empty, false if the return is caused by time out.  
`waitFor` is useful when a event queue processing thread has other condition to check. For example,
```c++
std::atomic<bool> shouldStop(false);
for(;;) {
	while(! eventQueue.waitFor(std::chrono::milliseconds(10)) && ! shouldStop.load()) ;
	if(shouldStop.load()) {
		break;
	}

	eventQueue.process();
}
```

<a id="a3_5"></a>
### Inner class HeterEventQueue::DisableQueueNotify  

`HeterEventQueue::DisableQueueNotify` is a RAII class that temporarily prevents the event queue from waking up any waiting threads. When any `DisableQueueNotify` object exist, calling `enqueue` doesn't wake up any threads that are blocked by `wait`. When the `DisableQueueNotify` object is out of scope, the waking up is resumed. If there are more than one `DisableQueueNotify` objects, the waking up is only resumed after all `DisableQueueNotify` objects are destroyed.  
`DisableQueueNotify` is useful to improve performance when batching adding events to the queue. For example, in a main loop of a game engine, `DisableQueueNotify` can be created on the start in a frame, then the game adding events to the queue, and the `DisableQueueNotify` is destroyed at the end of a frame and the events are processed.

To use `DisableQueueNotify`, construct it with a pointer to event queue.

Sample code
```c++
using EQ = eventpp::HeterEventQueue<int, void ()>;
EQ queue;
{
	EQ::DisableQueueNotify disableNotify(&queue);
	// any blocking threads will not be waken up by the below two lines.
	queue.enqueue(1);
	queue.enqueue(2);
}
// any blocking threads are waken up here immediately.

// any blocking threads will be waken up by below line since there is no DisableQueueNotify.
queue.enqueue(3);
```
