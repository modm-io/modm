// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_NPT_MACROS_HPP
#define XPCC_NPT_MACROS_HPP

#include <xpcc/utils/arithmetic_traits.hpp>

/**
 * Declare start of a generic nested protothread task.
 * This will immidiately return if the nesting is too deep
 * or if it was called in a different context than it was started in.
 *
 * @warning	Use at start of the `task(ctx)` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_BEGIN(context) \
	if (!this->nestingOkNPt()) return xpcc::pt::NestingError; \
	if (!this->beginNPt(context)) return xpcc::pt::WrongContext; \
	xpcc::pt::Result nptResult ATTRIBUTE_UNUSED = xpcc::pt::Stop; \
	switch (this->pushNPt()) { \
		case NPtStopped: \
			this->setNPt(__LINE__); \
		case __LINE__: ;

/**
 * Stop protothread task and end it.
 *
 * @warning	Use at end of the `task(ctx)` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_END() \
			this->stopNPt(); \
			break; \
		default: \
			this->popNPt(); \
			return xpcc::pt::WrongState; \
	} \
	this->popNPt(); \
	return xpcc::pt::Stop;

/**
 * Yield protothread task till next call to its `runTask()`.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_YIELD() \
	do { \
			this->setNPt(__LINE__); \
			this->popNPt(); \
			return xpcc::pt::Running; \
		case __LINE__: ; \
	} while (0)

/**
 * Cause protothread task to wait **while** given `condition` is true.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_WHILE(condition) \
	do { \
			this->setNPt(__LINE__); \
		case __LINE__: \
			if (condition) { \
				this->popNPt(); \
				return xpcc::pt::Running; \
			} \
	} while (0)

/**
 * Cause protothread to wait **until** given `condition` is true.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_UNTIL(condition) \
	NPT_WAIT_WHILE(!(condition))

/**
 * Causes Protothread to wait **while** given `condition` is true.
 * Use this when waiting for a starting condition.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_WHILE_START(condition) \
	do { \
			this->setNPt(__LINE__); \
		case __LINE__: \
			if (condition) { \
				this->popNPt(); \
				return xpcc::pt::Starting; \
			} \
	} while (0)

/**
 * Causes protothread to wait **until** given `condition` is true.
 * Use this when waiting for a starting condition.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_UNTIL_START(condition) \
	NPT_WAIT_WHILE_START(!(condition))

/**
 * Causes protothread to stop successfully if given `condition` is true.
 * If the `condition` is not met, the program flow continues.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_SUCCESS_IF(condition) \
	do { \
			if (condition) { \
				this->stopNPt(); \
				this->popNPt(); \
				return xpcc::pt::Successful; \
			} \
	} while (0)

/**
 * Spawns a given task, and returns whether the task completed successfully or not.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_SPAWN(task) \
	({ \
			this->setNPt(__LINE__); \
		case __LINE__: \
			nptResult = task; \
			if (nptResult > xpcc::pt::Successful) { \
				this->popNPt(); \
				return xpcc::pt::Running; \
			} \
			(nptResult == xpcc::pt::Successful); \
	})

/**
 * Stop and exit from protothread task.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_EXIT() \
	do { \
			this->stopNPt(); \
			this->popNPt(); \
			return xpcc::pt::Stop; \
	} while (0)

#endif // XPCC_NPT_MACROS_HPP
