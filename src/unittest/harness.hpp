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

/**
 * \brief	Verify (expr) is true
 * 
 * \ingroup	unittest
 */
#define	TEST_ASSERT_TRUE(expr)

/**
 * \brief	Verify (expr) is false
 * 
 * \ingroup	unittest
 */
#define	TEST_ASSERT_FALSE(expr)

/**
 * \brief	Verify (x==y)
 * 
 * Shortcut with extended output message for `TEST_ASSERT_TRUE(x == y);`
 * 
 * \ingroup	unittest
 */
#define	TEST_ASSERT_EQUALS(x, y)

/**
 * \brief	Verify (x==y) for floating point values
 * \ingroup	unittest
 */
#define	TEST_ASSERT_EQUALS_FLOAT(x, y)

/**
 * \brief	Verify (x==y) up to d
 *
 * This macro verifies two values are equal up to a delta
 * 
 * \ingroup	unittest
 */
#define	TEST_ASSERT_EQUALS_DELTA(x, y, d)

/**
 * \brief	Verify (lower <= value <= upper)
 * \ingroup	unittest
 */
#define	TEST_ASSERT_EQUALS_RANGE(value, lower, upper)

/**
 * \brief	Check if the arrays contains the same data
 * 
 * start is optional (default = 0).
 * 
 * \ingroup	unittest
 */
#define	TEST_ASSERT_EQUALS_ARRAY(array1, array2, count, start)

/**
 * \brief	Fail unconditionally  
 * \ingroup	unittest
 */
#define	TEST_FAIL(msg)

#else // !__DOXYGEN__

#include <xpcc/architecture/driver/accessor/flash.hpp>

namespace unittest
{
	EXTERN_FLASH_STORAGE_STRING(stringEqual);
	EXTERN_FLASH_STORAGE_STRING(stringNotInRange);
	EXTERN_FLASH_STORAGE_STRING(stringNotTrue);
	EXTERN_FLASH_STORAGE_STRING(stringNotFalse);
}

#ifdef	UNITTEST__RETURN_ON_FAIL
	#define	TEST_RETURN__(x)	do { if (!x) { return; } } while (0)
#else
	#define	TEST_RETURN__(x)	x
#endif

#define	TEST_REPORTER__		unittest::Controller::instance().getReporter()

namespace unittest
{
	// ------------------------------------------------------------------------
	bool
	checkExpression(bool expr, unsigned int line);
	
	// ------------------------------------------------------------------------
	bool
	checkEqual(const float& a, const float& b, unsigned int line);
	
	// ------------------------------------------------------------------------
	template <typename A, typename B>
	bool
	checkEqual(const A& a, const B& b, unsigned int line)
	{
		if (a == b) {
			TEST_REPORTER__.reportPass();
			return true;
		}
		else {
			TEST_REPORTER__.reportFailure(line)
				<< a << xpcc::accessor::asFlash(unittest::stringEqual) << b << '\n';
			return false;
		}
	}
	
	template <typename A, typename B, typename D>
	bool
	checkEqualDelta(const A& a, const B& b, const D& delta, unsigned int line)
	{
		if (((a + delta) >= b) and ((a - delta) <= b))
		{
			TEST_REPORTER__.reportPass();
			return true;
		}
		else {
			TEST_REPORTER__.reportFailure(line)
				<< a << xpcc::accessor::asFlash(unittest::stringEqual) << b << '\n';
			return false;
		}
	}
	
	template <typename T, typename B>
	bool
	checkRange(const T& value, const B& lower, const B& upper, unsigned int line)
	{
		if ((value >= lower) && (value <= upper))
		{
			TEST_REPORTER__.reportPass();
			return true;
		}
		else {
			TEST_REPORTER__.reportFailure(line)
				<< value << xpcc::accessor::asFlash(unittest::stringNotInRange)
				<< '[' << lower << ',' << upper << ']' << '\n';
			return false;
		}
	}
	
	// ------------------------------------------------------------------------
	template <typename T>
	inline void
	printArray(xpcc::IOStream& stream, unsigned int start, unsigned int count, const T& array)
	{
		stream << '[';
		for (unsigned int i = start; i < (start + count); ++i) {
			stream << array[i];
			if (i != start + count - 1) {
				stream << ',' << ' ';
			}
		}
		stream << ']' << '\n';
	}
	
	template <typename A, typename B>
	bool
	checkArray(const A& a, const B& b, unsigned int line, std::size_t count, std::size_t start = 0)
	{
		for (std::size_t i = start; i < (start + count); ++i)
		{
			if (a[i] != b[i])
			{
				xpcc::IOStream& stream = TEST_REPORTER__.reportFailure(line);
				
				stream << '\n';
				
				printArray(stream, start, count, a);
				printArray(stream, start, count, b);
				return false;
			}
		}
		
		TEST_REPORTER__.reportPass();
		return true;
	}
}

#define	TEST_ASSERT_TRUE(expr)	\
	TEST_RETURN__(::unittest::checkExpression((expr), __LINE__))

#define	TEST_ASSERT_FALSE(expr)	\
	TEST_RETURN__(::unittest::checkExpression(!static_cast<bool>(expr), __LINE__))

#define	TEST_ASSERT_EQUALS(x, y) \
	TEST_RETURN__(::unittest::checkEqual((x), (y), __LINE__))

#define	TEST_ASSERT_EQUALS_FLOAT(x, y) \
	TEST_RETURN__(::unittest::checkEqual(static_cast<float>(x), static_cast<float>(y), __LINE__))

#define	TEST_ASSERT_EQUALS_DELTA(x, y, d) \
	TEST_RETURN__(::unittest::checkEqualDelta((x), (y), (d), __LINE__))

#define	TEST_ASSERT_EQUALS_RANGE(value, lower, upper) \
	TEST_RETURN__(::unittest::checkRange((value), (lower), (upper), __LINE__))

#define	TEST_ASSERT_EQUALS_ARRAY(x, y, ...) \
	TEST_RETURN__(::unittest::checkArray((x), (y), __LINE__, __VA_ARGS__))

#define	TEST_FAIL(msg) \
	do {	TEST_REPORTER__.reportFailure(__LINE__) \
			<< msg << '\n'; \
	} while (0); \
	TEST_RETURN__(false)

#endif	// __DOXYGEN__

#endif	// UNITTEST__HARNESS_HPP
