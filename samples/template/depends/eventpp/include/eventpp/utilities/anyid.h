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

#ifndef ANYID_H_320827729782
#define ANYID_H_320827729782

#include <functional>
#include <type_traits>

namespace eventpp {

namespace anyid_internal_ {

template <typename T, typename Enabled = void>
struct MakeHash;

template <typename T>
struct MakeHash <T, typename std::enable_if<std::is_convertible<T, std::size_t>::value>::type>
{
	std::size_t operator() (T value) const {
		return static_cast<std::size_t>(value);
	}
};

template <typename T>
struct MakeHash <T, typename std::enable_if<! std::is_convertible<T, std::size_t>::value>::type>
{
	std::size_t operator() (const T & value) const {
		return std::hash<T>()(value);
	}
};

template <typename T>
class HasLess
{
	template <typename C> static std::true_type test(decltype(std::declval<C>() < std::declval<C>()) *);
	template <typename C> static std::false_type test(...);

public:
	enum { value = !! decltype(test<T>(0))() };
};

template <typename T>
auto compareLessThan(const T & a, const T & b)
	-> typename std::enable_if<HasLess<T>::value, bool>::type
{
	return a < b;
}

template <typename T>
auto compareLessThan(const T &, const T &)
	-> typename std::enable_if<! HasLess<T>::value, bool>::type
{
	return false;
}

template <typename T>
class HasEqual
{
	template <typename C> static std::true_type test(decltype(std::declval<C>() == std::declval<C>()) *);
	template <typename C> static std::false_type test(...);

public:
	enum { value = !! decltype(test<T>(0))() };
};

template <typename T>
auto compareEqual(const T & a, const T & b)
	-> typename std::enable_if<HasEqual<T>::value, bool>::type
{
	return a == b;
}

template <typename T>
auto compareEqual(const T &, const T &)
	-> typename std::enable_if<! HasEqual<T>::value, bool>::type
{
	return true;
}


} //namespace anyid_internal_

struct EmptyAnyStorage
{
	EmptyAnyStorage() {}

	template <typename T>
	EmptyAnyStorage(const T &) {}
};

template <template <typename> class Digester = std::hash, typename Storage = EmptyAnyStorage>
class AnyId
{
public:
	using DigestType = decltype(std::declval<Digester<int>>()(0));

public:
	AnyId() : digest(), value() {
	}

	template <typename T>
	AnyId(const T & value)
		: digest(Digester<T>()(value)), value(value)
	{
	}

	DigestType getDigest() const {
		return digest;
	}

	const Storage & getValue() const {
		return value;
	}

private:
	DigestType digest;
	Storage value;
};

template <template <typename> class Digester, typename Storage>
bool operator == (const AnyId<Digester, Storage> & a, const AnyId<Digester, Storage> & b)
{
	return a.getDigest() == b.getDigest() && anyid_internal_::compareEqual(a.getValue(), b.getValue());
}

template <template <typename> class Digester, typename Storage>
bool operator < (const AnyId<Digester, Storage> & a, const AnyId<Digester, Storage> & b)
{
	return (a.getDigest() < b.getDigest())
		|| (anyid_internal_::compareLessThan(a.getValue(), b.getValue()) && a.getDigest() == b.getDigest())
	;
}

using AnyHashableId = AnyId<>;

} //namespace eventpp

namespace std
{
template <template <typename> class Digester, typename Storage>
struct hash<eventpp::AnyId<Digester, Storage> >
{
	std::size_t operator()(const eventpp::AnyId<Digester, Storage> & value) const noexcept
	{
		return eventpp::anyid_internal_::MakeHash<typename eventpp::AnyId<Digester, Storage>::DigestType>()(value.getDigest());
	}
};
} //namespace std

#endif

