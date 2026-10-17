/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_DEQUE_HPP
#define	MODM_DEQUE_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace modm
{

/**
 * Double ended queue with a fixed capacity.
 *
 * Internally organized as a ring buffer without heap allocation. The interface
 * follows `std::deque`, so it can be used as the container of `std::queue` and
 * `std::stack`:
 *
 * ```cpp
 * std::queue<uint8_t, modm::BoundedDeque<uint8_t, 10>> queue;
 * std::stack<uint8_t, modm::BoundedDeque<uint8_t, 10>> stack;
 * ```
 *
 * The elements are only constructed when they are added to the container, so
 * `T` does not have to be default constructible. The container can only be
 * used in constant expressions if `T` is trivially default constructible.
 *
 * Up to a capacity of 254 elements 8-bit index variables are used, otherwise
 * 16-bit.
 *
 * @warning	Adding elements to a full container or removing elements from an
 *			empty container is undefined behavior. Use `full()` and `empty()`
 *			to check before or use `append()` and `prepend()`.
 *
 * @tparam	T	Type of the elements
 * @tparam	N	Capacity of the container
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	modm_container
 */
template<typename T, std::size_t N>
class BoundedDeque
{
	static_assert(N > 0, "size = 0 is not allowed");

	template<bool Const>
	class Iterator;

public:
	using Index = std::conditional_t< (N >= 255), uint_fast16_t, uint_fast8_t >;
	using Size = Index;

	using value_type = T;
	using size_type = Index;
	using difference_type = std::ptrdiff_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using iterator = Iterator<false>;
	using const_iterator = Iterator<true>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
	constexpr BoundedDeque()
	{
		// Activate the union member for constant evaluation, not required at runtime
		if consteval {
			if constexpr (std::is_trivially_default_constructible_v<T>) std::construct_at(&buffer);
		}
	}

	constexpr BoundedDeque(const BoundedDeque&)
	requires std::is_trivially_copy_constructible_v<T> = default;

	constexpr BoundedDeque(const BoundedDeque& other)
	{
		for (const auto& value : other) push_back(value);
	}

	constexpr BoundedDeque(BoundedDeque&&)
	requires std::is_trivially_move_constructible_v<T> = default;

	constexpr BoundedDeque(BoundedDeque&& other)
		noexcept(std::is_nothrow_move_constructible_v<T>)
	{
		for (auto& value : other) push_back(std::move(value));
		other.clear();
	}

	constexpr BoundedDeque&
	operator=(const BoundedDeque&)
	requires std::is_trivially_copyable_v<T> = default;

	constexpr BoundedDeque&
	operator=(const BoundedDeque& other)
	{
		if (this != &other) {
			clear();
			for (const auto& value : other) push_back(value);
		}
		return *this;
	}

	constexpr BoundedDeque&
	operator=(BoundedDeque&&)
	requires std::is_trivially_copyable_v<T> = default;

	constexpr BoundedDeque&
	operator=(BoundedDeque&& other)
		noexcept(std::is_nothrow_move_constructible_v<T>)
	{
		if (this != &other) {
			clear();
			for (auto& value : other) push_back(std::move(value));
			other.clear();
		}
		return *this;
	}

	constexpr ~BoundedDeque()
	requires std::is_trivially_destructible_v<T> = default;

	constexpr ~BoundedDeque() { clear(); }

	// Capacity ---------------------------------------------------------------
	[[nodiscard]] constexpr bool
	empty() const { return count == 0; }

	constexpr bool
	full() const { return count == N; }

	constexpr size_type
	size() const { return count; }

	static constexpr size_type
	max_size() { return N; }

	static constexpr size_type
	capacity() { return N; }

	// Element access ---------------------------------------------------------
	/// @warning	Undefined behavior if `n >= size()`
	constexpr reference
	operator[](size_type n) { return buffer.data[wrap(start + n)]; }

	/// @warning	Undefined behavior if `n >= size()`
	constexpr const_reference
	operator[](size_type n) const { return buffer.data[wrap(start + n)]; }

	/// @warning	Undefined behavior if the container is empty
	constexpr reference
	front() { return buffer.data[start]; }

	/// @warning	Undefined behavior if the container is empty
	constexpr const_reference
	front() const { return buffer.data[start]; }

	/// @warning	Undefined behavior if the container is empty
	constexpr reference
	back() { return (*this)[count - 1]; }

	/// @warning	Undefined behavior if the container is empty
	constexpr const_reference
	back() const { return (*this)[count - 1]; }

	// Modifiers --------------------------------------------------------------
	/// @warning	Undefined behavior if the container is full
	template< class... Args >
	constexpr reference
	emplace_back(Args&&... args)
	{
		T* value = std::construct_at(&buffer.data[wrap(start + count)], std::forward<Args>(args)...);
		count++;
		return *value;
	}

	/// @warning	Undefined behavior if the container is full
	template< class... Args >
	constexpr reference
	emplace_front(Args&&... args)
	{
		const size_type index = (start == 0) ? N - 1 : start - 1;
		T* value = std::construct_at(&buffer.data[index], std::forward<Args>(args)...);
		start = index;
		count++;
		return *value;
	}

	/// @warning	Undefined behavior if the container is full
	constexpr void
	push_back(const T& value) { emplace_back(value); }

	/// @warning	Undefined behavior if the container is full
	constexpr void
	push_back(T&& value) { emplace_back(std::move(value)); }

	/// @warning	Undefined behavior if the container is full
	constexpr void
	push_front(const T& value) { emplace_front(value); }

	/// @warning	Undefined behavior if the container is full
	constexpr void
	push_front(T&& value) { emplace_front(std::move(value)); }

	/// @warning	Undefined behavior if the container is empty
	constexpr void
	pop_back()
	{
		count--;
		std::destroy_at(&buffer.data[wrap(start + count)]);
	}

	/// @warning	Undefined behavior if the container is empty
	constexpr void
	pop_front()
	{
		std::destroy_at(&buffer.data[start]);
		start = wrap(start + 1);
		count--;
	}

	constexpr void
	clear()
	{
		if constexpr (not std::is_trivially_destructible_v<T>) {
			while (not empty()) pop_back();
		}
		start = 0;
		count = 0;
	}

	/// Appends an item to the back if the container is not full.
	/// @return	`true` if the item was appended, `false` if the container is full.
	constexpr bool
	append(const T& value)
	{
		if (full()) return false;
		push_back(value);
		return true;
	}

	/// @copydoc append(const T&)
	constexpr bool
	append(T&& value)
	{
		if (full()) return false;
		push_back(std::move(value));
		return true;
	}

	/// Prepends an item to the front if the container is not full.
	/// @return	`true` if the item was prepended, `false` if the container is full.
	constexpr bool
	prepend(const T& value)
	{
		if (full()) return false;
		push_front(value);
		return true;
	}

	/// @copydoc prepend(const T&)
	constexpr bool
	prepend(T&& value)
	{
		if (full()) return false;
		push_front(std::move(value));
		return true;
	}

	/// Appends an item to the back and removes the front item if the container is full.
	constexpr void
	appendOverwrite(const T& value)
	{
		if (full()) pop_front();
		push_back(value);
	}

	/// Prepends an item to the front and removes the back item if the container is full.
	constexpr void
	prependOverwrite(const T& value)
	{
		if (full()) pop_back();
		push_front(value);
	}

	// Iterators --------------------------------------------------------------
	constexpr iterator begin() { return {this, 0}; }
	constexpr const_iterator begin() const { return {this, 0}; }
	constexpr const_iterator cbegin() const { return begin(); }
	constexpr iterator end() { return {this, count}; }
	constexpr const_iterator end() const { return {this, count}; }
	constexpr const_iterator cend() const { return end(); }

	constexpr reverse_iterator rbegin() { return reverse_iterator(end()); }
	constexpr const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	constexpr const_reverse_iterator crbegin() const { return rbegin(); }
	constexpr reverse_iterator rend() { return reverse_iterator(begin()); }
	constexpr const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	constexpr const_reverse_iterator crend() const { return rend(); }

	// Deprecated interface ---------------------------------------------------
	[[deprecated("Use empty() instead")]] // DEPRECATED: 2027q4
	constexpr bool isEmpty() const { return empty(); }
	[[deprecated("Use not empty() instead")]] // DEPRECATED: 2027q4
	constexpr bool isNotEmpty() const { return not empty(); }
	[[deprecated("Use full() instead")]] // DEPRECATED: 2027q4
	constexpr bool isFull() const { return full(); }
	[[deprecated("Use not full() instead")]] // DEPRECATED: 2027q4
	constexpr bool isNotFull() const { return not full(); }
	[[deprecated("Use size() instead")]] // DEPRECATED: 2027q4
	constexpr Size getSize() const { return size(); }
	[[deprecated("Use max_size() instead")]] // DEPRECATED: 2027q4
	constexpr Size getMaxSize() const { return max_size(); }
	[[deprecated("Use front() instead")]] // DEPRECATED: 2027q4
	constexpr T& getFront() { return front(); }
	[[deprecated("Use front() instead")]] // DEPRECATED: 2027q4
	constexpr const T& getFront() const { return front(); }
	[[deprecated("Use back() instead")]] // DEPRECATED: 2027q4
	constexpr T& getBack() { return back(); }
	[[deprecated("Use back() instead")]] // DEPRECATED: 2027q4
	constexpr const T& getBack() const { return back(); }
	[[deprecated("Use operator[] instead")]] // DEPRECATED: 2027q4
	constexpr T& get(Index n) { return (*this)[n]; }
	[[deprecated("Use operator[] instead")]] // DEPRECATED: 2027q4
	constexpr const T& get(Index n) const { return (*this)[n]; }
	[[deprecated("Use operator[](size() - 1 - n) instead")]] // DEPRECATED: 2027q4
	constexpr T& rget(Index n) { return (*this)[count - 1 - n]; }
	[[deprecated("Use operator[](size() - 1 - n) instead")]] // DEPRECATED: 2027q4
	constexpr const T& rget(Index n) const { return (*this)[count - 1 - n]; }
	[[deprecated("Use pop_back() instead")]] // DEPRECATED: 2027q4
	constexpr void removeBack() { pop_back(); }
	[[deprecated("Use pop_front() instead")]] // DEPRECATED: 2027q4
	constexpr void removeFront() { pop_front(); }

private:
	template<bool Const>
	class Iterator
	{
		friend class BoundedDeque;
		using Parent = std::conditional_t<Const, const BoundedDeque, BoundedDeque>;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using iterator_concept = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = std::conditional_t<Const, const T*, T*>;
		using reference = std::conditional_t<Const, const T&, T&>;

		constexpr Iterator() = default;
		template<bool OtherConst> requires (Const and not OtherConst)
		constexpr Iterator(const Iterator<OtherConst>& other)
		:	parent(other.parent), index(other.index) {}

		constexpr reference operator*() const { return (*parent)[index]; }
		constexpr pointer operator->() const { return &(*parent)[index]; }
		constexpr reference operator[](difference_type n) const { return (*parent)[index + n]; }

		constexpr Iterator& operator++() { index++; return *this; }
		constexpr Iterator operator++(int) { Iterator tmp{*this}; index++; return tmp; }
		constexpr Iterator& operator--() { index--; return *this; }
		constexpr Iterator operator--(int) { Iterator tmp{*this}; index--; return tmp; }
		constexpr Iterator& operator+=(difference_type n) { index += n; return *this; }
		constexpr Iterator& operator-=(difference_type n) { index -= n; return *this; }

		friend constexpr Iterator operator+(Iterator it, difference_type n) { return it += n; }
		friend constexpr Iterator operator+(difference_type n, Iterator it) { return it += n; }
		friend constexpr Iterator operator-(Iterator it, difference_type n) { return it -= n; }
		friend constexpr difference_type operator-(const Iterator& lhs, const Iterator& rhs)
		{ return difference_type(lhs.index) - difference_type(rhs.index); }

		friend constexpr bool operator==(const Iterator& lhs, const Iterator& rhs)
		{ return lhs.index == rhs.index; }
		friend constexpr auto operator<=>(const Iterator& lhs, const Iterator& rhs)
		{ return lhs.index <=> rhs.index; }

	private:
		constexpr Iterator(Parent* parent, size_type index) : parent(parent), index(index) {}

		Parent* parent{nullptr};
		size_type index{0};

		friend class Iterator<not Const>;
	};

	static constexpr size_type
	wrap(std::size_t index) { return (index >= N) ? index - N : index; }

	struct Buffer { T data[N]; };
	union { Buffer buffer; };
	size_type start{0};
	size_type count{0};
};

}	// namespace modm

#endif	// MODM_DEQUE_HPP
