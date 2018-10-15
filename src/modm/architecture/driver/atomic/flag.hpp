/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ATOMIC_FLAG_HPP
#define	MODM_ATOMIC_FLAG_HPP

#include <modm/architecture/interface/atomic_lock.hpp>

namespace modm
{
	namespace atomic
	{
		/**
		 * \brief		Flag to signal events between interrupts and the
		 *				main-loop
		 *
		 * This class is quite handy when exchanging flags between a interrupt
		 * routine and the main-loop.
		 *
		 * \code
		 * modm::atomic::Flag isrFlag;
		 *
		 * ISR() {
		 *     ...
		 *     isrFlag.set();
		 * }
		 *
		 * function() {
		 *     bool flag = isrFlag.testAndSet(false);
		 *     ...
		 * }
		 * \endcode
		 *
		 * \ingroup	modm_architecture_atomic
		 * \author	Fabian Greif
		 */
		class Flag
		{
		public:
			Flag(bool state = false);

			Flag(const Flag& other);

			Flag&
			operator = (const Flag& other);

			/**
			 * \brief	Check state of the flag
			 */
			inline bool
			test() const
			{
				return this->state;
			}

			/**
			 * \brief	Set flag
			 */
			inline void
			set()
			{
				this->state = true;
			}

			/**
			 * \brief	Clear flag
			 */
			inline void
			reset()
			{
				this->state = false;
			}

			/**
			 * \brief	Sets the flag to a new value and returns the old one
			 */
			inline bool
			testAndSet(bool value)
			{
				Lock lock;

				bool oldValue = this->state;
				this->state = value;

				return oldValue;
			}

		private:
			volatile bool state;
		};
	}
}

#endif	// MODM_ATOMIC_FLAG_HPP
