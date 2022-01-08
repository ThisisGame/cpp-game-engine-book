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

// Include the head
#include "eventpp/hetercallbacklist.h"

#include "tutorial.h"

#include <iostream>

TEST_CASE("HeterCallbackList tutorial 1, basic")
{
	std::cout << std::endl << "HeterCallbackList tutorial 1, basic" << std::endl;

	// The namespace is eventpp
	// the first parameter is a HeterTuple of the listener prototypes.
	eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> > callbackList;

	// Add a callback.
	// []() {} is the callback.
	// Lambda is not required, any function or std::function
	// or whatever function object with the required prototype is fine.
	callbackList.append([]() {
		std::cout << "Got callback for void()." << std::endl;
	});
	callbackList.append([](int) {
		std::cout << "Got callback for void(int)." << std::endl;
	});

	// Invoke the callback list
	// Output: Got callback for void().
	callbackList();
	// Output: Got callback for void(int).
	callbackList(5);
}

TEST_CASE("HeterCallbackList tutorial 2, for each")
{
	std::cout << std::endl << "HeterCallbackList tutorial 2, for each" << std::endl;

	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	CL callbackList;

	// Add some callbacks.
	callbackList.append([]() {
		std::cout << "Got callback void()." << std::endl;
	});
	callbackList.append([]() {
		std::cout << "Got callback void() again." << std::endl;
	});
	callbackList.append([](int /*a*/) {
		std::cout << "Got callback void(int a)." << std::endl;
	});
	callbackList.append([](int /*a*/) {
		std::cout << "Got callback void(int a) again." << std::endl;
	});

	// Now call forEach to remove the second callback
	// forEach has one template parameter, here is void(). It's which prototype to iterate for.
	// The forEach callback prototype is void(const HeterCallbackList::Handle & handle, const std::function<prototype> & callback)
	int index = 0;
	callbackList.forEach<void()>([&callbackList, &index](const CL::Handle & handle, const std::function<void()> & /*callback*/) {
		std::cout << "forEach(Handle, Callback), invoked " << index << std::endl;
		if(index == 1) {
			callbackList.remove(handle);
			std::cout << "forEach(Handle, Callback), removed second callback" << std::endl;
		}
		++index;
	});

	// The forEach callback prototype is void(const std::function<prototype> & callback)
	// We can also use C++14 generic lambda to simplify the callback, for example,
	// callbackList.forEach<void (int)>([&callbackList, &index](const auto & callback)
	callbackList.forEach<void (int)>([&callbackList, &index](const std::function<void(int)> & callback) {
		std::cout << "forEach(Callback), invoked" << std::endl;
		callback(3);
	});

	// Invoke the callback list
	callbackList();
}

