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

#ifndef XPCC__DEBOUNCE_HPP
#define XPCC__DEBOUNCE_HPP

#include <stdint.h>
#include <xpcc/utils/macros.hpp>

namespace xpcc
{
	/**
	 * \brief	Debouncing signals
	 * 
	 * This class is able to debounce eight keys at the same time. The
	 * keys have to low-active. If this isn't the case invert their signal
	 * before passing it to the update() method.
	 * 
	 * The update() method needs to be called periodically for example
	 * every 10ms. Preferred in a timer interrupt function.
	 * 
	 * Example:
	 * \include debounce.cpp
	 * 
	 * The detection for long or repreated presses works only correct for
	 * one key at a time. This constraint only applies to keys listed in the
	 * \em mask variable.
	 * 
	 * This implementation is based on the C debounce functions written
	 * by Peter Dannegger (see http://www.mikrocontroller.net/topic/48465).
	 * 
	 * \ingroup	driver
	 */
	class Debounce
	{
	public:
		/** \brief	
		 *
		 * Provided for convenience only.
		 */
		typedef enum {
			BUTTON0 = (1 << 0),
			BUTTON1 = (1 << 1),
			BUTTON2 = (1 << 2),
			BUTTON3 = (1 << 3),
			BUTTON4 = (1 << 4),
			BUTTON5 = (1 << 5),
			BUTTON6 = (1 << 6),
			BUTTON7 = (1 << 7)
		} Buttons;
		
	public:
		/** \brief	Constructor
		 *
		 * \param	mask	TODO
		 * \param	start	TODO
		 * \param	next	TODO
		 */
		Debounce(uint8_t mask = 0, uint8_t start = 50, uint8_t next = 20);
		
		/// \brief	Get the current (debounced) state of a key
		uint8_t
		getState(uint8_t mask);
		
		/// \brief	check if a key has been released
		uint8_t
		getRelease(uint8_t mask);
		
		/** \brief	 check if a key has been pressed
		 * 
		 * Each pressed key is reported only once.
		 * 
		 * \code
		 * xpcc::Debounce keys;
		 * ...
		 * while (1) {
		 *		...
		 * 	if (keys.getPress(Debounce::KEY0)) {
		 * 		// do something
		 * 	}
		 * }
		 * \endcode
		 */
		uint8_t
		getPress(uint8_t mask);
	
		/** \brief	Check if a key has been pressed long enough such that the
		 *			key repeat functionality kicks in.
		 *
		 * After a small setup delay the key is reported beeing pressed
		 * subsequent calls to this function. This simulates the user
		 * repeatedly pressing and releasing the key.
		 * 
		 * \code
		 * xpcc::Debounce keys(Debounce::KEY0);
		 * ...
		 * while (1) {
		 *		...
		 * 	if (keys.getPress(Debounce::KEY0) || 
		 *			keys.getRepeat(Debounce::KEY0))
		 * 	{
		 * 		// do something
		 * 	}
		 * }
		 * \endcode
		 * 
		 * \see	getPress()
		 */
		uint8_t
		getRepeat(uint8_t mask);
		
		/** \brief	Get keys which were pressed short
		 * 
		 * \code
		 * xpcc::Debounce keys(Debounce::KEY0);
		 * ...
		 * while (1) {
		 * 	...
		 * 	if (keys.getShortPress(Debounce::KEY0)) {
		 * 		// do something
		 * 	}
		 * 	if (keys.getLongPress(Debounce::KEY0)) {
		 * 		// do something different
		 * 	}
		 * }
		 * \endcode
		 * 
		 * \warning	Use this function only in combination with
		 *				getLongPress(), otherwise it won't work correctly!
		 * 
		 * \see	getShortPress()
		 */
		uint8_t
		getShortPress(uint8_t mask);
		
		/** \brief	Get keys which were pressed long
		 *
		 * \see	getShortPress()
		 */
		uint8_t
		getLongPress(uint8_t mask);
		
		/** \brief	Update internal state
		 *
		 * Call this function periodically.
		 * 
		 * \param	input	input signals
		 */
		ALWAYS_INLINE void
		update(uint8_t input)
		{
			uint8_t i;
			
			i = state ^ ~input;			// key changed?
			ct0 = ~(ct0 & i);			// reset or count ct0
			ct1 = ct0 ^ (ct1 & i);		// reset or count ct1
			i &= ct0 & ct1;				// cound until roll over?
			
			state ^= i;					// then toggle debounced state
			pressState |= state & i;	// 0->1: key press detected
			releaseState |= ~state & i;	// 0->1: key release detected
			
			if ((state & repeatMask) == 0) {
				repeatCounter = repeatStart;
			}
			if (--repeatCounter == 0) {
				repeatCounter = repeatNext;
				repeatState |= state & repeatMask;
			}
		}
		
	protected:
		const uint8_t repeatMask;
		const uint8_t repeatStart;
		const uint8_t repeatNext;
		uint8_t repeatCounter;
		
		uint8_t state;		//!< debounced and inverted key state:
							//!< bit = 1: key pressed
		uint8_t pressState;
		uint8_t releaseState;
		uint8_t repeatState;
	
	private:
		uint8_t ct0;
		uint8_t ct1;
	};
}

#endif // XPCC__DEBOUNCE_HPP
