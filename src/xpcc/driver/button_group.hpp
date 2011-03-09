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

#ifndef XPCC__BUTTON_GROUP_HPP
#define XPCC__BUTTON_GROUP_HPP

#include <stdint.h>

#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>

namespace xpcc
{
	/**
	 * \brief	Debouncing signals
	 * 
	 * This class is able to buttons eight buttons at the same time. The
	 * buttons have to low-active. If this isn't the case invert their signal
	 * before passing it to the update() method.
	 * 
	 * The update() method needs to be called periodically for example
	 * every 10ms. Preferred in a timer interrupt function.
	 * 
	 * Example:
	 * \include gpio/button_group/main.cpp
	 * 
	 * The detection for long or repreated presses works only correct for
	 * one key at a time. This constraint only applies to buttons listed in the
	 * \em mask variable.
	 * 
	 * Mode 1:
\verbatim
                 Timeline ---->
                      __      _________________      __
getState()       ____/  \____/                 \____/  \____
isPressed()      ----X-------X----------------------X-------
isRepeated()     --------------------X--X--X--X-------------
isReleased()     -------X----------------------X-------X----
                             |       |__|__|
                             |_______| \ /
                                  \    interval
                                  timeout
\endverbatim
	 * 
	 * Mode 2:
\verbatim
                      __      _________________      __
getState()       ____/  \____/                 \____/  \____
isPressedShort() -------X------------------------------X----
isPressedLong()  --------------------X----------------------
isReleased()     -------X----------------------X-------X----
\endverbatim
	 * 
	 * This implementation is based on the C functions written
	 * by Peter Dannegger (see http://www.mikrocontroller.net/topic/48465).
	 * 
	 * \tparam	T			Container type (\c uint8_t for eight buttons or
	 * 							\c uint16_t for up to 16 buttons)
	 * 
	 * \ingroup	driver
	 */
	template <typename T = uint8_t>
	class ButtonGroup
	{
	public:
		/** \brief	Button masks
		 * 
		 * Provided for convenience only.
		 * 
		 * Normally it is best to define your own meaningful names for
		 * the buttons.
		 */
		enum Identifier
		{
			NONE = 0x00,
			BUTTON0 = 0x01,
			BUTTON1 = 0x02,
			BUTTON2 = 0x04,
			BUTTON3 = 0x08,
			BUTTON4 = 0x10,
			BUTTON5 = 0x20,
			BUTTON6 = 0x40,
			BUTTON7 = 0x80,
			ALL = 0xFF,
		};
		
	public:
		/** \brief	Constructor
		 *
		 * \param	mask		Repeat mask, only buttons listed here can be used
		 * 						with the methods isRepeated(), isPressedShort()
		 * 						and isPressedLong()
		 * \param	timeout		Timeout for the repeat operation
		 * \param	interval	Repeat interval
		 */
		ButtonGroup(T mask, uint16_t timeout = 50, uint16_t interval = 20);
		
		/// Get the current (debounced) state of a key
		T
		getState(T mask) const;
		
		/// Check if a key has been released
		T
		isReleased(T mask);
		
		/** \brief	 check if a key has been pressed
		 * 
		 * Each pressed key is reported only once.
		 * 
		 * \code
		 * xpcc::ButtonGroup<> buttons;
		 * ...
		 * while (1) {
		 *		...
		 * 	if (buttons.isPressed(xpcc::ButtonGroup::BUTTON0)) {
		 * 		// do something
		 * 	}
		 * }
		 * \endcode
		 */
		T
		isPressed(T mask);
	
		/** \brief	Check if a key has been pressed long enough such that the
		 *			key repeat functionality kicks in.
		 *
		 * After a small setup delay the key is reported beeing pressed
		 * subsequent calls to this function. This simulates the user
		 * repeatedly pressing and releasing the key.
		 * 
		 * \code
		 * xpcc::ButtonGroup<> buttons(xpcc::ButtonGroup::BUTTON0);
		 * ...
		 * while (1) {
		 *		...
		 * 	if (buttons.isPressed(xpcc::ButtonGroup::BUTTON0) || 
		 *		buttons.isRepeated(xpcc::ButtonGroup::BUTTON0))
		 * 	{
		 * 		// do something
		 * 	}
		 * }
		 * \endcode
		 * 
		 * \see	isPressed()
		 */
		T
		isRepeated(T mask);
		
		/** \brief	Get buttons which were pressed short
		 * 
		 * \code
		 * xpcc::ButtonGroup<> buttons(xpcc::ButtonGroup::BUTTON0);
		 * ...
		 * while (1) {
		 * 	...
		 * 	if (buttons.isPressedShort(xpcc::ButtonGroup::BUTTON0)) {
		 * 		// do something
		 * 	}
		 * 	if (buttons.isPressedLong(xpcc::ButtonGroup::BUTTON0)) {
		 * 		// do something different
		 * 	}
		 * }
		 * \endcode
		 * 
		 * \warning	Use this function only in combination with
		 *			isPressedLong(), otherwise it won't work correctly!
		 * 
		 * \see	isPressedLong()
		 */
		T
		isPressedShort(T mask);
		
		/** \brief	Get buttons which were pressed long
		 *
		 * \warning	Use this function only in combination with
		 *			isPressedShort(), otherwise it won't work correctly!
		 * \see	isPressedShort()
		 */
		T
		isPressedLong(T mask);
		
		/** \brief	Update internal state
		 *
		 * Call this function periodically every 5 to 10ms
		 * 
		 * \param	input	input signals
		 */
		ALWAYS_INLINE void
		update(T input)
		{
			T i = state ^ ~input;		// key changed?
			ct0 = ~(ct0 & i);			// reset or count ct0
			ct1 = ct0 ^ (ct1 & i);		// reset or count ct1
			i &= ct0 & ct1;				// count until roll over?
			
			state ^= i;					// then toggle debounced state
			pressState |= state & i;	// 0->1: key press detected
			releaseState |= ~state & i;	// 0->1: key release detected
			
			if ((state & repeatMask) == 0) {
				repeatCounter = timeout;
			}
			if (--repeatCounter == 0) {
				repeatCounter = interval;
				repeatState |= state & repeatMask;
			}
		}
		
	protected:
		const uint16_t timeout;
		const uint16_t interval;
		const T repeatMask;
		uint16_t repeatCounter;
		
		T state;		//!< debounced and inverted key state:
						//!< bit = 1: key pressed
		T pressState;
		T releaseState;
		T repeatState;
	
	private:
		T ct0;
		T ct1;
	};
}

#include "button_group_impl.hpp"

#endif // XPCC__BUTTON_GROUP_HPP
