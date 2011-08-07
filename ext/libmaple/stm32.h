/**
 *  @brief General STM32 specific definitions
 */

#ifndef _STM32_H_
#define _STM32_H_

#define PCLK1   36000000U
#define PCLK2   72000000U

// added for xpcc
#if defined STM32F10X_MD
#	define STM32_MEDIUM_DENSITY
#elif defined STM32F10X_HD
#	define STM32_HIGH_DENSITY
#endif

#if defined __ARM_STM32F103RB__
#	define	MCU_STM32F103RB
#elif defined __ARM_STM32F103ZE__
#	define MCU_STM32F103ZE
#endif


#ifdef STM32_MEDIUM_DENSITY
    #define NR_INTERRUPTS 43
#else
#ifdef STM32_HIGH_DENSITY
    #define NR_INTERRUPTS 60
#else
#error "No STM32 board type defined!"
#endif
#endif

#endif

