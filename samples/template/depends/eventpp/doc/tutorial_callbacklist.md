# Tutorials of CallbackList

<!--toc-->

## Tutorials

Note if you are going to try the tutorial code, you'd better test the code under the tests/unittest. The sample code in the document may be out of date and not compilable.

### CallbackList tutorial 1, basic

**Code**  
```c++
// The namespace is eventpp
// the first parameter is the prototype of the listener.
eventpp::CallbackList<void ()> callbackList;

// Add a callback.
// []() {} is the callback.
// Lambda is not required, any function or std::function
// or whatever function object with the required prototype is fine.
callbackList.append([]() {
	std::cout << "Got callback 1." << std::endl;
});
callbackList.append([]() {
	std::cout << "Got callback 2." << std::endl;
});

// Invoke the callback list
callbackList();
```

**Output**  
> Got callback 1.  
> Got callback 2.  

**Remarks**  
First let's define a callback list.
```c++
eventpp::CallbackList<void ()> callbackList;
```
class CallbackList takes at least one template arguments. It is the *prototype* of the callback.  
The *prototype* is C++ function type, such as `void (int)`, `void (const std::string &, const MyClass &, int, bool)`.  

Now let's add a callback.  
```c++
callbackList.append([]() {
	std::cout << "Got callback 1." << std::endl;
});
```
Function `append` takes one arguments, the *callback*.  
The *callback* can be any callback target -- functions, pointers to functions, , pointers to member functions, lambda expressions, and function objects. It must be able to be called with the *prototype* declared in `callbackList`.  
In the tutorial, we also add another callback.  

Now let's invoke the callbackList.
```c++
callbackList();
```
During the invoking, all callbacks will be invoked one by one in the order of they were added.

### CallbackList tutorial 2, callback with parameters

**Code**  
```c++
// The callback list prototype has two parameters.
eventpp::CallbackList<void (const std::string &, const bool)> callbackList;

callbackList.append([](const std::string & s, const bool b) {
	std::cout << std::boolalpha << "Got callback 1, s is " << s << " b is " << b << std::endl;
});
// The callback prototype doesn't need to be exactly same as the callback list.
// It would be fine as long as the arguments are compatible with the callbacklist.
callbackList.append([](std::string s, int b) {
	std::cout << std::boolalpha << "Got callback 2, s is " << s << " b is " << b << std::endl;
});

// Invoke the callback list
callbackList("Hello world", true);
```

**Output**  
> Got callback 1, s is Hello world b is true  
> Got callback 2, s is Hello world b is 1  

**Remarks**  
Now the callback list prototype takes two parameters, `const std::string &` and `const bool`.  
The callback's prototype is not required to be same as the callback list, it's fine as long as the prototype is compatible with the callback list. See the second callback, `[](std::string s, int b)`, its prototype is not same as the callback list.

### CallbackList tutorial 3, remove

**Code**  
```c++
using CL = eventpp::CallbackList<void ()>;
CL callbackList;

CL::Handle handle2;

// Add some callbacks.
callbackList.append([]() {
	std::cout << "Got callback 1." << std::endl;
});
handle2 = callbackList.append([]() {
	std::cout << "Got callback 2." << std::endl;
});
callbackList.append([]() {
	std::cout << "Got callback 3." << std::endl;
});

callbackList.remove(handle2);

// Invoke the callback list
// The "Got callback 2" callback should not be triggered.
callbackList();
```

**Output**  
> Got callback 1.  
> Got callback 3.  

**Remarks**  

### CallbackList tutorial 4, for each

**Code**  
```c++
using CL = eventpp::CallbackList<void ()>;
CL callbackList;

// Add some callbacks.
callbackList.append([]() {
	std::cout << "Got callback 1." << std::endl;
});
callbackList.append([]() {
	std::cout << "Got callback 2." << std::endl;
});
callbackList.append([]() {
	std::cout << "Got callback 3." << std::endl;
});

// Now call forEach to remove the second callback
// The forEach callback prototype is void(const CallbackList::Handle & handle, const CallbackList::Callback & callback)
int index = 0;
callbackList.forEach([&callbackList, &index](const CL::Handle & handle, const CL::Callback & callback) {
	std::cout << "forEach(Handle, Callback), invoked " << index << std::endl;
	if(index == 1) {
		callbackList.remove(handle);
		std::cout << "forEach(Handle, Callback), removed second callback" << std::endl;
	}
	++index;
});

// The forEach callback prototype can also be void(const CallbackList::Callback & callback)
callbackList.forEach([&callbackList, &index](const CL::Callback & callback) {
	std::cout << "forEach(Callback), invoked" << std::endl;
});

// Invoke the callback list
// The "Got callback 2" callback should not be triggered.
callbackList();
```

**Output**  
> Got callback 1.  
> Got callback 3.  

**Remarks**  
