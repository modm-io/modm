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
 * Declare start of resumable function with index.
 *
 * @warning	Use at start of the `resumable()` implementation!
 * @ingroup	resumable
 */
#define CO_BEGIN(index)

/**
 * Declare start of a nested resumable function.
 * This will immidiately return if the nesting is too deep.
 *
 * @warning	Use at start of the `resumable()` implementation!
 * @ingroup	resumable
 */
#define CO_BEGIN()
#endif

/**
 * End the resumable function and return a result.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_END_RETURN(result) \
			CO_RETURN(result); \
		default: \
			this->popCo(); \
			return {xpcc::co::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - coCounter < 256, "You have too many states in this resumable function!");

/**
 * End the resumable function. You can use this to return `void`, or if the result does not matter.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_END() \
 			this->stopCo(coIndex); \
			this->popCo(); \
			return {xpcc::co::Stop}; \
	default: \
			this->popCo(); \
			return {xpcc::co::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - coCounter < 256, "You have too many states in this resumable function!");

/**
 * End the resumable function by calling another resumable function and returning its result.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_END_RETURN_CALL(resumable) \
			CO_RETURN_CALL(resumable); \
		default: \
			this->popCo(); \
			return {xpcc::co::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - coCounter < 256, "You have too many states in this resumable function!");

/**
 * Yield resumable function until next invocation.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_YIELD() \
			CO_INTERNAL_SET_CASE_YIELD(__COUNTER__)

/**
 * Cause resumable function to wait until given child protothread completes.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_WAIT_THREAD(child) 	CO_WAIT_UNTIL(!(child).run())

/**
 * Cause resumable function to wait **while** given `condition` is true.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_WAIT_WHILE(condition) \
			CO_INTERNAL_SET_CASE(__COUNTER__); \
			if (condition) { \
				this->popCo(); \
				return {xpcc::co::Running}; \
			} \

/**
 * Cause resumable function to wait **until** given `condition` is true.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_WAIT_UNTIL(condition) \
	CO_WAIT_WHILE(!(condition))

/**
 * Calls a resumable function and returns its result.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_CALL(resumable) \
	({ \
			CO_INTERNAL_SET_CASE(__COUNTER__); \
			auto coResult = resumable; \
			if (coResult.getState() > xpcc::co::NestingError) { \
				this->popCo(); \
				return {xpcc::co::Running}; \
			} \
			coResult.getResult(); \
	})

/**
 * Calls a resumable function, busy-waits and returns its result.
 *
 * @warning	Use this with extreme caution, this can cause deadlocks!
 * @ingroup	resumable
 * @hideinitializer
 */
#define CO_CALL_BLOCKING(resumable) \
	({ \
			auto coResult = resumable; \
			while (coResult.getState() > xpcc::co::NestingError) \
			{ coResult = resumable; } \
			coResult.getResult(); \
	})

/**
* Exits a resumable function and returns another resumable function's result.
*
* @ingroup	resumable
* @hideinitializer
*/
#define CO_RETURN_CALL(resumable) \
		{ \
			CO_INTERNAL_SET_CASE(__COUNTER__); \
			{ \
				auto coResult = resumable; \
				if (coResult.getState() > xpcc::co::NestingError) { \
					this->popCo(); \
					return {xpcc::co::Running}; \
				} \
				CO_RETURN(coResult.getResult()); \
			} \
		}

/**
 * Stop and exit from resumable function.
 *
 * @ingroup	resumable
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

/// Beginner structure for nested resumable functions
#define CO_BEGIN_1() \
	constexpr uint16_t coCounter = __COUNTER__; \
	this->template checkCoType<true>(); \
	constexpr uint8_t coIndex = 0; \
	if (!this->nestingOkCo()) return {xpcc::co::NestingError}; \
	switch (this->pushCo(0)) { \
		case (::xpcc::co::CoStopped): \
			CO_INTERNAL_SET_CASE(__COUNTER__);

/// Beginner structure for conventional resumable functions
#define CO_BEGIN_0(index) \
	constexpr uint16_t coCounter = __COUNTER__; \
	this->template checkCoMethods<index>(); \
	this->template checkCoType<false>(); \
	constexpr uint_fast8_t coIndex = index; \
	switch (this->pushCo(index)) { \
		case (::xpcc::co::CoStopped): \
			CO_INTERNAL_SET_CASE(__COUNTER__);

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
