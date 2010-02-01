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

#ifndef	UNITTEST__HARNESS_HPP
#define	UNITTEST__HARNESS_HPP

#include "controller.hpp"

#define	TEST_FLOAT_EPISLON		0.00001f

#ifdef __DOXYGEN__

/// \ingroup	unittest
/// \brief		Verify (expr) is true 
#define	TEST_ASSERT_TRUE(expr)

/// \ingroup	unittest
/// \brief		Verify (expr) is false 
#define	TEST_ASSERT_FALSE(expr)

/// \ingroup	unittest
/// \brief		Verify (x==y)
#define	TEST_ASSERT_EQUALS(x, y)

/// \ingroup	unittest
/// \brief		Verify (x==y) for floating pointer values
#define	TEST_ASSERT_EQUALS_FLOAT(x, y)

/// \ingroup	unittest
/// \brief		Verify (x==y) up to d
///
/// This macro verifies two values are equal up to a delta
#define	TEST_ASSERT_EQUALS_DELTA(x, y, d)

/// \ingroup	unittest
/// \brief		Verify (lower <= value <= upper)
#define	TEST_ASSERT_EQUALS_RANGE(value, lower, upper)

/// \ingroup	unittest
/// \brief		Check if the arrays contains the same data
#define	TEST_ASSERT_EQUALS_ARRAY(array1, array2, start, count)

/// \ingroup	unittest
/// \brief		Fail unconditionally  
#define	TEST_FAIL(msg)

#else // !__DOXYGEN__

#include <xpcc/hal/flash/flash_pointer.hpp>

namespace unittest
{
	EXTERN_FLASH_STRING(stringEqual);
	EXTERN_FLASH_STRING(stringNotInRange);
	EXTERN_FLASH_STRING(stringNotTrue);
	EXTERN_FLASH_STRING(stringNotFalse);
}

#ifdef	TEST_RETURN_ON_FAIL
	#define	TEST_RETURN__	return
#else
	#define	TEST_RETURN__	
#endif

#define	TEST_REPORTER__		unittest::Controller::instance().getReporter()

#define	TEST_ASSERT_TRUE(expr)	\
	if (expr) { \
		TEST_REPORTER__.reportPass(); \
	} else { \
		TEST_REPORTER__.reportFailure(__LINE__) \
			<< xpcc::toFlashPointer(unittest::stringNotTrue); \
		TEST_RETURN__; \
	}

#define	TEST_ASSERT_FALSE(expr)	\
	if (expr) { \
		TEST_REPORTER__.reportFailure(__LINE__) \
			<< xpcc::toFlashPointer(unittest::stringNotFalse); \
		TEST_RETURN__; \
	} else { \
		TEST_REPORTER__.reportPass(); \
	}

#define	TEST_ASSERT_EQUALS(x, y) \
	if (x == y) { \
		TEST_REPORTER__.reportPass(); \
	} else { \
		TEST_REPORTER__.reportFailure(__LINE__) \
			<< x << xpcc::toFlashPointer(unittest::stringEqual) << y << '\n'; \
		TEST_RETURN__; \
	}

#define	TEST_ASSERT_EQUALS_FLOAT(x, y) \
	TEST_ASSERT_EQUALS_DELTA(x, y, TEST_FLOAT_EPISLON)

#define	TEST_ASSERT_EQUALS_DELTA(x, y, d) \
	if ((((x) + d) >= y) && (((x) - d) <= y)) { \
		TEST_REPORTER__.reportPass(); \
	} else { \
		TEST_REPORTER__.reportFailure(__LINE__) \
			<< x << xpcc::toFlashPointer(unittest::stringEqual) << y << '\n'; \
		TEST_RETURN__; \
	}

#define	TEST_ASSERT_EQUALS_RANGE(value, lower, upper) \
	if ((value >= lower) && (value <= upper)) { \
		TEST_REPORTER__.reportPass(); \
	} else { \
		TEST_REPORTER__.reportFailure(__LINE__) \
			<< value << xpcc::toFlashPointer(unittest::stringNotInRange) \
			<< '[' << lower << ',' << upper << ']' << '\n'; \
		TEST_RETURN__; \
	}

// FIXME replace with template functions!
#define	TEST_ASSERT_EQUALS_ARRAY(array1, array2, start, count) \
	do { \
		unsigned int i; \
		bool failure = false; \
		for (i = start; i < (start + count); i++) { \
			if (array1[i] != array2[i]) { \
				failure = true; \
				break; \
			} \
		} \
		if (failure) { \
			xpcc::IOStream& stream = TEST_REPORTER__.reportFailure(__LINE__); \
			stream << '\n'; \
			stream << "["; \
			for (unsigned int k = start; k < (start + count); k++) { \
				stream << array1[k] << ", "; \
			} \
			stream << "]\n"; \
			\
			stream << "["; \
			for (unsigned int k = start; k < (start + count); k++) { \
				stream << array2[k] << ", "; \
			} \
			stream << "]\n"; \
			TEST_RETURN__; \
		} else { \
			TEST_REPORTER__.reportPass(); \
		} \
	} while (0)

#define	TEST_FAIL(msg) \
	do {	TEST_REPORTER__.reportFailure(__LINE__) \
			<< msg << '\n'; \
		TEST_RETURN__; \
	} while (0)

#endif	// __DOXYGEN__

#endif	// UNITTEST__HARNESS_HPP
