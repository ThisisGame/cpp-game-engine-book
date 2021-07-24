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

#include <map>
#include <unordered_map>
#include <random>
#include <string>

namespace {

int getRandomeInt()
{
	static std::random_device rd;
	static std::mt19937 engine(rd());
	static std::uniform_int_distribution<> dist;
	return dist(engine);
}

int getRandomeInt(const int max)
{
	return getRandomeInt() % max;
}

int getRandomeInt(const int min, const int max)
{
	if(min >= max) {
		return min;
	}
	return min + getRandomeInt() % (max - min);
}

std::string generateRandomString(const int length){
	static std::string possibleCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string result(length, 0);
	for(int i = 0; i < length; i++){
		result[i] = possibleCharacters[getRandomeInt((int)possibleCharacters.size())];
	}
	return result;
}

} //unnamed namespace

TEST_CASE("b2, std::map vs std::unordered_map")
{
	constexpr int stringCount = 1000 * 1000;
	std::vector<std::string> stringList(stringCount);
	for(auto & s : stringList) {
		s = generateRandomString(getRandomeInt(3, 10));
	}

	constexpr int iterateCount = 1000 * 1000 * 1;

	uint64_t mapInsertTime = 0;
	uint64_t mapLookupTime = 0;
	{
		std::map<std::string, int> map;
		mapInsertTime = measureElapsedTime([iterateCount, stringCount, &map, &stringList]() {
			for(int i = 0; i < iterateCount; ++i) {
				map[stringList[i % stringCount]] = i;
			}
		});
		
		mapLookupTime = measureElapsedTime([iterateCount, stringCount, &map, &stringList]() {
			for(int i = iterateCount - 1; i >= 0; --i) {
				if(map.find(stringList[i % stringCount]) == map.end()) {
					stringList[i] = stringList[i];
				}
			}
		});
	}

	uint64_t unorderedMapInsertTime = 0;
	uint64_t unorderedMapLookupTime = 0;
	{
		std::unordered_map<size_t, int> map;
		unorderedMapInsertTime = measureElapsedTime([iterateCount, stringCount, &map, &stringList]() {
			for(int i = 0; i < iterateCount; ++i) {
				map[std::hash<std::string>()(stringList[i % stringCount])] = i;
			}
		});

		unorderedMapLookupTime = measureElapsedTime([iterateCount, stringCount, &map, &stringList]() {
			for(int i = stringCount - 1; i >= 0; --i) {
				if(map.find(std::hash<std::string>()(stringList[i])) == map.end()) {
					stringList[i] = stringList[i];
				}
			}
		});
	}
	std::cout << mapInsertTime << " " << mapLookupTime << std::endl;
	std::cout << unorderedMapInsertTime << " " << unorderedMapLookupTime << std::endl;
}

