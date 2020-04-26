/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ATOMIC_CONTAINER_HPP
#define	MODM_ATOMIC_CONTAINER_HPP

#include <modm/architecture/interface/atomic_lock.hpp>

namespace modm
{
	namespace atomic
	{
		/**
		 * \brief	Atomic access to objects
		 *
		 * The atomic container wraps objects and provides atomic access to
		 * them. This comes in handy when simple objects are accessed by an
		 * interrupt and the main program. The container provides secure
		 * access without much work in this case.
		 *
		 * \warning	This class should be used with precaution because the
		 * 			objects are copied for every access.
		 *
		 * Example:
		 * \code
		 * modm::atomic::Containter<uint32_t> data;
		 *
		 * // interrupt routine
		 * ISR() {
		 *     data.set(123);
		 * }
		 *
		 * function() {
		 *     uint32_t localData = data.get();
		 *     ...
		 * }
		 * \endcode
		 *
		 * If the interrupt routine is not interruptible (this is the default
		 * for the ATmega, but not for the ATxmega!) no atomic access is needed
		 * then:
		 * \code
		 * modm::atomic::Containter<uint32_t> data;
		 *
		 * ISR() {
		 *     data.directAccess() = 123;
		 * }
		 * \endcode
		 *
		 * This will slightly faster because no lock needs to be created.
		 *
		 * If the object is big or a lot of accessed one after another are
		 * done, consider dropping this class and create critical sections
		 * for the access by yourself with modm::atomic::Lock.
		 *
		 * This can be a lot faster because the object don't need to be copied
		 * as it is the case here!
		 *
		 * \author	Fabian Greif
		 * \ingroup	modm_architecture_atomic
		 */
		template<typename T>
		class Container
		{
		public:
			Container(const T& data = T()) :
				object(data)
			{
			}

			inline void
			set(T value)
			{
				Lock lock;
				this->object = value;
			}

			inline T
			get()
			{
				Lock lock;
				return (volatile T&)this->object;
			}

			/**
			 * \brief	Write a new value to the wrapped object and return
			 * 			the old value.
			 *
			 * This function is needed quite often when exchanging flags
			 * between interrupt routines and the main-loop.
			 *
			 * \code
			 * modm::atomic::Container<uint8_t> isrCounter;
			 *
			 * ISR() {
			 *     ...
			 *     isrCounter.directAccess() += 1;
			 * }
			 *
			 * function() {
			 *     static uint16_t counter;
			 *     counter += isrCounter.swap(0);
			 *     ...
			 * }
			 * \endcode
			 *
			 * The same behavior for boolean values is provide by
			 * modm::atomic::Flag()
			 */
			inline T
			swap(T value)
			{
				Lock lock;

				T oldValue = (volatile T&)this->object;
				this->object = value;

				return oldValue;
			}

			/**
			 * \brief		Get direct access to the object
			 *
			 * \return		Reference to the object
			 * \warning		If the object is accessed through this function the
			 *				operations are not atomic! This might be useful
			 *				inside a interrupt context where no atomic
			 *				access is necessary.
			 */
			inline T&
			directAccess()
			{
				return this->object;
			}

		private:
			Container(const Container&);

			Container&
			operator = (const Container&);

			T object;
		};
	}
}

#endif	// MODM_ATOMIC_CONTAINER_HPP
