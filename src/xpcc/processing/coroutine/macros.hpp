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

/// @internal Required macro to set the same unique number twice
/// @hideinitializer
#define CO_INTERNAL_SET_CASE(counter) \
			this->setCo((counter % 255) + 1); \
		case ((counter % 255) + 1): ;

/// @internal Internal macro for yield
/// @hideinitializer
#define CO_INTERNAL_SET_CASE_YIELD(counter) \
			this->setCo((counter % 255) + 1); \
			this->popCo(); \
			return {xpcc::co::Running, 0}; \
		case ((counter % 255) + 1): ;

/**
 * Declare start of a coroutine.
 * This will immidiately return if the nesting is too deep
 * or if it was called in a different context than it was started in.
 *
 * @warning	Use at start of the `coroutine(ctx)` implementation!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_BEGIN(context) \
	constexpr uint32_t coCounter = __COUNTER__; \
	if (!this->nestingOkCo()) return {xpcc::co::NestingError, 0}; \
	if (!this->beginCo(context)) return {xpcc::co::WrongContext, 0}; \
	switch (this->pushCo()) { \
		case CoStopped: \
			CO_INTERNAL_SET_CASE(__COUNTER__);

/**
 * End the coroutine and return a result.
 *
 * @warning	Use at end of the `coroutine(ctx)` implementation only!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END_RETURN(result) \
			CO_RETURN(result); \
		default: \
			this->popCo(); \
			return {xpcc::co::WrongState, 0}; \
	} \
	static_assert(__COUNTER__ - coCounter < 256, "You have too many states in this coroutine!");

/**
 * End the coroutine. You can use this to return `void`, or if the result does not matter.
 *
 * @warning	Use at end of the `coroutine(ctx)` implementation only!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END() \
	CO_END_RETURN(0)

/**
 * End the coroutine by calling another coroutine and returning its result.
 *
 * @warning	Use at end of the `coroutine(ctx)` implementation only!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END_RETURN_CALL(coroutine) \
			CO_RETURN_CALL(coroutine); \
		default: \
			this->popCo(); \
			return {xpcc::co::WrongState, 0}; \
	} \
	static_assert(__COUNTER__ - coCounter < 256, "You have too many states in this coroutine!");

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
				return {xpcc::co::Running, 0}; \
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
				return {xpcc::co::Running, 0}; \
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
					return {xpcc::co::Running, 0}; \
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
			this->stopCo(); \
			this->popCo(); \
			return {xpcc::co::Stop, (result)}; \
	}

#endif // XPCC_CO_MACROS_HPP
