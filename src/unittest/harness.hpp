/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2016, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	UNITTEST_HARNESS_HPP
#define	UNITTEST_HARNESS_HPP

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

#include <modm/architecture/driver/accessor/flash.hpp>

namespace unittest
{
	EXTERN_FLASH_STORAGE_STRING(stringEqual);
	EXTERN_FLASH_STORAGE_STRING(stringDiffer);
	EXTERN_FLASH_STORAGE_STRING(stringNotInRange);
	EXTERN_FLASH_STORAGE_STRING(stringNotTrue);
	EXTERN_FLASH_STORAGE_STRING(stringNotFalse);
}

#ifdef	UNITTEST_RETURN_ON_FAIL
	#define	TEST_RETURN_(x)	do { if (!x) { return; } } while (0)
#else
	#define	TEST_RETURN_(x)	x
#endif

#define	TEST_REPORTER_		unittest::Controller::instance().getReporter()

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
	inline bool
	checkEqual(const A& a, const B& b, unsigned int line)
	{
		if (a == b) {
			TEST_REPORTER_.reportPass();
			return true;
		}
		else {
			TEST_REPORTER_.reportFailure(line)
				<< a << modm::accessor::asFlash(unittest::stringEqual) << b << '\n';
			return false;
		}
	}
	
	// ------------------------------------------------------------------------
	template <typename A, typename B>
	inline bool
	checkDiffer(const A& a, const B& b, unsigned int line)
	{
		if (a != b) {
			TEST_REPORTER_.reportPass();
			return true;
		}
		else {
			TEST_REPORTER_.reportFailure(line)
				<< a << modm::accessor::asFlash(unittest::stringDiffer) << b << '\n';
			return false;
		}
	}

	template <typename A, typename B, typename D>
	bool
	checkEqualDelta(const A& a, const B& b, const D& delta, unsigned int line)
	{
		if (((a + delta) >= b) and ((a - delta) <= b))
		{
			TEST_REPORTER_.reportPass();
			return true;
		}
		else {
			TEST_REPORTER_.reportFailure(line)
				<< a << modm::accessor::asFlash(unittest::stringEqual) << b << '\n';
			return false;
		}
	}
	
	template <typename T, typename B>
	bool
	checkRange(const T& value, const B& lower, const B& upper, unsigned int line)
	{
		if ((value >= lower) && (value <= upper))
		{
			TEST_REPORTER_.reportPass();
			return true;
		}
		else {
			TEST_REPORTER_.reportFailure(line)
				<< value << modm::accessor::asFlash(unittest::stringNotInRange)
				<< '[' << lower << ',' << upper << ']' << '\n';
			return false;
		}
	}

	// ------------------------------------------------------------------------
	inline void
	printString(modm::IOStream& stream, const char* a, const char* b, size_t pos)
	{
		stream << modm::endl << a << modm::endl << b << modm::endl;
		for(size_t ii = 0; ii < pos; ++ii) { stream << " "; }
		stream << "^" << modm::endl;
	}

	inline bool
	checkString(const char* a, const char* b, unsigned int line)
	{
		size_t ii = 0;
		while(a[ii] != '\0' && b[ii] != '\0') {
			if (a[ii] != b[ii]) {
				modm::IOStream& stream = TEST_REPORTER_.reportFailure(line);
				printString(stream, a, b, ii);
				return false;
			}
			++ii;
		}
		// strings are not of equal length
		if(a[ii] != '\0' || b[ii] != '\0') {
			modm::IOStream& stream = TEST_REPORTER_.reportFailure(line);
			printString(stream, a, b, ii);
			return false;
		}
		TEST_REPORTER_.reportPass();
		return true;
	}

	// ------------------------------------------------------------------------
	template <typename T>
	inline void
	printArray(modm::IOStream& stream, unsigned int start, unsigned int count, const T& array)
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
				modm::IOStream& stream = TEST_REPORTER_.reportFailure(line);
				
				stream << '\n';
				
				printArray(stream, start, count, a);
				printArray(stream, start, count, b);
				return false;
			}
		}
		
		TEST_REPORTER_.reportPass();
		return true;
	}
}

#define	TEST_ASSERT_TRUE(expr)	\
	TEST_RETURN_(::unittest::checkExpression((expr), __LINE__))

#define	TEST_ASSERT_FALSE(expr)	\
	TEST_RETURN_(::unittest::checkExpression(!static_cast<bool>(expr), __LINE__))

#define	TEST_ASSERT_EQUALS(x, y) \
	TEST_RETURN_(::unittest::checkEqual((x), (y), __LINE__))

#define TEST_ASSERT_DIFFERS(x, y) \
	TEST_RETURN_(::unittest::checkDiffer((x), (y), __LINE__))

#define	TEST_ASSERT_EQUALS_FLOAT(x, y) \
	TEST_RETURN_(::unittest::checkEqual(static_cast<float>(x), static_cast<float>(y), __LINE__))

#define	TEST_ASSERT_EQUALS_DELTA(x, y, d) \
	TEST_RETURN_(::unittest::checkEqualDelta((x), (y), (d), __LINE__))

#define	TEST_ASSERT_EQUALS_RANGE(value, lower, upper) \
	TEST_RETURN_(::unittest::checkRange((value), (lower), (upper), __LINE__))

#define	TEST_ASSERT_EQUALS_STRING(x, y) \
	TEST_RETURN_(::unittest::checkString((x), (y), __LINE__))

#define	TEST_ASSERT_EQUALS_ARRAY(x, y, ...) \
	TEST_RETURN_(::unittest::checkArray((x), (y), __LINE__, __VA_ARGS__))

#define	TEST_FAIL(msg) \
	do {	TEST_REPORTER_.reportFailure(__LINE__) \
			<< msg << '\n'; \
	} while (0); \
	TEST_RETURN_((void) false)

#endif	// __DOXYGEN__

#endif	// UNITTEST_HARNESS_HPP
