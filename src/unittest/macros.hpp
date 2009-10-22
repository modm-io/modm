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

#ifndef	UNITTEST__MACROS_HPP
#define	UNITTEST__MACROS_HPP

#include <xpcc/utils/misc.hpp>

#define	TEST_FLOAT_EPISLON		0.00001f

#ifdef __DOXYGEN__

/// @ingroup	unittest
/// @brief		Verify (expr) is true 
#define	TEST_ASSERT(expr)

/// @ingroup	unittest
/// @brief		Verify (x==y)
#define	TEST_ASSERT_EQUALS(x, y)

/// @ingroup	unittest
/// @brief		Verify (x==y) for floating pointer values
#define	TEST_ASSERT_EQUALS_FLOAT(x, y)

/// @ingroup	unittest
/// @brief		Verify (x==y) up to d
///
/// This macro verifies two values are equal up to a delta
#define	TEST_ASSERT_DELTA(x, y, d)

/// @ingroup	unittest
/// @brief		Verify (lower <= value <= upper)
#define	TEST_ASSERT_RANGE(value, lower, upper)

/// @ingroup	unittest
/// @brief		Fail unconditionally  
#define	TEST_FAIL(msg)

#else // !__DOXYGEN__

#define	TEST_ASSERT(expr)	\
	if (expr) { \
		reportPass(); \
	} else { \
		reportFailure(__LINE__) << '\n'; \
	}

#define	TEST_ASSERT_EQUALS(x, y) \
	if (x == y) { \
		reportPass(); \
	} else { \
		reportFailure(__LINE__) << x << " == " << y << '\n'; \
	}

#define	TEST_ASSERT_EQUALS_FLOAT(x, y) \
	TEST_ASSERT_DELTA(x, y, TEST_FLOAT_EPISLON)

#define	TEST_ASSERT_DELTA(x, y, d) \
	if (((x + d) > y) && ((x - d) < y)) { \
		reportPass(); \
	} else { \
		reportFailure(__LINE__) << x << " == " << y << '\n'; \
	}

#define	TEST_ASSERT_RANGE(value, lower, upper) \
	if ((value >= lower= && (value <= upper)) { \
		reportPass(); \
	} else { \
		reportFailure(__LINE__) << value << "not in range "\
					"[" << lower << "," << upper << "]\n"; \
	}

#define	TEST_FAIL(msg) \
	{	reportFailure(__LINE__) << '\"' << msg << "\"\n"; } \

#endif	// __DOXYGEN__

#endif	// UNITTEST__MACROS_HPP
