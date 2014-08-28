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
	if (!this->nestingOkNPt()) return this->NestingError; \
	bool nptYielded ATTRIBUTE_UNUSED = true; \
	xpcc::pt::State nptState ATTRIBUTE_UNUSED = this->Stop; \
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
	bool nptYielded ATTRIBUTE_UNUSED = true; \
	xpcc::pt::State nptState ATTRIBUTE_UNUSED = this->Stop; \
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
	return this->Stop;

/**
* Yield protothread task till next call to its `runTask()`.
*
* \ingroup	protothread
* \hideinitializer
*/
#define NPT_YIELD() \
	do { \
		nptYielded = false; \
		this->setNPt(__LINE__); \
		case __LINE__: \
			if (!nptYielded) { \
				this->popNPt(); \
				return this->Running; \
			} \
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
				return this->Running; \
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
				return this->Starting; \
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
			return this->Successful; \
		} \
	} while (0)

/**
 * Spawns a given compound task with optional arguments
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_SPAWN(task) \
	NPT_WAIT_WHILE(task >= this->Starting)

/**
 * Spawns a given compound task with optional arguments, and returns
 * whether the task completed successfully or not.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_SPAWN_SUCCESS(task) \
	({ \
		this->setNPt(__LINE__); \
		case __LINE__: \
			nptState = task; \
			if (nptState > this->Successful) { \
				this->popNPt(); \
				return this->Running; \
			} \
			(nptState == this->Successful); \
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
		return this->Stop; \
	} while (0)

#endif // XPCC_NPT_MACROS_HPP
