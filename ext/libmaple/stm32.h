/**
 *  @brief General STM32 specific definitions
 */

#ifndef _STM32_H_
#define _STM32_H_

#define PCLK1   36000000U
#define PCLK2   72000000U

/*
#if defined __ARM_STM32F103RB__
#	define	MCU_STM32F103RB
#elif defined __ARM_STM32F103ZE__
#	define MCU_STM32F103ZE
#endif
*/

// number of peripheral interrupts
#if defined STM32_MEDIUM_DENSITY
#	define NR_INTERRUPTS 43
#elif defined STM32_HIGH_DENSITY
#	define NR_INTERRUPTS 60
#elif defined STM32_XL_DENSITY
#	define NR_INTERRUPTS 60
#elif defined STM32_CONNECTIVITY
#	define NR_INTERRUPTS 68
#else
#	error "No STM32 board type defined!"
#endif

#endif

