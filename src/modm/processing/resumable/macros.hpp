/*
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014-2015, 2017, Niklas Hauser
 * Copyright (c) 2017, Raphael Lehmann
 * Copyright (c) 2018, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_RF_MACROS_HPP
#define MODM_RF_MACROS_HPP

#include <modm/architecture/interface/assert.hpp>
#include <modm/architecture/utils.hpp>

#ifdef __DOXYGEN__
/**
 * Declare start of resumable function with index.
 *
 * @warning	Use at start of the `resumable()` implementation!
 * @ingroup	resumable
 */
#define RF_BEGIN(index)

/**
 * Declare start of a nested resumable function.
 * This will immidiately return if the nesting is too deep.
 *
 * @warning	Use at start of the `resumable()` implementation!
 * @ingroup	resumable
 */
#define RF_BEGIN()
#endif

/**
 * End the resumable function and return a result.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_END_RETURN(result) \
			RF_RETURN(result); \
			MODM_FALLTHROUGH; \
		default: \
			this->popRf(); \
			return {modm::rf::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - rfCounter < 256, "You have too many states in this resumable function!")

/**
 * End the resumable function. You can use this to return `void`, or if the result does not matter.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_END() \
 			this->stopRf(rfIndex); \
			this->popRf(); \
			return {modm::rf::Stop}; \
	default: \
			this->popRf(); \
			return {modm::rf::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - rfCounter < 256, "You have too many states in this resumable function!")

/**
 * End the resumable function by calling another resumable function and returning its result.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_END_RETURN_CALL(resumable) \
			RF_RETURN_CALL(resumable); \
			MODM_FALLTHROUGH; \
		default: \
			this->popRf(); \
			return {modm::rf::WrongState}; \
	} \
	static_assert(uint16_t(__COUNTER__) - rfCounter < 256, "You have too many states in this resumable function!")

/**
 * Yield resumable function until next invocation.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_YIELD() \
			RF_INTERNAL_SET_CASE_YIELD(__COUNTER__)

/**
 * Cause resumable function to wait until given child protothread completes.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_WAIT_THREAD(child) 	RF_WAIT_UNTIL(!(child).run())

/**
 * Cause resumable function to wait **while** given `condition` is true.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_WAIT_WHILE(condition) \
		do { \
			RF_INTERNAL_SET_CASE(__COUNTER__); \
			if (condition) { \
				this->popRf(); \
				return {modm::rf::Running}; \
			} \
		} while(0)

/**
 * Cause resumable function to wait **until** given `condition` is true.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_WAIT_UNTIL(condition) \
	RF_WAIT_WHILE(!(condition))

/**
 * Calls a resumable function and returns its result.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_CALL(resumable) \
	({ \
			RF_INTERNAL_SET_CASE(__COUNTER__); \
			auto rfResult = resumable; \
			if (rfResult.getState() > modm::rf::NestingError) { \
				this->popRf(); \
				return {modm::rf::Running}; \
			} \
			rfResult.getResult(); \
	})

/**
 * Calls a resumable function, busy-waits and returns its result.
 *
 * @warning	Use this with extreme caution, this can cause deadlocks!
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_CALL_BLOCKING(resumable) \
	({ \
			auto rfResult = resumable; \
			while (rfResult.getState() > modm::rf::NestingError) \
			{ rfResult = resumable; } \
			rfResult.getResult(); \
	})

/**
* Exits a resumable function and returns another resumable function's result.
*
* @ingroup	resumable
* @hideinitializer
*/
#define RF_RETURN_CALL(resumable) \
		do { \
			RF_INTERNAL_SET_CASE(__COUNTER__); \
			{ \
				auto rfResult = resumable; \
				if (rfResult.getState() > modm::rf::NestingError) { \
					this->popRf(); \
					return {modm::rf::Running}; \
				} \
				RF_RETURN(rfResult.getResult()); \
			} \
		} while(0)

#ifdef __DOXYGEN__
/**
 * Stop and exit from resumable function with a result.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_RETURN(result)

/**
 * Stop and exit from resumable function of `void` return type.
 *
 * @ingroup	resumable
 * @hideinitializer
 */
#define RF_RETURN()

#endif


#ifndef __DOXYGEN__
/// Required macro to set the same unique number twice
#define RF_INTERNAL_SET_CASE(counter) \
			this->setRf((counter % 255) + 1, rfIndex); \
			MODM_FALLTHROUGH; \
		case ((counter % 255) + 1): ;

/// Internal macro for yield
#define RF_INTERNAL_SET_CASE_YIELD(counter) \
			this->setRf((counter % 255) + 1, rfIndex); \
			this->popRf(); \
			return {modm::rf::Running}; \
		case ((counter % 255) + 1): ;

#define RF_RETURN_1() \
	do { \
			this->stopRf(rfIndex); \
			this->popRf(); \
			return {modm::rf::Stop}; \
	} while(0)

#define RF_RETURN_0(result) \
	do { \
			this->stopRf(rfIndex); \
			this->popRf(); \
			return {modm::rf::Stop, (result)}; \
	} while(0)

/// Beginner structure for nested resumable functions
#define RF_BEGIN_1() \
	constexpr uint16_t rfCounter = __COUNTER__; \
	this->template checkRfType<true>(); \
	constexpr uint8_t rfIndex = 0; \
	if (!this->nestingOkRf()) { \
		modm_assert(false, MODM_RESUMABLE_MODULE_NAME, "begin", "nesting", this); \
		return {modm::rf::NestingError}; \
	} \
	switch (this->pushRf(0)) { \
		case (::modm::rf::Stopped): \
			RF_INTERNAL_SET_CASE(__COUNTER__);

/// Beginner structure for conventional resumable functions
#define RF_BEGIN_0(index) \
	constexpr uint16_t rfCounter = __COUNTER__; \
	this->template checkRfFunctions<index>(); \
	this->template checkRfType<false>(); \
	constexpr uint_fast8_t rfIndex = index; \
	switch (this->pushRf(index)) { \
		case (::modm::rf::Stopped): \
			RF_INTERNAL_SET_CASE(__COUNTER__);

// the following completely unreadable preprocessor macro magic is based on this
// https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
// http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments/#comment-255
#define RF_ARG3(_0, _1, _2, ...) _2
#define RF_HAS_COMMA(...) RF_ARG3(__VA_ARGS__, 1, 0)
#define RF_TRIGGER_PARENTHESIS_(...) ,

#define RF_ISEMPTY(...) _RF_ISEMPTY( \
			/* test if there is just one argument, eventually an empty one */ \
			RF_HAS_COMMA(__VA_ARGS__), \
			/* test if _TRIGGER_PARENTHESIS_ together with the argument adds a comma */ \
			RF_HAS_COMMA(RF_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
			/* test if the argument together with a parenthesis adds a comma */ \
			RF_HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
			/* test if placing it between _TRIGGER_PARENTHESIS_ and the parenthesis adds a comma */ \
			RF_HAS_COMMA(RF_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
			)

#define RF_PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define _RF_ISEMPTY(_0, _1, _2, _3) RF_HAS_COMMA(RF_PASTE5(RF_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define RF_IS_EMPTY_CASE_0001 ,

// all we wanted is to call RF_BEGIN_0 for 1 argument
// and call RF_BEGIN_1 for 0 arguments. Makes total sense.
#define RF_GET_BEGIN_MACRO3(n) RF_BEGIN_ ## n
#define RF_GET_BEGIN_MACRO2(n) RF_GET_BEGIN_MACRO3(n)
#define RF_GET_BEGIN_MACRO(...) RF_GET_BEGIN_MACRO2(RF_ISEMPTY(__VA_ARGS__))
#define RF_BEGIN(...) RF_GET_BEGIN_MACRO(__VA_ARGS__)(__VA_ARGS__)

// all we wanted is to call RF_RETURN_0 for 1 argument
// and call RF_RETURN_1 for 0 arguments. Makes total sense.
#define RF_GET_RETURN_MACRO3(n) RF_RETURN_ ## n
#define RF_GET_RETURN_MACRO2(n) RF_GET_RETURN_MACRO3(n)
#define RF_GET_RETURN_MACRO(...) RF_GET_RETURN_MACRO2(RF_ISEMPTY(__VA_ARGS__))
#define RF_RETURN(...) RF_GET_RETURN_MACRO(__VA_ARGS__)(__VA_ARGS__)

#endif

#endif // MODM_RF_MACROS_HPP
