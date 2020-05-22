/*
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Martin Rosekeit
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Kevin Läufer
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

#include <cstddef>
#include <modm/utils/allocator.hpp>
#include <initializer_list>
#include <iterator>

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
	template <typename T, typename Allocator = allocator::Dynamic<T> >
	class DynamicArray
	{
	public:
		typedef std::size_t SizeType;
	public:
		/**
		 * \brief	Default constructor
		 *
		 * Constructs an empty dynamic array, with no content and a
		 * size of zero.
		 */
		DynamicArray(const Allocator& allocator = Allocator());

		/**
		 * \brief	Allocation constructor
		 *
		 * Construct a dynamic array of given capacity. The array will still
		 * be empty.
		 */
		DynamicArray(SizeType n, const Allocator& allocator = Allocator());

		/**
		 * \brief	Repetitive sequence constructor
		 *
		 * Construct a dynamic array of given size.
		 *
		 * Initializes the dynamic array with its content set to a
		 * repetition, n times, of copies of value.
		 */
		DynamicArray(SizeType n, const T& value, const Allocator& allocator = Allocator());

		/**
		 * \brief	Initializer List constructor
		 *
		 * Construct a dynamic array that holds the values specified in the
		 * initialize list
		 */
		DynamicArray(std::initializer_list<T> init,
			const Allocator& allocator = Allocator());

		DynamicArray(const DynamicArray& other);

		~DynamicArray();

		DynamicArray&
		operator = (const DynamicArray& other);

		/**
		 * \brief	Test whether dynamic array is empty
		 *
		 * Returns whether the dynamic array container is empty, i.e. whether
		 * its size is 0.
		 */
		inline bool
		isEmpty() const
		{
			return (this->size == 0);
		}

		/**
		 * \brief	Return size
		 *
		 * Returns the number of elements in the container.
		 */
		inline SizeType
		getSize() const
		{
			return this->size;
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
		inline SizeType
		getCapacity() const
		{
			return this->capacity;
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
		reserve(SizeType n);

		/**
		 * \brief	Remove all elements and set capacity to zero
		 *
		 * Frees all allocated memory and sets the capacity of the container
		 * to zero.
		 *
		 * \warning	This will discard all the items in the container
		 */
		void
		clear();

		/**
		 * \brief	Remove all elements
		 *
		 * Keeps the capacity at its current level.
		 */
		void
		removeAll();

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
			return this->values[index];
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
			return this->values[index];
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
		append(const T& value);

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
		removeBack();

		// TODO insert implementation
		//void
		//insert(...);

		inline const T&
		getFront() const
		{
			return this->values[0];
		}

		inline T&
		getFront()
		{
			return this->values[0];
		}

		inline const T&
		getBack() const
		{
			return this->values[this->size - 1];
		}

		inline T&
		getBack()
		{
			return this->values[this->size - 1];
		}

	public:
		/**
		 * \brief	Forward iterator
		 */
		class iterator : public std::iterator<std::forward_iterator_tag, T>
		{
			friend class DynamicArray;
			friend class const_iterator;

		public:
			/// Default constructor
			iterator();
			iterator(const iterator& other);

			iterator& operator = (const iterator& other);
			iterator& operator ++ ();
			iterator& operator -- ();
			bool operator == (const iterator& other) const;
			bool operator != (const iterator& other) const;
			bool operator <  (const iterator& other) const;
			bool operator >  (const iterator& other) const;
			T& operator * ();
			T* operator -> ();

		private:
			iterator(DynamicArray* parent, SizeType index);

			DynamicArray *parent;
			SizeType index;
		};

		/**
		 * \brief	forward const iterator
		 */
		class const_iterator : public std::iterator<std::forward_iterator_tag, T>
		{
			friend class DynamicArray;

		public:
			/// Default constructor
			const_iterator();

			/**
			 * \brief	Copy constructor
			 *
			 * Used to convert a normal iterator to a const iterator.
			 * The other way is not possible.
			 */
			const_iterator(const iterator& other);

			/**
			 * \brief	Copy constructor
			 */
			const_iterator(const const_iterator& other);

			const_iterator& operator = (const const_iterator& other);
			const_iterator& operator ++ ();
			const_iterator& operator -- ();
			bool operator == (const const_iterator& other) const;
			bool operator != (const const_iterator& other) const;
			const T& operator * () const;
			const T* operator -> () const;

		private:
			const_iterator(const DynamicArray* parent, SizeType index);

			const DynamicArray *parent;
			SizeType index;
		};

		/**
		 * Returns a read/write iterator that points to the first element in the
		 * list.  Iteration is done in ordinary element order.
		 */
		iterator
		begin();

		/**
		 * Returns a read-only (constant) iterator that points to the
		 * first element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		begin() const;

		/**
		 * Returns a read/write iterator that points one past the last
		 * element in the list. Iteration is done in ordinary element
		 * order.
		 */
		iterator
		end();

		/**
		 * Returns a read-only (constant) iterator that points one past
		 * the last element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		end() const;

		/**
		 * Returns a read/write iterator that points to the first element
		 * that contains value. If value is not found, it points to the
		 * last element.
		 */
		iterator
		find(const T& value);

		/**
		 * Returns a read-only (constant) iterator that points to the first
		 * element that contains value. If value is not found, it points to
		 * the last element.
		 */
		const_iterator
		find(const T& value) const;

	private:
		friend class const_iterator;
		friend class iterator;

	private:
		/*
		 * Allocate a new buffer of size n and copy the elements from the
		 * old buffer to the new buffer.
		 */
		void
		relocate(SizeType n);

		Allocator allocator;

		SizeType size;
		SizeType capacity;
		T* values;
	};
}

#include "dynamic_array_impl.hpp"
#include "dynamic_array_iterator_impl.hpp"

#endif // MODM_DYNAMIC_ARRAY_HPP
