#ifndef EVENTQUEUE_I_H
#define EVENTQUEUE_I_H

#include <array>
#include <cassert>

namespace eventpp {

namespace internal_ {

template <size_t ...Indexes>
struct IndexSequence
{
};

template <size_t N, size_t ...Indexes>
struct MakeIndexSequence : MakeIndexSequence <N - 1, N - 1, Indexes...>
{
};

template <std::size_t ...Indexes>
struct MakeIndexSequence<0, Indexes...>
{
	using Type = IndexSequence<Indexes...>;
};

template <typename T>
struct CounterGuard
{
	explicit CounterGuard(T & v) : value(v) {
		++value;
	}

	~CounterGuard() {
		--value;
	}

	T & value;
};

using DtorFunc = void (*)(void *);

template <typename T>
void commonDtor(void * instance)
{
	reinterpret_cast<T *>(instance)->~T();
}

// used by EventQueue
template <typename T>
class BufferedItem
{
public:
	using ValueType = T;

public:
	explicit BufferedItem() : buffer(), dtor(nullptr)
	{
	}

	~BufferedItem()
	{
		if(dtor != nullptr) {
			clear();
		}
	}

	BufferedItem(BufferedItem &&) = delete;
	BufferedItem(const BufferedItem &) = delete;
	BufferedItem & operator = (const BufferedItem &) = delete;

	void set(T && item) {
		assert(dtor == nullptr);

		new (buffer.data()) T(std::forward<T>(item));
		dtor = &commonDtor<T>;
	}

	T & get() {
		assert(dtor != nullptr);

		return *reinterpret_cast<T *>(buffer.data());
	}

	const T & get() const {
		assert(dtor != nullptr);

		return *reinterpret_cast<const T *>(buffer.data());
	}

	void clear() {
		assert(dtor != nullptr);

		dtor(buffer.data());
		dtor = nullptr;
	}

	bool empty() const {
		return dtor == nullptr;
	}

private:
	std::array<char, sizeof(T)> buffer;
	DtorFunc dtor;
};


// used by HeterEventQueue
template <size_t Size>
class BufferedUnion
{
public:
	explicit BufferedUnion() : buffer(), dtor(nullptr)
	{
	}

	~BufferedUnion()
	{
		if(dtor != nullptr) {
			clear();
		}
	}

	BufferedUnion(BufferedUnion &&) = delete;
	BufferedUnion(const BufferedUnion &) = delete;
	BufferedUnion & operator = (const BufferedUnion &) = delete;

	template <typename U>
	void set(U && item) {
		static_assert(sizeof(U) <= Size, "Item is too large to fit in BufferedUnion");
		assert(dtor == nullptr);

		new (buffer.data()) U(std::forward<U>(item));
		dtor = &commonDtor<U>;
	}

	template <typename U>
	U & get() {
		assert(dtor != nullptr);

		return *reinterpret_cast<U *>(buffer.data());
	}

	template <typename U>
	const U & get() const {
		assert(dtor != nullptr);

		return *reinterpret_cast<const U *>(buffer.data());
	}

	void clear() {
		assert(dtor != nullptr);

		dtor(buffer.data());
		dtor = nullptr;
	}

	bool empty() const {
		return dtor == nullptr;
	}

private:
	std::array<char, Size> buffer;
	DtorFunc dtor;
};


} //namespace internal_

} //namespace eventpp


#endif

