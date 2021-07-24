# Conditional functor reference

## Description

The header file `eventpp/utilities/conditionalfunctor.h` contains utilities that can check certain condition then decide whether to invoke the underlying function.  

For example,  

```c++
eventpp::CallbackList<void(int)> callbackList;

callbackList.append(
	eventpp::conditionalFunctor(
		[](const int value) {
			std::cout << "We should get value 1." << here;
		},
		[](const int value) {
			return value == 1;
		}
	)
);
callbackList(2); // not trigger the callback
callbackList(1); // trigger the callback
```

## Header

eventpp/utilities/conditionalfunctor.h

## API reference

```c++
template <typename Func, typename Condition>
ConditionalFunctor<Func, Condition> conditionalFunctor(Func func, Condition condition);
```

Function `conditionalFunctor` receives a function `func`, and a conditional function `condition`, and return a functor object of `ConditionalFunctor`. `ConditionalFunctor` has a function invoking operator that forwards all arguments to `condition`, if `condition` returns true, `func` will be invoked with same arguments, if `condition` return false, `func` will not be invoked. The return value of `conditionalFunctor` can be passed to CallbackList, EventDispatcher, or EventQueue.  

Below is the example code to demonstrate how to use `conditionalFunctor`, it's grabbed from the unit tests.  

```c++

eventpp::CallbackList<void(int)> callbackList;

std::vector<int> dataList(3);
callbackList.append(
	eventpp::conditionalFunctor(
		[&dataList](const int index) {
			++dataList[index];
		},
		[](const int index) {
			return index == 0;
		}
	)
);
callbackList.append(
	eventpp::conditionalFunctor(
		[&dataList](const int index) {
			++dataList[index];
		},
		[](const int index) {
			return index == 1;
		}
	)
);
callbackList.append(
	eventpp::conditionalFunctor(
		[&dataList](const int index) {
			++dataList[index];
		},
		[](const int index) {
			return index == 2;
		}
	)
);

REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });

callbackList(2);
REQUIRE(dataList == std::vector<int>{ 0, 0, 1 });

callbackList(0);
REQUIRE(dataList == std::vector<int>{ 1, 0, 1 });

callbackList(1);
REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });

```
