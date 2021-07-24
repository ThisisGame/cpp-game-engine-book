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
#include "eventpp/utilities/conditionalfunctor.h"
#include "eventpp/callbacklist.h"

#include <vector>

TEST_CASE("ConditionalFunctor, lambda")
{
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
}

void conditionalFunctorIncreaseOne(std::vector<int> & dataList, const int index)
{
	++dataList[index];
}

TEST_CASE("ConditionalFunctor, free function")
{
	eventpp::CallbackList<void(std::vector<int> &, int)> callbackList;

	std::vector<int> dataList(3);

	callbackList.append(
		eventpp::conditionalFunctor(&conditionalFunctorIncreaseOne,
			[](std::vector<int> &, const int index) {
				return index == 0;
			}
		)
	);
	callbackList.append(
		eventpp::conditionalFunctor(&conditionalFunctorIncreaseOne,
			[](std::vector<int> &, const int index) {
				return index == 1;
			}
		)
	);
	callbackList.append(
		eventpp::conditionalFunctor(&conditionalFunctorIncreaseOne,
			[](std::vector<int> &, const int index) {
				return index == 2;
			}
		)
	);

	REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });

	callbackList(dataList, 2);
	REQUIRE(dataList == std::vector<int>{ 0, 0, 1 });

	callbackList(dataList, 0);
	REQUIRE(dataList == std::vector<int>{ 1, 0, 1 });

	callbackList(dataList, 1);
	REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
}

