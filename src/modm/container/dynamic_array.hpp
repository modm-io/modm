/*
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Martin Rosekeit
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DYNAMIC_ARRAY_HPP
#define MODM_DYNAMIC_ARRAY_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <vector>

namespace modm
{
	/**
	 * \brief	Dynamic Arrays
	 *
	 * Dynamic Arrays are a kind of sequence containers. As such, their
	 * elements are ordered following a strict linear sequence.
	 *
	 * Just as regular arrays, dynamic arrays containers have their elements
	 * stored in contiguous storage locations, which means that their elements
	 * can be accessed not only using iterators but also using offsets on
	 * regular pointers to elements.
	 *
	 * Reallocations may be a costly operation in terms of performance, since
	 * they generally involve the entire storage space used by the
	 * dynamic array to be copied to a new location. Therefore, whenever large
	 * increases in size are planned for a dynamic array, it is recommended to
	 * explicitly indicate a capacity for the dynamic array using member
	 * function DynamicArray::reserve().
	 *
	 * \author	Fabian Greif <fabian.greif@rwth-aachen.de>
	 * \ingroup	modm_container
	 */
	template <typename T, typename Allocator = std::allocator<T>>
	class [[deprecated("Use std::vector instead")]] DynamicArray
	{
	public:
		using SizeType = std::size_t;
		using const_iterator = std::vector<T, Allocator>::const_iterator;
		using iterator = std::vector<T, Allocator>::iterator;

	public:
		/**
		 * \brief	Default constructor
		 *
		 * Constructs an empty dynamic array, with no content and a
		 * size of zero.
		 */
		DynamicArray(const Allocator& allocator = Allocator())
			: data_{std::move(allocator)}
		{}

		/**
		 * \brief	Allocation constructor
		 *
		 * Construct a dynamic array of given capacity. The array will still
		 * be empty.
		 */
		DynamicArray(SizeType n, const Allocator& allocator = Allocator())
			: data_{std::move(allocator)}
		{
			data_.reserve(n);
		}

		/**
		 * \brief	Repetitive sequence constructor
		 *
		 * Construct a dynamic array of given size.
		 *
		 * Initializes the dynamic array with its content set to a
		 * repetition, n times, of copies of value.
		 */
		DynamicArray(SizeType n, const T& value, const Allocator& allocator = Allocator())
			: data_{n, value, std::move(allocator)}
		{}

		/**
		 * \brief	Initializer List constructor
		 *
		 * Construct a dynamic array that holds the values specified in the
		 * initialize list
		 */
		DynamicArray(std::initializer_list<T> init, const Allocator& allocator = Allocator())
			: data_{init, std::move(allocator)}
		{}

		/**
		 * \brief	Test whether dynamic array is empty
		 *
		 * Returns whether the dynamic array container is empty, i.e. whether
		 * its size is 0.
		 */
		bool
		isEmpty() const
		{
			return data_.empty();
		}

		/**
		 * \brief	Return size
		 *
		 * Returns the number of elements in the container.
		 */
		SizeType
		getSize() const
		{
			return data_.size();
		}

		/**
		 * \brief	Return size of allocated storage
		 *
		 * Returns the size of the allocated storage space in the dynamic array
		 * object.
		 *
		 * Notice that, in dynamic arrays, the capacity is not necessarily
		 * equal to the number of elements that conform the underlying
		 * dynamic array content (this can be obtained with member
		 * DynamicArray::getSize()), but the capacity of the actual allocated
		 * space, which is either equal or greater than the content size.
		 *
		 * \see	getSize()
		 */
		SizeType
		getCapacity() const
		{
			return data_.capacity();
		}

		/**
		 * \brief	Request a change in capacity
		 *
		 * Requests that the capacity of the allocated storage space for the
		 * elements of the dynamic array container be at least enough to
		 * hold n more elements.
		 *
		 * This informs the dynamic array of a planned increase in size,
		 * although notice that the parameter n informs of a minimum, so the
		 * resulting capacity may be any capacity equal or larger than this.
		 *
		 * \see	getCapacity()
		 */
		void
		reserve(SizeType n)
		{
			data_.reserve(n);
		}

		/**
		 * \brief	Remove all elements and set capacity to zero
		 *
		 * Frees all allocated memory and sets the capacity of the container
		 * to zero.
		 *
		 * \warning	This will discard all the items in the container
		 */
		void
		clear()
		{
			data_.clear();
			data_.shrink_to_fit();
		}

		/**
		 * \brief	Remove all elements
		 *
		 * Keeps the capacity at its current level.
		 */
		void
		removeAll()
		{
			data_.clear();
		}

		/**
		 * \brief	Access element
		 *
		 * Returns a reference to the element at position n in the
		 * dynamic array container.
		 *
		 * \warning Do not use this operator to access an element the
		 * first time. Use the append methode to create it.
		 *
		 * \see	append()
		 */
		inline T&
		operator [](SizeType index)
		{
			return data_[index];
		}

		/**
		 * \brief	Access element
		 *
		 * Returns a reference to the element at position n in the
		 * dynamic array container.
		 */
		inline const T&
		operator [](SizeType index) const
		{
			return data_[index];
		}

		/**
		 * \brief	Add element at the end
		 *
		 * Adds a new element at the end of the dynamic array, after its
		 * current last element. The content of this new element is
		 * initialized to a copy of \p value.
		 *
		 * This effectively increases the dynamic array size by one, which
		 * causes a reallocation of the internal allocated storage if the
		 * dynamic array size was equal to the dynamic array capacity before
		 * the call. Reallocations invalidate all previously obtained
		 * iterators, references and pointers.
		 */
		void
		append(const T& value)
		{
			data_.push_back(value);
		}

		/**
		 * \brief	Delete last element
		 *
		 * Removes the last element in the dynamic array, effectively reducing
		 * the dynamic array size by one and invalidating all iterators and
		 * references to it.
		 *
		 * This calls the removed element's destructor.
		 */
		void
		removeBack()
		{
			data_.pop_back();
		}

		// TODO insert implementation
		//void
		//insert(...);

		inline const T&
		getFront() const
		{
			return data_.front();
		}

		inline T&
		getFront()
		{
			return data_.front();
		}

		inline const T&
		getBack() const
		{
			return data_.back();
		}

		inline T&
		getBack()
		{
			return data_.back();
		}

		/**
		 * Returns a read/write iterator that points to the first element in the
		 * list.  Iteration is done in ordinary element order.
		 */
		iterator
		begin()
		{
			return data_.begin();
		}

		/**
		 * Returns a read-only (constant) iterator that points to the
		 * first element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		begin() const
		{
			return data_.begin();
		}

		/**
		 * Returns a read/write iterator that points one past the last
		 * element in the list. Iteration is done in ordinary element
		 * order.
		 */
		iterator
		end()
		{
			return data_.end();
		}

		/**
		 * Returns a read-only (constant) iterator that points one past
		 * the last element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		end() const
		{
			return data_.end();
		}

		/**
		 * Returns a read/write iterator that points to the first element
		 * that contains value. If value is not found, it points to the
		 * last element.
		 */
		iterator
		find(const T& value)
		{
			return std::find(data_.begin(), data_.end(), value);
		}

		/**
		 * Returns a read-only (constant) iterator that points to the first
		 * element that contains value. If value is not found, it points to
		 * the last element.
		 */
		const_iterator
		find(const T& value) const
		{
			return std::find(data_.begin(), data_.end(), value);
		}

	private:
		std::vector<T, Allocator> data_;
	};
}

#endif // MODM_DYNAMIC_ARRAY_HPP
