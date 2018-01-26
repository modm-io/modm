/*
 * Copyright (c) 2011, 2018, Fabian Greif
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2017, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PT_MACROS_HPP
#define MODM_PT_MACROS_HPP

#include <modm/architecture/utils.hpp>

/**
 * Declare start of protothread
 *
 * \warning	Use at start of the run() implementation!
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_BEGIN() \
	switch (this->ptState) { \
		case 0:

/**
 * Stop protothread and end it
 *
 * \warning	Use at end of the run() implementation!
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_END() \
		default: ; \
	} \
	this->stop(); \
	return false;

/**
 * Yield protothread till next call to its run().
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_YIELD() \
    do { \
		this->ptState = __LINE__; \
		return true; \
		case __LINE__: ; \
	} while (0)

/**
 * Cause protothread to wait **while** given condition is true.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_WAIT_WHILE(condition) \
    do { \
		this->ptState = __LINE__; \
		MODM_FALLTHROUGH; \
		case __LINE__: \
			if (condition) \
				return true; \
    } while (0)

/**
 * Cause protothread to wait **until** given condition is true.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_WAIT_UNTIL(condition) \
	PT_WAIT_WHILE(!(condition))

/**
 * Cause protothread to wait until given child protothread completes.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_WAIT_THREAD(child) 	PT_WAIT_UNTIL(!(child).run())

/**
 * Restart and spawn given child protothread and wait until it completes.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_SPAWN(child) \
    do { \
		(child).restart(); \
		PT_WAIT_THREAD(child); \
    } while (0)


/**
 * Calls a given resumable function and returns
 * whether it completed successfully or not.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_CALL(resumable) \
	({ \
		this->ptState = __LINE__; \
		MODM_FALLTHROUGH; \
		case __LINE__: \
			auto rfResult = resumable; \
			if (rfResult.getState() > modm::rf::NestingError) { \
				return true; \
			} \
			rfResult.getResult(); \
	})

/**
 * Reset protothread to start from the beginning
 *
 * In the next executing cycle the protothread will restart its execution at
 * its PT_BEGIN.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_RESTART() \
	do { \
		this->restart(); \
		return true; \
	} while (0)

/**
 * Stop and exit from protothread.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_EXIT() \
	do { \
		this->stop(); \
		return false; \
	} while (0)

#endif // MODM_PT_MACROS_HPP
