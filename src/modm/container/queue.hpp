/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_QUEUE_HPP
#define	MODM_QUEUE_HPP

#include <cstddef>

#include "deque.hpp"

namespace modm
{
	/**
	 * \brief	FIFO queue
	 *
	 * getFront(), removeFront(), append()
	 *
	 * \tparam	T			type
	 * \tparam	Container	container
	 *
	 * \see		Deque()
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_container
	 */
	template<typename T,
			 typename Container>
	class Queue
	{
	public:
		typedef typename Container::Size Size;

	public:
		inline bool
		isEmpty() const
		{
			return c.isEmpty();
		}

		inline bool
		isNotEmpty() const { return not isEmpty(); };

		inline bool
		isFull() const
		{
			return c.isFull();
		}

		inline bool
		isNotFull() const
		{
			return not isFull();
		}

		inline Size
		getSize() const
		{
			return c.getSize();
		}

		inline Size
		getMaxSize() const
		{
			return c.getMaxSize();
		}

		/// Access first element
		inline T&
		get()
		{
			return c.getFront();
		}

		/// Access first element
		inline const T&
		get() const
		{
			return c.getFront();
		}

		inline bool
		push(const T& value)
		{
			return c.append(value);
		}

		inline void
		pop()
		{
			c.removeFront();
		}

	protected:
		Container c;
	};


	// ------------------------------------------------------------------------
	/**
	 * \brief	Bounded queue
	 * \ingroup	modm_container
	 */
	template<typename T,
			 std::size_t N,
			 typename Container = BoundedDeque<T, N> >
	class BoundedQueue : public Queue<T, Container>
	{
	};

}

#endif	// MODM_QUEUE_HPP
