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

#include "driver_config.h"
#if CONFIG_ENABLE_DRIVER_SMALLGPIO
#include "small_gpio.h"

const uint8_t IOCON_LUT[PORT_COUNT][PORT_BIT_COUNT] = {
    { 0x0C, 0x10, 0x1C, 0x2C, 0x30, 0x34, 0x4C, 0x50, 0x60, 0x64, 0x68, 0x74 },
    { 0x78, 0x7C, 0x80, 0x90, 0x94, 0xA0, 0xA4, 0xA8, 0x14, 0x38, 0x6C, 0x98 },
    { 0x08, 0x28, 0x5C, 0x8C, 0x40, 0x44, 0x00, 0x20, 0x24, 0x54, 0x58, 0x70 },
    { 0x84, 0x88, 0x9C, 0xAC, 0x3C, 0x48 }
};
#endif
