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


TEST_CASE("b1, CallbackList invoking vs C++ invoking")
{
	constexpr int iterateCount = 1000 * 1000 * 10;
	constexpr int callbackCount = 10;

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					globalFunction(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(&globalFunction);
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(&globalFunction);
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "globalFunction: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					nonInlineGlobalFunction(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(&nonInlineGlobalFunction);
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(&nonInlineGlobalFunction);
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "nonInlineGlobalFunction: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					funcObject(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(funcObject);
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(funcObject);
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "funcObject: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					funcObject.virFunc(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(std::bind(&FunctionObject::virFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(std::bind(&FunctionObject::virFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "funcObject.virFunc: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					funcObject.nonVirFunc(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(std::bind(&FunctionObject::nonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(std::bind(&FunctionObject::nonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "funcObject.nonVirFunc: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					funcObject.nonInlineVirFunc(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(std::bind(&FunctionObject::nonInlineVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(std::bind(&FunctionObject::nonInlineVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "funcObject.nonInlineVirFunc: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, callbackCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				for(int c = 0; c < callbackCount; ++c) {
					funcObject.nonInlineNonVirFunc(i, i);
				}
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListSingleThreading.append(std::bind(&FunctionObject::nonInlineNonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		for(int c = 0; c < callbackCount; ++c) {
			callbackListMultiThreading.append(std::bind(&FunctionObject::nonInlineNonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		}
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "funcObject.nonInlineNonVirFunc: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}

	{
		FunctionObject funcObject;
		const uint64_t cppTime = measureElapsedTime([iterateCount, &funcObject]() {
			for(int i = 0; i < iterateCount; ++i) {
				globalFunction(i, i);
				nonInlineGlobalFunction(i, i);
				funcObject(i, i);
				funcObject.virFunc(i, i);
				funcObject.nonVirFunc(i, i);
				funcObject.nonInlineVirFunc(i, i);
				funcObject.nonInlineNonVirFunc(i, i);
			}
		});

		struct SingleThreadingPolicies {
			using Threading = eventpp::SingleThreading;
		};
		eventpp::CallbackList<void (int, int), SingleThreadingPolicies> callbackListSingleThreading;
		callbackListSingleThreading.append(&globalFunction);
		callbackListSingleThreading.append(&nonInlineGlobalFunction);
		callbackListSingleThreading.append(funcObject);
		callbackListSingleThreading.append(std::bind(&FunctionObject::virFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		callbackListSingleThreading.append(std::bind(&FunctionObject::nonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		callbackListSingleThreading.append(std::bind(&FunctionObject::nonInlineVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		callbackListSingleThreading.append(std::bind(&FunctionObject::nonInlineNonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		const uint64_t callbackListSingleThreadingTime = measureElapsedTime([iterateCount, &callbackListSingleThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListSingleThreading(i, i);
			}
		});

		struct MultiThreadingPolicies {
			using Threading = eventpp::MultipleThreading;
		};
		eventpp::CallbackList<void (int, int), MultiThreadingPolicies> callbackListMultiThreading;
		callbackListMultiThreading.append(&globalFunction);
		callbackListMultiThreading.append(&nonInlineGlobalFunction);
		callbackListMultiThreading.append(funcObject);
		callbackListMultiThreading.append(std::bind(&FunctionObject::virFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		callbackListMultiThreading.append(std::bind(&FunctionObject::nonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		callbackListMultiThreading.append(std::bind(&FunctionObject::nonInlineVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		callbackListMultiThreading.append(std::bind(&FunctionObject::nonInlineNonVirFunc, &funcObject, std::placeholders::_1, std::placeholders::_2));
		const uint64_t callbackListMultiThreadingTime = measureElapsedTime([iterateCount, &callbackListMultiThreading]() {
			for(int i = 0; i < iterateCount; ++i) {
				callbackListMultiThreading(i, i);
			}
		});

		std::cout << "All: " << cppTime << " " << callbackListSingleThreadingTime << " " << callbackListMultiThreadingTime << std::endl;
	}
}

