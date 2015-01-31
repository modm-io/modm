// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_CO_MACROS_HPP
#define XPCC_CO_MACROS_HPP

#include <xpcc/utils/arithmetic_traits.hpp>

#ifdef __DOXYGEN__
/**
 * Declare start of coroutine with index.
 *
 * @warning	Use at start of the `coroutine()` implementation!
 * @ingroup	coroutine
 */
#define CO_BEGIN(index)

/**
 * Declare start of a nested coroutine.
 * This will immidiately return if the nesting is too deep/
 *
 * @warning	Use at start of the `coroutine()` implementation!
 * @ingroup	coroutine
 */
#define CO_BEGIN()
#endif

/**
 * End the coroutine and return a result.
 *
 * @warning	Use at end of the `coroutine()` implementation only!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END_RETURN(result) \
			CO_RETURN(result); \
		default: \
			this->popCo(); \
			return {xpcc::co::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - coCounter < 256, "You have too many states in this coroutine!");

/**
 * End the coroutine. You can use this to return `void`, or if the result does not matter.
 *
 * @warning	Use at end of the `coroutine()` implementation only!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END() \
	CO_END_RETURN(0)

/**
 * End the coroutine by calling another coroutine and returning its result.
 *
 * @warning	Use at end of the `coroutine()` implementation only!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END_RETURN_CALL(coroutine) \
			CO_RETURN_CALL(coroutine); \
		default: \
			this->popCo(); \
			return {xpcc::co::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - coCounter < 256, "You have too many states in this coroutine!");

/**
 * Yield coroutine until next invocation.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_YIELD() \
			CO_INTERNAL_SET_CASE_YIELD(__COUNTER__)

/**
 * Cause coroutine to wait **while** given `condition` is true.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_WAIT_WHILE(condition) \
			CO_INTERNAL_SET_CASE(__COUNTER__); \
			if (condition) { \
				this->popCo(); \
				return {xpcc::co::Running}; \
			} \

/**
 * Cause coroutine to wait **until** given `condition` is true.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_WAIT_UNTIL(condition) \
	CO_WAIT_WHILE(!(condition))

/**
 * Calls a coroutine and returns its result.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_CALL(coroutine) \
	({ \
			CO_INTERNAL_SET_CASE(__COUNTER__); \
			auto coResult = coroutine; \
			if (coResult.state > xpcc::co::NestingError) { \
				this->popCo(); \
				return {xpcc::co::Running}; \
			} \
			coResult.result; \
	})

/**
 * Calls a coroutine, busy-waits and returns its result.
 *
 * @warning	Use this with extreme caution, this can cause deadlocks!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_CALL_BLOCKING(coroutine) \
	({ \
			auto coResult = coroutine; \
			while (coResult.state > xpcc::co::NestingError) \
			{ coResult = coroutine; } \
			coResult.result; \
	})

/**
* Exits a coroutine and returns another coroutine's result.
*
* @ingroup	coroutine
* @hideinitializer
*/
#define CO_RETURN_CALL(coroutine) \
		{ \
			CO_INTERNAL_SET_CASE(__COUNTER__); \
			{ \
				auto coResult = coroutine; \
				if (coResult.state > xpcc::co::NestingError) { \
					this->popCo(); \
					return {xpcc::co::Running}; \
				} \
				CO_RETURN(coResult.result); \
			} \
		}

/**
 * Stop and exit from coroutine.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_RETURN(result) \
	{ \
			this->stopCo(coIndex); \
			this->popCo(); \
			return {xpcc::co::Stop, (result)}; \
	}


#ifndef __DOXYGEN__
/// Required macro to set the same unique number twice
#define CO_INTERNAL_SET_CASE(counter) \
			this->setCo((counter % 255) + 1, coIndex); \
		case ((counter % 255) + 1): ;

/// Internal macro for yield
#define CO_INTERNAL_SET_CASE_YIELD(counter) \
			this->setCo((counter % 255) + 1, coIndex); \
			this->popCo(); \
			return {xpcc::co::Running}; \
		case ((counter % 255) + 1): ;

/// Beginner structure for nested coroutines
#define CO_BEGIN_1() \
	constexpr uint16_t coCounter = __COUNTER__; \
	constexpr uint8_t coIndex = 0; \
	if (!this->nestingOkCo()) return {xpcc::co::NestingError}; \
	switch (this->pushCo()) { \
		case (::xpcc::co::CoStopped): \
			CO_INTERNAL_SET_CASE(__COUNTER__);

/// Beginner structure for conventional coroutines
#define CO_BEGIN_0(index) \
	constexpr uint16_t coCounter = __COUNTER__; \
	constexpr uint_fast8_t coIndex = index; \
	switch (this->pushCo(index)) { \
		case (::xpcc::co::CoStopped): \
			CO_INTERNAL_SET_CASE(__COUNTER__);

// These static_asserts would be very useful, but we can only access
// inherited member variables from template classes using `this->variable`,
// which is not allowed in a `static_assert`.
// Maybe this will change in the future. clang++ can already deal with accessing
// inherited members without using `this->`.
// Note that these asserts require static constexpr `coIsNested` and `coMethods`
// in the Coroutine and NestedCoroutine classes respectively.

// this should go into CO_BEGIN_1
//	static_assert(coIsNested == true,
//			"You must declare the index for this method!");

// this should go into CO_BEGIN_0
//	static_assert(coIsNested == false,
//			"You must _not_ declare an index for this method!");
//	static_assert(index < coMethods,
//			"Index out of bounds! Increase the number of methods of your Coroutine class.");

// the following completely unreadable preprocessor macro magic is based on this
// https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
// http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments/#comment-255
#define CO_ARG3(_0, _1, _2, ...) _2
#define CO_HAS_COMMA(...) CO_ARG3(__VA_ARGS__, 1, 0)
#define CO_TRIGGER_PARENTHESIS_(...) ,

#define CO_ISEMPTY(...) _CO_ISEMPTY( \
			/* test if there is just one argument, eventually an empty one */ \
			CO_HAS_COMMA(__VA_ARGS__), \
			/* test if _TRIGGER_PARENTHESIS_ together with the argument adds a comma */ \
			CO_HAS_COMMA(CO_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
			/* test if the argument together with a parenthesis adds a comma */ \
			CO_HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
			/* test if placing it between _TRIGGER_PARENTHESIS_ and the parenthesis adds a comma */ \
			CO_HAS_COMMA(CO_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
			)

#define CO_PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define _CO_ISEMPTY(_0, _1, _2, _3) CO_HAS_COMMA(CO_PASTE5(CO_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define CO_IS_EMPTY_CASE_0001 ,

// all we wanted is to call CO_BEGIN_0 for 1 argument
// and call CO_BEGIN_1 for 0 arguments. Makes total sense.
#define CO_GET_MACRO3(n) CO_BEGIN_ ## n
#define CO_GET_MACRO2(n) CO_GET_MACRO3(n)
#define CO_GET_MACRO(...) CO_GET_MACRO2(CO_ISEMPTY(__VA_ARGS__))
#define CO_BEGIN(...) CO_GET_MACRO(__VA_ARGS__)(__VA_ARGS__)

#endif

#endif // XPCC_CO_MACROS_HPP
