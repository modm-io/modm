/***********************************************************************
 * $Id::                                                               $
 *
 *     Copyright (C) 2010 NXP Semiconductors.

 * Description:
 *     Simple GPIO I/O routines for the LPC11xx series of Cortex M0
 *     microcontrollers.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/


#ifndef SMALLGPIO_H_INCLUDED
#define SMALLGPIO_H_INCLUDED
#if CONFIG_ENABLE_DRIVER_SMALLGPIO==1
#include "driver_config.h"

// InitGPIO() turns on the GPIO and IOCON clocks
#define InitGPIO() LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<6) | (1<<16))

#define PORT_BIT_COUNT 12 // up to 12 I/O lines per port
#define PORT_COUNT 4 // up to 4 ports

extern const uint8_t IOCON_LUT[PORT_COUNT][PORT_BIT_COUNT];
#define IOCONAddress(port,bit) ((volatile uint16_t * const) (LPC_IOCON_BASE + IOCON_LUT[port][bit]))
#define GPIODirAddress(port) ((volatile uint16_t * const) (LPC_GPIO_BASE + (0x10000*(port)) + 0x8000))
#define GPIOMaskAddress(port,bits) ((volatile uint16_t * const) (LPC_GPIO_BASE + (0x10000*(port)) + (4*(bits))))

#define SetIOCON(port, bit, ioconmask, ioconbits) \
    *IOCONAddress((port),(bit)) = (*IOCONAddress((port),(bit)) & ~(ioconmask)) | (ioconbits)

#define SetupGPIO(port, bit, output, ioconmask, ioconbits) \
    { \
        SetIOCON((port), (bit), (ioconmask), (ioconbits)); \
        *GPIODirAddress((port)) = (output) ? *GPIODirAddress((port)) | (1<<(bit)) : *GPIODirAddress((port)) & ~(1<<(bit)); \
    }

#define SetGPIOOut(port, bit) *GPIODirAddress((port)) |= 1<<(bit)
#define SetGPIOIn(port, bit) *GPIODirAddress((port)) &= ~(1<<(bit))

#define SetGPIOBits(port,bits) ( *GPIOMaskAddress((port),(bits)) = (bits) )
#define ClrGPIOBits(port,bits) ( *GPIOMaskAddress((port),(bits)) = 0 )
#define ToggleGPIOBits(port,bits) ( *GPIOMaskAddress((port),(bits)) = ~(*GPIOMaskAddress((port),(bits))) )

#define SetGPIOBit(port,bit) SetGPIOBits((port), 1<<(bit))
#define ClrGPIOBit(port,bit) ClrGPIOBits((port), 1<<(bit))
#define ToggleGPIOBit(port,bit) ToggleGPIOBits((port), 1<<(bit))
#define GetGPIOBit(port, bit) (*GPIOMaskAddress((port), 1<<(bit)) ? 1 : 0)

typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1,
    IOCON_MODE_MASK = (3<<3),
    IOCON_MODE_PULLUP = (2<<3),
    IOCON_MODE_PULLDOWN = (1<<3),
    IOCON_MODE_REPEATER = (3<<3),
    IOCON_MODE_INACTIVE = (0<<3),
    IOCON_FUNC_MASK = 7,
    IOCON_FUNC_0 = 0,
    IOCON_FUNC_1 = 1,
    IOCON_FUNC_2 = 2,
    IOCON_FUNC_3 = 3,
    IOCON_FUNC_4 = 4,
    IOCON_FUNC_5 = 5,
    IOCON_FUNC_6 = 6,
    IOCON_FUNC_7 = 7,
    IOCON_HYS_MASK = (1<<5),
    IOCON_HYS_ENABLE = (1<<5),
    IOCON_HYS_DISABLE = (0<<5),
    IOCON_ADMODE_MASK = (1<<7),
    IOCON_ADMODE_ANALOG = (0<<7),
    IOCON_ADMODE_DIGITAL = (1<<7)
} IOCON_BITFIELD_ENUM;

#endif
// #ifndef SMALLGPIO_H_INCLUDED
#endif
