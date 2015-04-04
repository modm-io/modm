// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2010, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PT__MACROS_HPP
#define XPCC_PT__MACROS_HPP

#include <xpcc/processing/coroutine.hpp>

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
 * Calls a given coroutine and returns
 * whether it completed successfully or not.
 *
 * \ingroup	protothread
 * \hideinitializer
 */
#define PT_CALL(coroutine) \
	({ \
		this->ptState = __LINE__; \
		case __LINE__: \
			auto coResult = coroutine; \
			if (coResult.getState() > xpcc::co::NestingError) { \
				return true; \
			} \
			coResult.getResult(); \
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

#endif // XPCC_PT__MACROS_HPP
