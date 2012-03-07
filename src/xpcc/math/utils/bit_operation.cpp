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
 * $Id: bit_operation.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include "bit_operation.hpp"

// ----------------------------------------------------------------------------
uint8_t
xpcc::bitReverse(uint8_t n)
{
	n = ((uint8_t) (n >> 1) & 0x55) | ((uint8_t) (n << 1) & 0xaa);
	n = ((uint8_t) (n >> 2) & 0x33) | ((uint8_t) (n << 2) & 0xcc);
	
	return swap(n);
}

uint16_t
xpcc::bitReverse(uint16_t n)
{
	n = ((n >>  1) & 0x5555) | ((n <<  1) & 0xaaaa);
	n = ((n >>  2) & 0x3333) | ((n <<  2) & 0xcccc);
	n = ((n >>  4) & 0x0f0f) | ((n <<  4) & 0xf0f0);
	
	return swap(n);
}

uint32_t
xpcc::bitReverse(uint32_t n)
{
	n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
	n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
	n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
	n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
	n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
	
	return n;
}

// ----------------------------------------------------------------------------
std::size_t
xpcc::bitCount(uint8_t n)
{
	n = ((uint8_t) (n >> 1) & 0x55) + (n & 0x55);
	n = ((uint8_t) (n >> 2) & 0x33) + (n & 0x33);
	n = ((uint8_t) (n >> 4) + n) & 0xf;
	
	return n;
}

std::size_t
xpcc::bitCount(uint16_t n)
{
	return (bitCount((uint8_t) (n)) +
			bitCount((uint8_t) (n >> 8)));
}

std::size_t
xpcc::bitCount(uint32_t n)
{
	n = ((n >> 1) & 0x55555555) + (n & 0x55555555);
	n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
	n = ((n >> 4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);
	
	return n % 255;
}
