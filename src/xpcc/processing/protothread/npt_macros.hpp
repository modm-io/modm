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
 * Declare start of nested protoface task, with the task id.
 *
 * @warning	Use at start of the `runTask()` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_BEGIN(task) \
	bool nptYielded ATTRIBUTE_UNUSED = true; \
	switch (this->pushNPt()) { \
		case (xpcc::ArithmeticTraits<uint16_t>::max - 1 - task):

/**
 * Stop protoface task and end it.
 *
 * @warning	Use at end of the `runTask()` implementation!
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_END() \
		this->invalidateNPt(); \
		default: ; \
	} \
	this->popNPt(); \
	return false;

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
				return true; \
			} \
	} while (0)

/**
 * Cause protothread task to wait **until** given `condition` is true.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_UNTIL(condition) \
	do { \
		this->setNPt(__LINE__); \
		case __LINE__: \
			if (!(condition)) { \
				this->popNPt(); \
				return true; \
			} \
	} while (0)

/**
 * Cause protothread to wait **while** given `condition` is true.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_WAIT_WHILE(condition)		NPT_WAIT_UNTIL(!condition)

/**
 * Stop and exit from protothread task.
 *
 * @ingroup	protothread
 * @hideinitializer
 */
#define NPT_EXIT() \
	do { \
		this->invalidateNPt(); \
		this->popNPt(); \
		return false; \
	} while (0)

#endif // XPCC_NPT_MACROS_HPP
