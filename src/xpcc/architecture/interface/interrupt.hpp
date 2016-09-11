// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERRUPT_HPP
#define XPCC_INTERRUPT_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

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
 * interrupt vector, such as `XPCC_ISR_NAME(ADC)` vs. the `ADC` peripheral.
 * When defining interrupt vectors for compatibility, you MUST use this macro
 * instead of manually defining it!
 * @code
 * #define               ADC                  ADC_CAN_TX  // will not compile!
 * #define XPCC_ISR_NAME(ADC)   XPCC_ISR_NAME(ADC_CAN_TX) // will     compile
 * XPCC_ISR(ADC) {...}
 * @endcode
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 *
 * @ingroup	platform
 */
#define XPCC_ISR_NAME(vector)

/**
 * Forward declare in interrupt function.
 *
 * This maps to `extern void XPCC_ISR_NAME({vector})(void)`.
 * `extern "C"` is used automatically in a C++ environment.
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 *
 * @ingroup	platform
 */
#define XPCC_ISR_DECL(vector)

/**
 * Directly calls an interrupt function.
 *
 * This maps to `XPCC_ISR_NAME({vector})()`.
 * @note You may have to forward declare the interrupt using `XPCC_ISR_DECL({vector})`.
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 *
 * @ingroup	platform
 */
#define XPCC_ISR_CALL(vector)

/**
 * Declare an interrupt handler.
 *
 * This macro allows the declaration of interrupt handlers using the name
 * declared in the datasheet in C or C++ code on any platform.
 *
 * On AVRs this maps to `ISR(XPCC_ISR_NAME({vector}), args)`.
 * On Cortex-M and Hosted this maps to `void XPCC_ISR_NAME({vector})(void) args`.
 * `extern "C"` is automatically added in a C++ environment.
 *
 * @param vector
 *        The name of the interrupt without any suffix (neither `_vect`, nor `_IRQHandler`).
 * @param ...
 *        Multiple compiler attributes can be added to an interrupt. For example `xpcc_fastcode`.
 *
 * @ingroup	platform
 */
#define XPCC_ISR(vector, ...)

#else

#ifdef XPCC__CPU_AVR

#	define XPCC_ISR_NAME(vector) \
		vector ## _vect
#	define XPCC_ISR_DECL(vector) \
		xpcc_extern_c void vector ## _vect(void)
#	define XPCC_ISR_CALL(vector) \
		vector ## _vect()
#	define XPCC_ISR(vector, ...) \
		ISR( vector ## _vect, ##__VA_ARGS__)

#elif defined XPCC__CPU_ARM

#	define XPCC_ISR_NAME(vector) \
		vector ## _IRQHandler
#	define XPCC_ISR_DECL(vector) \
		xpcc_extern_c void vector ## _IRQHandler(void)
#	define XPCC_ISR_CALL(vector) \
		vector ## _IRQHandler()
#	define XPCC_ISR(vector, ...) \
		xpcc_extern_c void vector ## _IRQHandler(void) \
			__attribute__((externally_visible)) __VA_ARGS__; \
		void vector ## _IRQHandler(void)

#else

#	define XPCC_ISR_NAME(vector) \
		vector ## _isr
#	define XPCC_ISR_DECL(vector) \
		xpcc_extern_c void vector ## _isr(void)
#	define XPCC_ISR_CALL(vector) \
		vector ## _isr()
#	define XPCC_ISR(vector, ...) \
		xpcc_extern_c void vector ## _isr(void) __VA_ARGS__; \
		void vector ## _isr(void)

#endif

#endif // __DOXYGEN__

#endif // XPCC_INTERRUPT_HPP
