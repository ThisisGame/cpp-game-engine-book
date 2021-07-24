# Class AnyId reference

## Description

The template class `AnyId` can be used as the event ID type in `EventDispatcher` and `EventQueue`, then any types can be used as the event type.

For example,  

```c++
eventpp::EventQueue<eventpp::AnyId<>, void()> eventQueue;

eventQueue.appendListener(3, []() {}); // listener 1
eventQueue.appendListener(std::string("hello"), []() {}); // listener 2

eventQueue.dispatch(3); // trigger listener 1
eventQueue.dispatch(std::string("hello")); // trigger listener 2
```

Note the `eventpp::AnyId<>` in the example code, it's an instantiation of `AnyId` with default template parameters.  It's in the place of where an event type should be, such as int.  

Without `AnyId`, a typical EventQueue looks like,  
```c++
eventpp::EventQueue<int, void()> eventQueue;

eventQueue.appendListener(3, []() {});

// This doesn't compile because std::string can't be converted to int
// eventQueue.appendListener(std::string("hello"), []() {});
```
For an `int` event type, we can't use `std::string` as the event ID.

With `AnyId` in previous example code, we can pass any types as the event ID.

## API reference

### Header

eventpp/utilities/anyid.h  

### Class AnyId template parameters

```c++
template <template <typename> class Digester = std::hash, typename Storage = EmptyStorage>
class AnyId;
```
`Digester`: a template class that has one template parameter. It has a function call operator that receives one value and returns the digest of the value. The returned digest must be hashable, i.e, it must be able to be passed to `std::hash`. One of such `Digester` is `std::hash`. The parameter default value is `std::hash`. An event ID that's converted to `AnyId` must be able to pass to `Digester` function call operator. For exmaple, if `Digester` is `std::hash`, the event ID must be hashable, aka, it must be able to be passed to `std::hash`, so `int` and `std::string` works, but `const char *` not.  
`Storage`: a class that can be constructed with any types of values which are going to be used in `AnyId`. One of such `Storage` is `std::any` (in C++17). The parameter default value is an empty storage class that can be constructed with any types and it doesn't hold the value.  

`Digester` is used to convert any types to a specified type and `AnyId` stores the digest instead of the value itself.  
`Storage` is used to store the actural value.  

A typical implementation of `Digester`:  
```c++
template <typename T>
struct MyDigest
{
	TheDigestTypeSuchAsSizeT operator() (const T & value) const {
		// compute the digest of value and return the digest.
	}
};
```
Note: the return type of the function call operator (here is TheDigestTypeSuchAsSizeT) must be the same for all T, it can't be different type for different T.  

A typical implementation of `Storage`:  
```c++
struct MyStorage
{
	template <typename T>
	MyStorage(const T & value) {
		// store the value
	}
	
	// any other member functions can be added, such as getting the underlying value.
};
```
Or none template version:  
```c++
// In this version, only value of `int` and `std::string` can be stored.
struct MyStorage
{
	MyStorage(const int value) {}
	MyStorage(const std::string & value) {}
	
	// any other member functions can be added, such as getting the underlying value.
};
```

### Public types
`DigestType`: the digest type that returned by `Digester`. If `Digester` is `std::hash`, `DigestType` is `std::size_t`.  

### Member functions

#### constructors
```c++
AnyId();

template <typename T>
AnyId(const T & value);
```

Any value can be converted to `AnyId` implicitly.

#### getDigest
```c++
DigestType getDigest() const;
```
Return the digest for the value that passed in the constructor.

```c++
const Storage & getValue() const;
```
Return the value that's stored in `Storage`. The default `Storage` is an empty structure, so you can't get the real value from it.  
If `std::any` is used as the `Storage` parameter when instantiating the `AnyId` template, `getValue` returns the `std::any` thus the value can be obtained from the `std::any`.  

## Global type AnyHashableId

```c++
using AnyHashableId = AnyId<>;
```

`AnyHashableId` is an instantiation of `AnyId` with the default parameters. It can be used in place of the event ID in `EventDispatcher` or `EventQueue`.  
In the example code in the beginning of this document, the `eventpp::AnyId<>` can be replaced with `eventpp::AnyHashableId`.  

## Comparison AnyId

`AnyId` supports `operator ==` for being used in `std::unordered_map`, and `operator <` for being used in `std::map` (which map is used depending on the policies), in `EventDispatcher` and `EventQueue`.  
`AnyId` compares the digest first (the digest must be comparable).  
If the `Storage` supports the operators, the values in the storage are compared. In this case, it doesn't matter if digest collides.  
If the `Storage` doesn't support the operators, only the digests are compared. In this case, if digest collides, the result is in collision.  

## When to use AnyId?

Even though `AnyId` looks smart and very flexible, I highly don't encourage you to use it at all because that means the architecture has flaws. You should always prefer to single event type, such as `int`, or `std::string`, than mixing them.  
If you want to use `AnyHashableId` (aka, `AnyId<>`), don't forget to take into account of the collision created by `std::hash`, and be sure your event IDs don't collide with each other. Instead of using `std::hash`, you may implement more safer digester, such as SHA256.  
If you find there are good reasons to mix the event types and there are good cases to use `AnyId`, you can let me know.  
