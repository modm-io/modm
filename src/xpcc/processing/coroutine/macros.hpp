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
	if (!this->nestingOkCo()) return xpcc::co::NestingError; \
	if (!this->beginCo(context)) return xpcc::co::WrongContext; \
	xpcc::co::Result coResult ATTRIBUTE_UNUSED = xpcc::co::Stop; \
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
			return this->isStoppedCo() ? xpcc::co::Stop : xpcc::co::WrongState; \
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
			return xpcc::co::Running; \
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
				return xpcc::co::Running; \
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
		case __LINE__: \
			coResult = coroutine; \
			if (coResult > xpcc::co::Success) { \
				this->popCo(); \
				return xpcc::co::Running; \
			} \
			(coResult == xpcc::co::Success); \
	})

/**
 * Stop and exit from coroutine successfully.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_EXIT_SUCCESS() \
	{ \
			this->stopCo(); \
			this->popCo(); \
			return xpcc::co::Success; \
	}

/**
 * Stop and exit from coroutine.
 *
 * @ingroup	coroutine
 * @hideinitializer
 */
#define CO_EXIT() \
	{ \
			this->stopCo(); \
			this->popCo(); \
			return xpcc::co::Stop; \
	}

#endif // XPCC_CO_MACROS_HPP
