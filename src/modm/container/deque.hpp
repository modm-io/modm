/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
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
#include <type_traits>
#include <iterator>

namespace modm
{
	/**
	 * \brief	Double ended queue
	 *
	 * Internally organized as a ring buffer.
	 *
	 * \verbatim
	 *           tail --\                 /-- head
	 *                  |                 |
	 *        +------+------+---- ----+------+------+
	 *      0 |      | data |   ...   | data |      | N-1
	 *        +------+------+---- ----+------+------+
	 *           |      |                 |      |
	 * prepend --/      |                 |      \-- append
	 *                  |                 |
	 *       getFront --/                 \-- getBack
	 * \endverbatim
	 *
	 * \tparam	T	Type of the elements
	 * \tparam	N	Size of the queue
	 *
	 * Up to a size of 254 small index variables with 8-bits are used, after
	 * this they are switched to 16-bit.
	 *
	 * \warning	This class don't check if the container is empty before
	 * 			a pop-operation. You have to do this by yourself!
	 *
	 * \author		Fabian Greif
	 * \ingroup		modm_container
	 */
	template<typename T,
			 std::size_t N>
	class BoundedDeque
	{
	public:
		// select the type of the index variables with some template magic :-)
		using Index = std::conditional_t< (N >= 255),
										  uint_fast16_t,
										  uint_fast8_t >;

		using Size = Index;

	public:
		BoundedDeque();

		inline bool
		isEmpty() const;

		inline bool
		isNotEmpty() const { return not isEmpty(); };

		inline bool
		isFull() const;

		inline bool
		isNotFull() const { return not isFull(); };

		inline Size
		getSize() const;

		inline Size
		getMaxSize() const;

		/**
		 * \brief	Clear the container
		 *
		 * \warning	This will discard all the items in the container
		 */
		void
		clear();


		inline T&
		getFront();

		inline const T&
		getFront() const;

		/**
		 * \brief	Get item at specified index
		 *
		 * Returns a reference to the item at index `n`, counting 0-indexed
		 * from Front to Back, which is the same order in that the items were
		 * appended and the iterator addresses them.
		 *
		 * \warning Please make sure `n` is a valid index: 0 <= *n* < *size*.
		 * 			Other indexes will cause undefined behaviour.
		 */
		inline T&
		get(Index n);

		inline const T&
		get(Index n) const;

		/**
		 * \brief	Get item at specified index
		 *
		 * Returns a reference to the item at index `n`, counting 0-indexed
		 * from Front to Back.
		 *
		 * \warning Please make sure `n` is a valid index: 0 <= *n* < *size*.
		 * 			Other indexes will cause undefined behaviour.
		 */
		inline T&
		operator[](Index n);

		inline const T&
		operator[](Index n) const;

		/**
		 * \brief	Get item at specified index
		 *
		 * Returns a reference to the item at index `n`. The items are indexed
		 * in reverse (Back to Front), which is their order when they have been
		 * prepended.
		 *
		 * \warning Please make sure `n` is a valid index: 0 <= *n* < *size*.
		 * 			Other indexes will cause undefined behaviour.
		 */
		inline T&
		rget(Index n);

		inline const T&
		rget(Index n) const;

		inline T&
		getBack();

		inline const T&
		getBack() const;

		bool
		append(const T& value);

		/**
		 * \brief	Append an item to the back of the deque overwriting existing items
		 *
		 * This method, in contrast to `append()`, overwrites existing items in the deque
		 * if it is full. When an item is appended to the already full deque, the front
		 * item is removed to use its space for the new item, which is inserted after back.
		 */
		void
		appendOverwrite(const T& value);

		bool
		prepend(const T& value);

		/**
		 * \brief	Prepend an item to the front of the deque overwriting existing items
		 *
		 * This method, in contrast to `prepend()`, overwrites existing items in the deque
		 * if it is full. When an item is prepended to the already full deque, the back
		 * item is removed to use its space for the new item, which is inserted at front.
		 */
		void
		prependOverwrite(const T& value);

		void
		removeBack();

		void
		removeFront();

	public:
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		/**
		 * \brief	Bidirectional const iterator
		 *
		 * \todo	check if a simpler implementation is possible
		 */
		class const_iterator : public std::iterator<std::bidirectional_iterator_tag, T>
		{
			friend class BoundedDeque;

		public:
			const_iterator();
			const_iterator(const const_iterator& other);

			const_iterator& operator = (const const_iterator& other);
			const_iterator& operator ++ ();
			const_iterator& operator -- ();
			bool operator == (const const_iterator& other) const;
			bool operator != (const const_iterator& other) const;
			const T& operator * () const;
			const T* operator -> () const;

		private:
			const_iterator(Index index, const BoundedDeque * parent);

			Index index;
			const BoundedDeque * parent;

			Size count;
		};
		#pragma GCC diagnostic pop

		const_iterator
		begin() const;

		const_iterator
		end() const;

	private:
		friend class const_iterator;

		Index head;
		Index tail;
		Size size;

		T buffer[N];
	};
}

#include "deque_impl.hpp"

#endif	// MODM_DEQUE_HPP
