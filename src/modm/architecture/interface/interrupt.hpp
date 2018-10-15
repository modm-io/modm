/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERRUPT_HPP
#define MODM_INTERRUPT_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>

#ifdef __DOXYGEN__

/**
 * Get the expanded interrupt name.
 *
 * On AVRs this maps to `{vector}_vect`.
 * On Cortex-M this maps to `{vector}_IRQHandler`.
 * On Hosted this maps to `{vector}_isr`.
 * @warning These mappings are internal and can change without notice!
 *
 * @warning
 * The CMSIS header files may define a macro with an identical name as an
 * interrupt vector, such as `MODM_ISR_NAME(ADC)` vs. the `ADC` peripheral.
 * When defining interrupt vectors for compatibility, you MUST use this macro
 * instead of manually defining it!
 * @code
 * #define               ADC                  ADC_CAN_TX  // will not compile!
 * #define MODM_ISR_NAME(ADC)   MODM_ISR_NAME(ADC_CAN_TX) // will     compile
 * MODM_ISR(ADC) {...}
 * @endcode
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 *
 * @ingroup	modm_architecture_interrupt
 */
#define MODM_ISR_NAME(vector)

/**
 * Forward declare in interrupt function.
 *
 * This maps to `extern void MODM_ISR_NAME({vector})(void)`.
 * `extern "C"` is used automatically in a C++ environment.
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 *
 * @ingroup	modm_architecture_interrupt
 */
#define MODM_ISR_DECL(vector)

/**
 * Directly calls an interrupt function.
 *
 * This maps to `MODM_ISR_NAME({vector})()`.
 * @note You may have to forward declare the interrupt using `MODM_ISR_DECL({vector})`.
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 *
 * @ingroup	modm_architecture_interrupt
 */
#define MODM_ISR_CALL(vector)

/**
 * Declare an interrupt handler.
 *
 * This macro allows the declaration of interrupt handlers using the name
 * declared in the datasheet in C or C++ code on any platform.
 *
 * On AVRs this maps to `ISR(MODM_ISR_NAME({vector}), args)`.
 * On Cortex-M and Hosted this maps to `void MODM_ISR_NAME({vector})(void) args`.
 * `extern "C"` is automatically added in a C++ environment.
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 * @param ...
 *        Multiple compiler attributes can be added to an interrupt. For example `modm_fastcode`.
 *
 * @ingroup	modm_architecture_interrupt
 */
#define MODM_ISR(vector, ...)

#else

#ifdef MODM_CPU_AVR

#	define MODM_ISR_NAME(vector) \
		vector ## _vect
#	define MODM_ISR_DECL(vector) \
		modm_extern_c void vector ## _vect(void)
#	define MODM_ISR_CALL(vector) \
		vector ## _vect()
#	define MODM_ISR(vector, ...) \
		ISR( vector ## _vect, ##__VA_ARGS__)

#elif defined MODM_CPU_ARM

#	define MODM_ISR_NAME(vector) \
		vector ## _IRQHandler
#	define MODM_ISR_DECL(vector) \
		modm_extern_c void vector ## _IRQHandler(void)
#	define MODM_ISR_CALL(vector) \
		vector ## _IRQHandler()
#	define MODM_ISR(vector, ...) \
		modm_extern_c void vector ## _IRQHandler(void) \
			__attribute__((externally_visible)) __VA_ARGS__; \
		void vector ## _IRQHandler(void)

#else

#	define MODM_ISR_NAME(vector) \
		vector ## _isr
#	define MODM_ISR_DECL(vector) \
		modm_extern_c void vector ## _isr(void)
#	define MODM_ISR_CALL(vector) \
		vector ## _isr()
#	define MODM_ISR(vector, ...) \
		modm_extern_c void vector ## _isr(void) __VA_ARGS__; \
		void vector ## _isr(void)

#endif

#endif // __DOXYGEN__

#endif // MODM_INTERRUPT_HPP
