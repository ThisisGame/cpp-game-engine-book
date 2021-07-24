# Class OrderedQueueList reference

<!--begintoc-->
## Table Of Contents

* [Description](#a2_1)
* [API reference](#a2_2)
  * [Header](#a3_1)
  * [Template parameters](#a3_2)
  * [Sample code](#a3_3)
<!--endtoc-->

<a id="a2_1"></a>
## Description

OrderedQueueList is a utility class that sorts the events in an EventQueue in certain order.  
With OrderedQueueList, we can dispatch events in certain order such as in priority order.  
This class is used by the QueueList policy. See [document of policies](policies.md) for details.  

<a id="a2_2"></a>
## API reference

<a id="a3_1"></a>
### Header

eventpp/utilities/orderedqueuelist.h

<a id="a3_2"></a>
### Template parameters

```c++
template <typename Item, typename Compare = OrderedQueueListCompare>
class OrderedQueueList;
```

`Item` is used by the policies.  
`Compare` is the comparison function object.  
Its default implementation `OrderedQueueListCompare` looks like,  

```c++
struct OrderedQueueListCompare
{
	template <typename T>
	bool operator() (const T & a, const T & b) const {
		return a.event < b.event;
	}
};
```
The typename T is actually the `EventQueue::QueuedEvent`. But since `OrderedQueueList` is used in the policies that are used to construct the EventQueue, it's not possible to specify the actual type in `OrderedQueueListCompare`, so here we use a template operator.  

`EventQueue::QueuedEvent` is declared as,  

```c++
struct QueuedEvent
{
	EventType event;
	std::tuple<Args> arguments;
};
```
`event` is the event sent to the queue.  
`arguments` are the arguments that passed to `EventQueue::enqueue`.  

<a id="a3_3"></a>
### Sample code

```c++
// First let's define the event struct. e is the event type, priority determines the priority.
struct MyEvent
{
	int e;
	int priority;
};

// The comparison function object used by eventpp::OrderedQueueList.
// The function compares the event by priority.
struct MyCompare
{
	template <typename T>
	bool operator() (const T & a, const T & b) const {
		return std::get<0>(a.arguments).priority > std::get<0>(b.arguments).priority;
	}
};

// Define the EventQueue policy
struct MyPolicy
{
	template <typename Item>
	using QueueList = eventpp::OrderedQueueList<Item, MyCompare >;

	static int getEvent(const MyEvent & event) {
		return event.e;
	}
};

void main()
{
	using EQ = eventpp::EventQueue<int, void(const MyEvent &), MyPolicy>;
	EQ queue;

	queue.appendListener(3, [](const MyEvent & event) {
		std::cout << "Get event " << event.e << "(should be 3)." << " priority: " << event.priority << std::endl;
	});
	queue.appendListener(5, [](const MyEvent & event) {
		std::cout << "Get event " << event.e << "(should be 5)." << " priority: " << event.priority << std::endl;
	});
	queue.appendListener(7, [](const MyEvent & event) {
		std::cout << "Get event " << event.e << "(should be 7)." << " priority: " << event.priority << std::endl;
	});

	// Add an event, the first number 5 is the event type, the second number 100 is the priority.
	// After the queue processes, the events will be processed from higher priority to lower priority.
	queue.enqueue(MyEvent{ 5, 100 });
	queue.enqueue(MyEvent{ 5, 200 });
	queue.enqueue(MyEvent{ 7, 300 });
	queue.enqueue(MyEvent{ 7, 400 });
	queue.enqueue(MyEvent{ 3, 500 });
	queue.enqueue(MyEvent{ 3, 600 });

	queue.process();
}

```
