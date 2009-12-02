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

#include <xpcc/hal/atomic/lock.hpp>

#include "debounce.hpp"

xpcc::Debounce::Debounce(uint8_t mask, uint8_t start, uint8_t next) :
	repeatMask(mask), repeatStart(start), repeatNext(next), repeatCounter(start),
	state(0), pressState(0), releaseState(0), repeatState(0), ct0(0), ct1(0)
{
}

uint8_t
xpcc::Debounce::getState(uint8_t mask)
{
	atomic::Lock lock;
	
	return mask & state;
}

uint8_t
xpcc::Debounce::getRelease(uint8_t mask)
{
	atomic::Lock lock;
	
	mask &= releaseState;
	releaseState ^= mask;
	
	return mask;
}

uint8_t
xpcc::Debounce::getPress(uint8_t mask)
{
	atomic::Lock lock;
	
	mask &= pressState;
	pressState ^= mask;
	
	return mask;
}

uint8_t
xpcc::Debounce::getRepeat(uint8_t mask)
{
	atomic::Lock lock;
	
	mask &= repeatState;
	repeatState ^= mask;
	
	return mask;
}

uint8_t
xpcc::Debounce::getShortPress(uint8_t mask)
{
	atomic::Lock lock;
	
	// get all keys which were pressed but are currently not pressed. This
	// could only have been a short press, otherwise the getLongPress() method
	// would have reset pressState.
	mask = mask & pressState & ~state;
	pressState ^= mask;
	
	return mask;
}

uint8_t
xpcc::Debounce::getLongPress(uint8_t mask)
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
