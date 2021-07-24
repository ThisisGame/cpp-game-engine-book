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

#ifndef TEST_CALLBACKLIST_UTIL_H
#define TEST_CALLBACKLIST_UTIL_H

#include "test.h"

#define private public
#include "eventpp/callbacklist.h"
#undef private

#include <vector>
#include <algorithm>
#include <numeric>

template <typename CL, typename T>
void verifyLinkedList(CL & callbackList, const std::vector<T> & dataList)
{
	const int count = (int)dataList.size();
	if(count == 0) {
		REQUIRE(! callbackList.head);
		REQUIRE(! callbackList.tail);
		return;
	}

	REQUIRE(! callbackList.head->previous);
	REQUIRE(! callbackList.tail->next);

	if(count == 1) {
		REQUIRE(callbackList.head);
		REQUIRE(callbackList.head == callbackList.tail);
	}

	auto node = callbackList.head;
	for(int i = 0; i < count; ++i) {
		REQUIRE(node);
		
		if(i == 0) {
			REQUIRE(! node->previous);
			REQUIRE(node == callbackList.head);
		}
		if(i == count - 1) {
			REQUIRE(! node->next);
			REQUIRE(node == callbackList.tail);
		}
		
		REQUIRE(node->callback == dataList[i]);

		node = node->next;
	}
}

template <typename CL, typename T>
void verifyDisorderedLinkedList(CL & callbackList, std::vector<T> dataList)
{
	std::vector<T> buffer;

	auto node = callbackList.head;
	while(node) {
		buffer.push_back(node->callback);
		node = node->next;
	}

	std::sort(buffer.begin(), buffer.end());
	std::sort(dataList.begin(), dataList.end());

	REQUIRE(buffer == dataList);
}

template <typename CL>
auto extractCallbackListHandles(CL & callbackList)
	-> std::vector<typename CL::Handle>
{
	std::vector<typename CL::Handle> result;

	auto node = callbackList.head;
	while(node) {
		result.push_back(typename CL::Handle(node));
		node = node->next;
	}

	return result;
}

struct RemovalTester
{
	RemovalTester(
			const int callbackCount,
			const int removerIndex,
			const std::vector<int> & indexesToBeRemoved
		)
		:
			callbackCount(callbackCount),
			removerIndex(removerIndex),
			indexesToBeRemoved(indexesToBeRemoved)
	{
	}

	void test()
	{
		using CL = eventpp::CallbackList<void()>;
		CL callbackList;
		std::vector<CL::Handle> handleList(callbackCount);
		std::vector<int> dataList(callbackCount);

		for(int i = 0; i < callbackCount; ++i) {
			if(i == removerIndex) {
				handleList[i] = callbackList.append([this, &dataList, &handleList, &callbackList, i]() {
					dataList[i] = i + 1;
					
					for(auto index : indexesToBeRemoved) {
						callbackList.remove(handleList[index]);
					}
				});
			}
			else {
				handleList[i] = callbackList.append([&dataList, i]() {
					dataList[i] = i + 1;
				});
			}
		}

		callbackList();

		std::vector<int> compareList(callbackCount);
		std::iota(compareList.begin(), compareList.end(), 1);

		for (auto index : indexesToBeRemoved) {
			if(index > removerIndex) {
				compareList[index] = 0;
			}
		}

		REQUIRE(dataList == compareList);
	}

	const int callbackCount;
	const int removerIndex;
	const std::vector<int> indexesToBeRemoved;
};

struct FakeCallbackListPolicies
{
	using Callback = int;
};


#endif
