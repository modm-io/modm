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
 * This will immidiately return if the nesting is too deep.
 *
 * @warning	Use at start of the `runTask()` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_BEGIN() \
	if (!this->nestingOkNPt()) return xpcc::pt::NestingError; \
	xpcc::pt::Result nptResult ATTRIBUTE_UNUSED = xpcc::pt::Stop; \
	switch (this->pushNPt()) { \
		case NPtStopped: \
			this->setNPt(__LINE__); \
		case __LINE__: ;

/**
 * Declare start of a specific nested protothread task, with the task id.
 *
 * @warning	Use at start of the `runTask()` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_BEGIN_TASK(task) \
	xpcc::pt::Result nptResult ATTRIBUTE_UNUSED = xpcc::pt::Stop; \
	switch (this->pushNPt()) { \
		case stateFromTask(task): ;

/**
 * Stop protothread task and end it.
 *
 * @warning	Use at end of the `runTask()` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_END() \
			this->stopNPt(); \
		default: ; \
	} \
	this->popNPt(); \
	return xpcc::pt::Stop;

/**
* Yield protothread task till next call to its `runTask()`.
*
* \ingroup	protothread
* \hideinitializer
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
 * Causes Protothread to wait **until** given `condition` is true.
 * Use this when waiting for a starting condition.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_UNTIL_START(condition) \
	NPT_WAIT_WHILE_START(!(condition))

/**
 * Causes Protothread to stop successfully if given `condition` is true.
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
 * Spawns a given compound task, and returns
 * whether the task completed successfully or not.
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
