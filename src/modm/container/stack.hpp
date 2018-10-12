/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_STACK_HPP
#define	MODM_STACK_HPP

#include <cstddef>
#include <stdint.h>

#include "deque.hpp"

namespace modm
{
	/**
	 * \brief	LIFO stack
	 *
	 * Elements are pushed/popped from the "back" of the specific container,
	 * which is known as the top of the stack.
	 *
	 * getFront(), removeFront(), prepend()
	 *
	 * \see		Deque()
	 *
	 * \author	Fabian Greif
	 * \ingroup		modm_container
	 */
	template<typename T,
			 typename Container>
	class Stack
	{
	public:
		typedef typename Container::Size Size;

	public:
		bool
		isEmpty()
		{
			return c.isEmpty();
		}

		bool
		isFull()
		{
			return c.isFull();
		}

		Size
		getSize()
		{
			return c.getSize();
		}

		Size
		getMaxSize()
		{
			return c.getMaxSize();
		}

		T&
		get()
		{
			return c.getFront();
		}

		const T&
		get() const
		{
			return c.getFront();
		}

		bool
		push(const T& value)
		{
			return c.prepend(value);
		}

		void
		pop()
		{
			c.removeFront();
		}

	protected:
		Container c;
	};

	// ------------------------------------------------------------------------
	/**
	 * \brief		Bounded stack
	 *
	 * \ingroup		modm_container
	 */
	template<typename T,
			 std::size_t N,
			 typename Container = BoundedDeque<T, N> >
	class BoundedStack : public Stack<T, Container>
	{
	};
}

#endif	// MODM_STACK_HPP
