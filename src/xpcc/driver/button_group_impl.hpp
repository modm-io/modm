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
	#error	"Don't include this file directly, use 'button_group.hpp' instead!"
#endif

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::ButtonGroup(T mask) :
	repeatMask(mask), repeatCounter(TIMEOUT),
	state(), pressState(), releaseState(), repeatState(), ct0(), ct1()
{
}

// ----------------------------------------------------------------------------

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
T
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::getState(T mask) const
{
	atomic::Lock lock;
	
	return mask & state;
}

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
T
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::isReleased(T mask)
{
	atomic::Lock lock;
	
	mask &= releaseState;
	releaseState ^= mask;
	
	return mask;
}

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
T
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::isPressed(T mask)
{
	atomic::Lock lock;
	
	mask &= pressState;
	pressState ^= mask;
	
	return mask;
}

// ----------------------------------------------------------------------------

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
T
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::isRepeated(T mask)
{
	atomic::Lock lock;
	
	mask &= repeatState;
	repeatState ^= mask;
	
	return mask;
}

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
T
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::isPressedShort(T mask)
{
	atomic::Lock lock;
	
	// get all keys which were pressed but are currently not pressed. This
	// must be a short press then, otherwise the isPressedLong() method
	// would have reseted pressState.
	mask = mask & pressState & ~state;
	pressState ^= mask;
	
	return mask;
}

template <uint16_t TIMEOUT, uint16_t INTERVAL, typename T>
T
xpcc::ButtonGroup<TIMEOUT, INTERVAL, T>::isPressedLong(T mask)
{
	atomic::Lock lock;
	
	// get all keys which are long enough pressState so that the repeatState
	// variable was set
	mask = mask & repeatState;
	repeatState ^= mask;
	mask = mask & pressState;
	pressState ^= mask;
	
	return mask;
}
