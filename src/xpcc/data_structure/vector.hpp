// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__VECTOR_HPP
#define XPCC__VECTOR_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	Vector
	 *
	 * Vectors are a kind of sequence containers. As such, their elements are
	 * ordered following a strict linear sequence.
	 *
	 * Vector containers are implemented as dynamic arrays; Just as regular
	 * arrays, vector containers have their elements stored in contiguous
	 * storage locations, which means that their elements can be accessed not
	 * only using iterators but also using offsets on regular pointers to
	 * elements.
	 *
	 * Reallocations may be a costly operation in terms of performance, since
	 * they generally involve the entire storage space used by the vector to
	 * be copied to a new location. Therefore, whenever large increases in
	 * size are planned for a vector, it is recommended to explicitly indicate
	 * a capacity for the vector using member function Vector::reserve().
	 *
	 * \author	Fabian Greif <fabian.greif@rwth-aachen.de>
	 * \ingroup	data_structure
	 */
	template <typename T>
	class Vector
	{
	public:
		typedef int_fast16_t SizeType;
	public:
		/**
		 * \brief	Default constructor
		 *
		 * Constructs an empty vector, with no content and a size of zero.
		 */
		Vector();

		/**
		 * \brief	Allocation Constructor
		 *
		 * Construct a vector of given size.
		 */
		Vector(SizeType n);

		/**
		 * \brief	Repetitive sequence constructor
		 *
		 * Construct a vector of given size.
		 *
		 * Initializes the vector with its content set to a repetition,
		 * n times, of copies of value.
		 */
		Vector(SizeType n, const T& value);

		/**
		 * \brief	Test whether vector is empty
		 *
		 * Returns whether the vector container is empty, i.e. whether its
		 * size is 0.
		 */
		inline bool
		isEmpty() const
		{
			return (this->size == 0);
		}

		/**
		 * \brief	Return size
		 *
		 * Returns the number of elements in the vector container.
		 */
		inline SizeType
		getSize() const
		{
			return this->size;
		}

		/**
		 * \brief	Return size of allocated storage
		 *
		 * Returns the size of the allocated storage space in the vector
		 * object.
		 *
		 * Notice that, in vectors, the capacity is not necessarily equal to
		 * the number of elements that conform the underlying vector content
		 * (this can be obtained with member Vector::getSize()), but the
		 * capacity of the actual allocated space, which is either equal or
		 * greater than the content size.
		 *
		 * \see	getSize()
		 */
		inline SizeType
		getCapacity() const
		{
			return (this->size + this->remainingCapacity);
		}

		/**
		 * \brief	Request a change in capacity
		 *
		 * Requests that the capacity of the allocated storage space for the
		 * elements of the vector container be at least enough to hold n more
		 * elements.
		 *
		 * This informs the vector of a planned increase in size, although
		 * notice that the parameter n informs of a minimum, so the resulting
		 * capacity may be any capacity equal or larger than this.
		 *
		 * \see	getCapacity()
		 */
		void
		reserve(SizeType n);

		/**
		 * \brief	Access element
		 *
		 * Returns a reference to the element at position n in the vector
		 * container.
		 */
		inline T&
		operator [](SizeType index);

		/**
		 * \brief	Access element
		 *
		 * Returns a reference to the element at position n in the vector
		 * container.
		 */
		inline const T&
		operator [](SizeType index) const;

		/**
		 * \brief	Add element at the end
		 *
		 * Adds a new element at the end of the vector, after its current last
		 * element. The content of this new element is initialized to a
		 * copy of x.
		 *
		 * This effectively increases the vector size by one, which causes a
		 * reallocation of the internal allocated storage if the vector size
		 * was equal to the vector capacity before the call. Reallocations
		 * invalidate all previously obtained iterators, references and
		 * pointers.
		 */
		void
		append(const T& value);

		/**
		 * \brief	Delete last element
		 *
		 * Removes the last element in the vector, effectively reducing the
		 * vector size by one and invalidating all iterators and references
		 * to it.
		 *
		 * This calls the removed element's destructor.
		 */
		void
		removeLast();

		// TODO insert, first, last

	private:
		/*
		 * Allocate a new buffer of size n and copy the elements from the
		 * old buffer to the new buffer
		 */
		void
		relocate(SizeType n);

		SizeType size;
		SizeType remainingCapacity;
		T* buffer;
	};
}

#include "vector_impl.hpp"

#endif // XPCC__VECTOR_HPP
