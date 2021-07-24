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
#include "eventpp/callbacklist.h"

#include "tutorial.h"

#include <iostream>

TEST_CASE("CallbackList tutorial 1, basic")
{
	std::cout << std::endl << "CallbackList tutorial 1, basic" << std::endl;

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
}

TEST_CASE("CallbackList tutorial 2, callback with parameters")
{
	std::cout << std::endl << "CallbackList tutorial 2, callback with parameters" << std::endl;

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
}

TEST_CASE("CallbackList tutorial 3, remove")
{
	std::cout << std::endl << "CallbackList tutorial 3, remove" << std::endl;

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
}

TEST_CASE("CallbackList tutorial 4, for each")
{
	std::cout << std::endl << "CallbackList tutorial 4, for each" << std::endl;

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
	callbackList.forEach([&callbackList, &index](const CL::Handle & handle, const CL::Callback & /*callback*/) {
		std::cout << "forEach(Handle, Callback), invoked " << index << std::endl;
		if(index == 1) {
			callbackList.remove(handle);
			std::cout << "forEach(Handle, Callback), removed second callback" << std::endl;
		}
		++index;
	});

	// The forEach callback prototype can also be void(const CallbackList::Callback & callback)
	callbackList.forEach([&callbackList, &index](const CL::Callback & /*callback*/) {
		std::cout << "forEach(Callback), invoked" << std::endl;
	});

	// Invoke the callback list
	// The "Got callback 2" callback should not be triggered.
	callbackList();
}

