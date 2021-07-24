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
#include "eventpp/callbacklist.h"

#include <functional>
#include <vector>

namespace {

#if defined(_MSC_VER)
#define NON_INLINE __declspec(noinline)
#else
// gcc
#define NON_INLINE __attribute__((noinline))
#endif

volatile int globalValue = 0;

void globalFunction(int a, const int b)
{
	globalValue += a + b;
}

NON_INLINE void nonInlineGlobalFunction(int a, const int b)
{
	globalValue += a + b;
}

struct FunctionObject
{
	void operator() (int a, const int b)
	{
		globalValue += a + b;
	}

	virtual void virFunc(int a, const int b)
	{
		globalValue += a + b;
	}

	void nonVirFunc(int a, const int b)
	{
		globalValue += a + b;
	}

	NON_INLINE virtual void nonInlineVirFunc(int a, const int b)
	{
		globalValue += a + b;
	}

	NON_INLINE void nonInlineNonVirFunc(int a, const int b)
	{
		globalValue += a + b;
	}
};
#undef NON_INLINE

template <typename Policies>
using CLT = eventpp::CallbackList<void (int, int), Policies>;
using FLT = std::vector<std::function<void (int, int)> >;

template <typename Policies, typename AddCL, typename AddFL>
void doCallbackListVsFunctionList(const std::string & message, AddCL && addCl, AddFL && addFL)
{
	CLT<Policies> callbackList;
	FLT functionList;
	constexpr int callbackCount = 100;
	constexpr int iterateCount = 1000 * 1000;
	
	for(int i = 0; i < callbackCount; ++i) {
		addCl(callbackList);
		addFL(functionList);
	}
	const uint64_t timeCallbackList = measureElapsedTime(
		[iterateCount, &callbackList]() {
			for(int iterate = 0; iterate < iterateCount; ++iterate) {
				callbackList(iterate, iterate);
			}
		}
	);
	const uint64_t timeFunctionList = measureElapsedTime(
		[iterateCount, &functionList]() {
			for(int iterate = 0; iterate < iterateCount; ++iterate) {
				for(auto & func : functionList) {
					func(iterate, iterate);
				}
			}
		}
	);

	std::cout << message << " timeCallbackList " << timeCallbackList << std::endl;
	std::cout << message << " timeFunctionList " << timeFunctionList << std::endl;
}

} //unnamed namespace

TEST_CASE("b7, CallbackList vs vector of functions")
{
	struct PoliciesMultiThreading {
		using Threading = eventpp::MultipleThreading;
	};
	struct PoliciesSingleThreading {
		using Threading = eventpp::SingleThreading;
	};
	
	struct BenchmarkItem {
		std::string message;
		std::function<void (CLT<PoliciesMultiThreading> &)> addClMulti;
		std::function<void (CLT<PoliciesSingleThreading> &)> addClSingle;
		std::function<void (FLT &)> addFl;
	};
	std::vector<BenchmarkItem> itemList {
		{
			"global",
			[](CLT<PoliciesMultiThreading> & cl) {
				cl.append(&globalFunction);
			},
			[](CLT<PoliciesSingleThreading> & cl) {
				cl.append(&globalFunction);
			},
			[](FLT & fl) {
				fl.push_back(&globalFunction);
			}
		},

		{
			"nonInlineGlobalFunction",
			[](CLT<PoliciesMultiThreading> & cl) {
				cl.append(&nonInlineGlobalFunction);
			},
			[](CLT<PoliciesSingleThreading> & cl) {
				cl.append(&nonInlineGlobalFunction);
			},
			[](FLT & fl) {
				fl.push_back(&nonInlineGlobalFunction);
			}
		},

		{
			"FunctionObject",
			[](CLT<PoliciesMultiThreading> & cl) {
				cl.append(FunctionObject());
			},
			[](CLT<PoliciesSingleThreading> & cl) {
				cl.append(FunctionObject());
			},
			[](FLT & fl) {
				fl.push_back(FunctionObject());
			}
		},

	};
	
	for(BenchmarkItem & item : itemList) {
		doCallbackListVsFunctionList<PoliciesMultiThreading>("Multi thread, " + item.message, item.addClMulti, item.addFl);
	}
	
}
