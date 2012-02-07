/****************************************************************************
 *   $Id::                                                             $
 *   Project: NXP LPC11xx SSP example
 *
 *   Description:
 *     Code Read Protection example for NXP LPC11xx Family Microprocessors
 *
 ****************************************************************************
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
****************************************************************************/
#ifndef _CRP_H_INCLUDED_
#define _CRP_H_INCLUDED_
#if CONFIG_ENABLE_DRIVER_CRP==1

#include "driver_config.h"

#define CRP_NO_CRP          0xFFFFFFFF

// Disables UART and USB In System Programming (reads and writes)
// Leaves SWD debugging, with reads and writes, enabled
#define CRP_NO_ISP    0x4E697370

// Disables SWD debugging & JTAG, leaves ISP with with reads and writes enabled
// You will need UART connectivity and FlashMagic (flashmagictool.com) to reverse
// this. Don't even try this without these tools; most likely the SWD flash
// programming will not even complete.
// Allows reads and writes only to RAM above 0x10000300 and flash other than
// sector 0 (the first 4 kB). Full erase also allowed- again only through UART
// and FlashMagic (NO JTAG/SWD)
#define CRP_CRP1      0x12345678

// Disables SWD debugging & JTAG, leaves UART ISP with with only full erase
// enabled. You must have UART access and FlashMagic before setting this
// option.
// Don't even try this without these tools; most likely the SWD flash
// programming will not even complete.
#define CRP_CRP2      0x87654321

/************************************************************/
/**** DANGER CRP3 WILL LOCK PART TO ALL READS and WRITES ****/
#define CRP_CRP3_CONSUME_PART 0x43218765
/************************************************************/

#if CONFIG_CRP_SETTING_NO_CRP == 1
#define CURRENT_CRP_SETTING CRP_NO_CRP
#endif

#if CONFIG_CRP_SETTING_NOISP == 1
#define CURRENT_CRP_SETTING CRP_NO_ISP
#endif

#if CONFIG_CRP_SETTING_CRP1 == 1
#define CURRENT_CRP_SETTING CRP_CRP1
#endif

#if CONFIG_CRP_SETTING_CRP2 == 1
#define CURRENT_CRP_SETTING CRP_CRP2
#endif

#if CONFIG_CRP_SETTING_CRP3_CONSUME_PART == 1
#define CURRENT_CRP_SETTING CRP_CRP3_CONSUME_PART
#endif

#ifndef CURRENT_CRP_SETTING
#define CURRENT_CRP_SETTING CRP_NO_CRP
#endif

#endif
 /* _CRP_H_INCLUDED_ */
#endif
