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
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/interpolation/lagrange.hpp>

#include "lagrange_interpolation_test.hpp"

void
LagrangeInterpolationTest::testCreation()
{
	typedef xpcc::Pair<uint8_t, float> Point;
	
	Point points[3] =
	{
		{  10, -50 },
		{  50,   0 },
		{ 100,  50 }
	};
	
	xpcc::interpolation::Lagrange<Point> value(points, 3);
}

void 
LagrangeInterpolationTest::testInterpolation()
{
	typedef xpcc::Pair<float, float> Point;
	
	// interpolate x^2 over the range of 1 <= x <= 3
	Point points[3] =
	{
		{ 1, 1 },
		{ 2, 4 },
		{ 3, 9 }
	};
	
	xpcc::interpolation::Lagrange<Point> value(points, 3);
	
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(1.f),   1.f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(1.5f),  2.25f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(2.f),   4.f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(2.5f),  6.25f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(3.f),   9.f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(3.5f), 12.25f);
}


