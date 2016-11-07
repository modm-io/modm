/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 * Copyright (c) 2012, 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_ATOMIC__LOCK_HPP
#define	XPCC_ATOMIC__LOCK_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	namespace atomic
	{
		/**
		 * \ingroup	atomic
		 * \brief		Critical section
		 *  
		 * Typical usage:
		 * \code
		 * function()
		 * {
		 *     // some code with interrupts enabled
		 *     
		 *     {
		 *         atomic::Lock lock;
		 *         
		 *         // code which should be executed with disabled interrupts.
		 *         
		 *         // with the end of this block the lock object is destroyed
		 *         // and the interrupts are reenabled.
		 *     }
		 *     
		 *     // other code with interrupts enabled
		 * }
		 * \endcode
		 * 
		 * \warning	Interrupts should be disabled the shortest possible time!
		 */
		class Lock
		{
		public:
			/**
			 * \brief	Constructor
			 * 
			 * Disables interrupts.
			 */
			xpcc_always_inline
			Lock();
			
			/**
			 * \brief	Destructor
			 * 
			 * Restore the interrupt settings.
			 */
			xpcc_always_inline
			~Lock();
		
		private:
#if defined(XPCC__CPU_AVR)
			uint8_t sreg;
#elif defined(XPCC__CPU_ARM)
			uint32_t cpsr;
#endif
		};
		
		/**
		 * \ingroup	atomic
		 * \brief	Opposite to atomic::Lock
		 * 
		 * Use this class to create a block of code with interrupts enabled
		 * inside a locked block.
		 * 
		 * Most of the time you won't need this class. But on some rare
		 * times it is useful. The xpcc::Scheduler is an example for that.
		 */
		class Unlock
		{
		public:
			/**
			 * \brief	Constructor
			 * 
			 * Enable interrupts
			 */
			xpcc_always_inline
			Unlock();
			
			/**
			 * \brief	Destructor
			 * 
			 * Restore the interrupt settings.
			 */
			xpcc_always_inline
			~Unlock();
		
		private:
#if defined(XPCC__CPU_AVR)
			uint8_t sreg;
#elif defined(XPCC__CPU_ARM)
			uint32_t cpsr;
#endif
		};
	}
}

#ifdef XPCC__CPU_AVR

	#include <avr/interrupt.h>

	xpcc::atomic::Lock::Lock() :
		sreg(SREG)
	{
		cli();
	}

	xpcc::atomic::Lock::~Lock()
	{
		SREG = sreg;
		__asm__ volatile ("" ::: "memory");
	}

	// ------------------------------------------------------------------------

	xpcc::atomic::Unlock::Unlock() :
		sreg(SREG)
	{
		sei();
	}

	xpcc::atomic::Unlock::~Unlock()
	{
		SREG = sreg;
		__asm__ volatile ("" ::: "memory");
	}

#elif defined(XPCC__CPU_CORTEX_M0) || defined(XPCC__CPU_CORTEX_M3) || defined(XPCC__CPU_CORTEX_M4) || defined(XPCC__CPU_CORTEX_M7)

	xpcc_always_inline
	xpcc::atomic::Lock::Lock()
	{
		// cpsr = __get_PRIMASK();
		// __disable_irq();
		// disable interrupts -> PRIMASK=1
		// enable interrupts -> PRIMASK=0
		asm volatile (
				"mrs %0, PRIMASK"	"\n\t"
				"cpsid i"
				: "=&r" (cpsr)
				:: "memory");
	}

	xpcc_always_inline
	xpcc::atomic::Lock::~Lock()
	{
		// __set_PRIMASK(cpsr);
		asm volatile ("msr PRIMASK, %0" : : "r" (cpsr) : "memory" );
	}

	xpcc_always_inline
	xpcc::atomic::Unlock::Unlock()
	{
		// cpsr = __get_PRIMASK();
		// __disable_irq();
		// disable interrupts -> PRIMASK=1
		// enable interrupts -> PRIMASK=0
		asm volatile (
				"mrs %0, PRIMASK"	"\n\t"
				"cpsie i"
				: "=&r" (cpsr)
				:: "memory");
	}

	xpcc_always_inline
	xpcc::atomic::Unlock::~Unlock()
	{
		// __set_PRIMASK(cpsr);
		asm volatile ("msr PRIMASK, %0" : : "r" (cpsr) : "memory" );
	}

#elif defined(XPCC__OS_HOSTED)

	xpcc::atomic::Lock::Lock()
	{
	}

	xpcc::atomic::Lock::~Lock()
	{
	}

	xpcc::atomic::Unlock::Unlock()
	{
	}

	xpcc::atomic::Unlock::~Unlock()
	{
	}
#else
#	error	"Please provide an atomic lock implementation for this target!"
#endif

#endif	// XPCC_ATOMIC__LOCK_HPP
