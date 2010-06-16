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

#include <xpcc/container/pair.hpp>
#include <xpcc/architecture/general/accessor/flash.hpp>

#include "pair_test.hpp"

// check if Pair could be stored inside the Flash memory
typedef xpcc::Pair<uint8_t, int16_t> myPair;

FLASH(myPair values[]) = {
	{ 1, 3 },
	{ 5, 30 },
	{ 27, 100 }
};

void
PairTest::testPair()
{
	xpcc::accessor::Flash<myPair> ptr(values);
	
	TEST_ASSERT_EQUALS(ptr[1].getFirst(), 5);
	TEST_ASSERT_EQUALS(ptr[1].getSecond(), 30);
	
	TEST_ASSERT_EQUALS(ptr[2].getFirst(), 27);
	TEST_ASSERT_EQUALS(ptr[2].getSecond(), 100);
	
	xpcc::Pair<int16_t, int16_t> pair = { 124, -1523 };
	
	TEST_ASSERT_EQUALS(pair.getFirst(), 124);
	TEST_ASSERT_EQUALS(pair.getSecond(), -1523);
}
