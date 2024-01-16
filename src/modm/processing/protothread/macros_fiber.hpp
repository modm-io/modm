/*
 * Copyright (c) 2011, 2018, Fabian Greif
 * Copyright (c) 2012, 2014-2015, 2018, 2023, Niklas Hauser
 * Copyright (c) 2017, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PT_MACROS_FIBERS_HPP
#define MODM_PT_MACROS_FIBERS_HPP

#include <modm/architecture/utils.hpp>

/// @ingroup modm_processing_protothread
/// @{

/**
 * Declare start of protothread
 *
 * \warning	Use at start of the run() implementation!
 * \hideinitializer
 */
#define PT_BEGIN()

/**
 * Stop protothread and end it
 *
 * \warning	Use at end of the run() implementation!
 * \hideinitializer
 */
#define PT_END() \
	return false;

/// Yield protothread till next call to its run().
/// \hideinitializer
#define PT_YIELD() \
	modm::this_fiber::yield()

/// Cause protothread to wait **while** given condition is true.
/// \hideinitializer
#define PT_WAIT_WHILE(...) \
	while(__VA_ARGS__) { PT_YIELD(); }

/// Cause protothread to wait **until** given condition is true.
/// \hideinitializer
#define PT_WAIT_UNTIL(...) \
	PT_WAIT_WHILE(!(__VA_ARGS__))

/// Cause protothread to wait until given child protothread completes.
/// \hideinitializer
#define PT_WAIT_THREAD(...) \
	PT_WAIT_WHILE((__VA_ARGS__).run())

/// Restart and spawn given child protothread and wait until it completes.
/// \hideinitializer
#define PT_SPAWN(...) \
	PT_WAIT_THREAD(__VA_ARGS__);


/**
 * Calls a given resumable function and returns
 * whether it completed successfully or not.
 * \hideinitializer
 */
#define PT_CALL(...) \
	__VA_ARGS__

/**
 * Reset protothread to start from the beginning
 *
 * In the next executing cycle the protothread will restart its execution at
 * its PT_BEGIN.
 * \hideinitializer
 */
#define PT_RESTART() \
	return true;

/// Stop and exit from protothread.
/// \hideinitializer
#define PT_EXIT() \
	return false;

/// @}

#endif // MODM_PT_MACROS_FIBERS_HPP
