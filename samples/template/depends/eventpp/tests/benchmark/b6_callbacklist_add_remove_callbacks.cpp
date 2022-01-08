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

TEST_CASE("b6, CallbackList add/remove callbacks")
{
	using CL = eventpp::CallbackList<void ()>;
	constexpr size_t callbackCount = 1000;
	constexpr size_t iterateCount = 1000 * 100;
	CL callbackList;
	std::vector<CL::Handle> handleList(callbackCount);
	const uint64_t time = measureElapsedTime(
		[callbackCount, iterateCount, &callbackList, &handleList]() {
		for(size_t iterate = 0; iterate < iterateCount; ++iterate) {
			for(size_t i = 0; i < callbackCount; ++i) {
				handleList[i] = callbackList.append([]() {});
			}
			for(size_t i = 0; i < callbackCount; ++i) {
				callbackList.remove(handleList[i]);
			}
		}
	});

	std::cout
		<< "CallbackList add/remove callbacks,"
		<< " callbackCount: " << callbackCount
		<< " iterateCount: " << iterateCount
		<< " time: " << time
		<< std::endl;
}

