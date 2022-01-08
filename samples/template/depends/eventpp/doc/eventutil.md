# Utilities reference

## Header

eventpp/utilities/eventutil.h

## API reference

```c++
template <typename DispatcherType>
bool removeListener(
	DispatcherType & dispatcher,
	const typename DispatcherType::Event & event,
	const typename DispatcherType::Callback & listener
);
```
The function finds `listener` of `event` in `dispatcher`, if it finds one, removes the listener and returns true, otherwise returns false.  
Note: the function only removes the first found listener. To remove more than one listeners, repeat calling this function until it returns false.  
This function requires the listener be able to be compared with equal operator (==).  

```c++
template <typename CallbackListType>
bool removeListener(
	CallbackListType & callbackList,
	const typename CallbackListType::Callback & callback
);
```
The function finds `callback` in `callbackList`, if it finds one, removes the callback and returns true, otherwise returns false.  
Note: the function only removes the first found callback. To remove more than one callbacks, repeat calling this function until it returns false.  
This function requires the callback be able to be compared with equal operator (==).  

```c++
template <typename DispatcherType>
bool hasListener(
	DispatcherType & dispatcher,
	const typename DispatcherType::Event & event,
	const typename DispatcherType::Callback & listener
);
```
The function finds `listener` of `event` in `dispatcher`, returns true if it finds any one, otherwise returns false.  
This function requires the listener be able to be compared with equal operator (==).  

```c++
template <typename DispatcherType>
bool hasAnyListener(
	DispatcherType & dispatcher,
	const typename DispatcherType::Event & event
);
```
The function finds any listener of `event` in `dispatcher`, returns true if it finds any one, otherwise returns false.  

```c++
template <typename CallbackListType>
bool hasListener(
	CallbackListType & callbackList,
	const typename CallbackListType::Callback & callback
);
```
The function finds `callback` in `callbackList`, returns true if it finds one, otherwise returns false.  
This function requires the callback be able to be compared with equal operator (==).  

```c++
template <typename CallbackListType>
bool hasAnyListener(
	CallbackListType & callbackList
);
```
The function finds any callback in `callbackList`, returns true if it finds any one, otherwise returns false.  
