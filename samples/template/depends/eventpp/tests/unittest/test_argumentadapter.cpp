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

#include "test.h"
#include "eventpp/utilities/argumentadapter.h"
#include "eventpp/callbacklist.h"

class Base
{
public:
	// Intentionally make the functions virtual, so if any non-Base derived object
	// casted to Base wrongly, calling these functions will crash immediately.
	virtual int getValue() const {
		return 0;
	}

	virtual void setValue(const int /*n*/) {
	}

};

class Derived : public Base
{
public:
	explicit Derived(const int value = 0) : Base(), value(value) {
	}

	virtual int getValue() const override {
		return value;
	}
	
	virtual void setValue(const int n) override {
		value = n;
	}

	void addValue(const int n) {
		value += n;
	}

private:
	int value;
};

void setDerivedValue(Derived & obj, const int value)
{
	obj.setValue(value);
}

void addDerivedValue(Derived & obj, const int value)
{
	obj.addValue(value);
}

TEST_CASE("ArgumentAdapter, lambda with std::function")
{
	auto f = [](Derived & obj) { obj.setValue(5); };
	//std::function<void(Base &)> func(f); // won't compile
	std::function<void(Base &)> func(eventpp::argumentAdapter<void(Derived &)>(f));

	Derived obj;
	REQUIRE(obj.getValue() == 0);
	obj.setValue(8);
	REQUIRE(obj.getValue() == 8);
	func(obj);
	REQUIRE(obj.getValue() == 5);
}

TEST_CASE("ArgumentAdapter, std::function with std::function")
{
	std::function<void(Base &, int)> func(eventpp::argumentAdapter(std::function<void(Derived &, int)>(setDerivedValue)));
	
	Derived obj;
	REQUIRE(obj.getValue() == 0);
	obj.setValue(8);
	REQUIRE(obj.getValue() == 8);
	func(obj, 3);
	REQUIRE(obj.getValue() == 3);
}

TEST_CASE("ArgumentAdapter, function with std::function")
{
	std::function<void(Base &, int)> func(eventpp::argumentAdapter(setDerivedValue));
	
	Derived obj;
	REQUIRE(obj.getValue() == 0);
	obj.setValue(8);
	REQUIRE(obj.getValue() == 8);
	func(obj, 3);
	REQUIRE(obj.getValue() == 3);
}

TEST_CASE("ArgumentAdapter, lambda with CallbackList")
{
	eventpp::CallbackList<void (Base &)> callbackList;
	auto f1 = [](Derived & obj) { obj.addValue(1); };
	auto f2 = [](Derived & obj) { obj.addValue(2); };
	//callbackList.append(f1); // won't compile
	callbackList.append(eventpp::argumentAdapter<void(Derived &)>(f1));
	callbackList.append(eventpp::argumentAdapter<void(Derived &)>(f2));
	
	Derived obj;
	REQUIRE(obj.getValue() == 0);
	obj.setValue(8);
	REQUIRE(obj.getValue() == 8);
	callbackList(obj);
	REQUIRE(obj.getValue() == 11);
}

TEST_CASE("ArgumentAdapter, std::function with CallbackList")
{
	eventpp::CallbackList<void (Base &, int)> callbackList;
	callbackList.append(eventpp::argumentAdapter(std::function<void(Derived &, int)>(addDerivedValue)));
	callbackList.append(eventpp::argumentAdapter(std::function<void(Derived &, int)>(addDerivedValue)));
	
	Derived obj;
	REQUIRE(obj.getValue() == 0);
	obj.setValue(8);
	REQUIRE(obj.getValue() == 8);
	callbackList(obj, 5);
	REQUIRE(obj.getValue() == 18);
}

TEST_CASE("ArgumentAdapter, function with CallbackList")
{
	eventpp::CallbackList<void (Base &, int)> callbackList;
	callbackList.append(eventpp::argumentAdapter(addDerivedValue));
	callbackList.append(eventpp::argumentAdapter(addDerivedValue));
	
	Derived obj;
	REQUIRE(obj.getValue() == 0);
	obj.setValue(8);
	REQUIRE(obj.getValue() == 8);
	callbackList(obj, 5);
	REQUIRE(obj.getValue() == 18);
}

TEST_CASE("ArgumentAdapter, std::shared_ptr with CallbackList")
{
	eventpp::CallbackList<void(std::shared_ptr<Base>)> callbackList;
	auto f1 = [](std::shared_ptr<Derived> obj) { obj->addValue(1); };
	auto f2 = [](std::shared_ptr<Derived> obj) { obj->addValue(2); };
	//callbackList.append(f1); // won't compile
	callbackList.append(eventpp::argumentAdapter<void(std::shared_ptr<Derived>)>(f1));
	callbackList.append(eventpp::argumentAdapter<void(std::shared_ptr<Derived>)>(f2));
	
	std::shared_ptr<Derived> obj(std::make_shared<Derived>());
	REQUIRE(obj->getValue() == 0);
	obj->setValue(8);
	REQUIRE(obj->getValue() == 8);
	callbackList(obj);
	REQUIRE(obj->getValue() == 11);
}

/*
TEST_CASE("ArgumentAdapter, check binary code length")
{
	// The code in this test is not to test anything.
	// It's to check if ArgumentAdapter generates any extra code on invoking the underlying functor.
	// To do such check, change enabled to true, then adjust each condition, then run the code in a debugger.
	// The debugger will stop at any __debugbreak, then the developer can examine the disassemblied code.
	// The code between the three __debugbreak in each code block should be same length, unless there are special comments.
	// Don't run the code without a debugger, it will crash.

	bool enabled = false; // don't make it constexpr, otherwise VC will issue warnings

	// The code between the __debugbreak should be same.
	if(enabled && false)
	{
		auto func = [](Derived & obj) { obj.addValue(1); };
		auto adapted = eventpp::argumentAdapter<void(Derived &)>(func);
		Derived storage;
		Base & obj = storage;
		__debugbreak();
		func(static_cast<Derived &>(obj));
		__debugbreak();
		adapted(obj);
		__debugbreak();
	}

	// The code between the __debugbreak should be same.
	if(enabled && false)
	{
		auto func = std::function<void(Derived &)>([](Derived & obj) { obj.addValue(1); });
		auto adapted = eventpp::argumentAdapter<void(Derived &)>(func);
		Derived storage;
		Base & obj = storage;
		__debugbreak();
		func(static_cast<Derived &>(obj));
		__debugbreak();
		adapted(obj);
		__debugbreak();
	}

	// The code between the __debugbreak is not same.
	if(enabled && false)
	{
		auto func = std::function<void(std::shared_ptr<Derived>)>([](std::shared_ptr<Derived> obj) { obj->addValue(1); });
		auto adapted = eventpp::argumentAdapter<void(std::shared_ptr<Derived>)>(func);
		std::shared_ptr<Base> obj(std::make_shared<Derived>());
		__debugbreak();
		func(std::static_pointer_cast<Derived>(obj));
		__debugbreak();
		adapted(obj);
		__debugbreak();
	}
}
*/
