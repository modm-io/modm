/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2016-2017, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ATOMIC_QUEUE_HPP
#define	MODM_ATOMIC_QUEUE_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <modm/architecture/utils.hpp>

namespace modm
{
	namespace atomic
	{
		/**
		 * \ingroup	modm_architecture_atomic
		 * \brief	Interrupt save queue
		 *
		 * A maximum size of 254 is allowed for 8-bit microcontrollers.
		 *
		 * \todo	This implementation should work but could be improved
		 */
		template<typename T, std::size_t N>
		class Queue
		{
		public:
			// select the type of the index variables with some template magic :-)
			using Index = std::conditional_t< (N >= 254), uint16_t, uint8_t >;

			using Size = Index;

		public:
			Queue();

			bool
			isFull() const;

			bool
			isNotFull() const { return not isFull(); }

			/**
			 * \returns	\c false if less than three elements
			 * 			can be stored in queue.
			 *
			 * Only works with queue with more than three elements.
			 */
			bool
			isNearlyFull() const;

			bool
			isEmpty() const;

			bool
			isNotEmpty() const { return not isEmpty(); }

			/**
			 * Check if the queue is nearly empty.
			 *
			 * \returns	\c true if less than three elements are stored
			 * 			in the queue, \c false otherwise.
			 *
			 * Only works with queue with more than three elements.
			 * TODO: calculations are approximate and may include off-by-one errors.
			 */
			bool
			isNearlyEmpty() const;

			Size
			getMaxSize() const;

			Size
			getSize() const;

			const T&
			get() const;

			bool
			push(const T& value);

			void
			pop();

		private:
			volatile Index head;
			volatile Index tail;

			T buffer[N+1];
		};
	}
}

#include "queue_impl.hpp"

#endif	// MODM_ATOMIC_QUEUE_HPP
