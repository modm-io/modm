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
	if (!this->nestingOkCo()) return {xpcc::co::NestingError, 0}; \
	if (!this->beginCo(context)) return {xpcc::co::WrongContext, 0}; \
	switch (this->pushCo()) { \
		case CoStopped: \
			this->setCo(__LINE__); \
		case __LINE__: ;

/**
 * End the coroutine.
 *
 * @warning	Use at end of the `coroutine(ctx)` implementation!
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_END() \
			this->stopCo(); \
		default: \
			this->popCo(); \
			if (this->isStoppedCo()) return {xpcc::co::Stop, 0}; else return {xpcc::co::WrongState, 0}; \
	}

/**
 * Yield coroutine until next invocation.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_YIELD() \
			this->setCo(__LINE__); \
			this->popCo(); \
			return {xpcc::co::Running, 0}; \
		case __LINE__: ;

/**
 * Cause coroutine to wait **while** given `condition` is true.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_WAIT_WHILE(condition) \
			this->setCo(__LINE__); \
		case __LINE__: \
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
 * Calls a coroutine and returns whether it completed successfully or not.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_CALL(coroutine) \
	({ \
			this->setCo(__LINE__); \
		case __LINE__:\
			auto coResult = coroutine; \
			if (coResult.state > xpcc::co::NestingError) { \
				this->popCo(); \
				return {xpcc::co::Running, 0}; \
			} \
			coResult.result; \
	})

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
			return {xpcc::co::Stop, result}; \
	}

#endif // XPCC_CO_MACROS_HPP
