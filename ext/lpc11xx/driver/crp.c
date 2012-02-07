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
#include "driver_config.h"
#if CONFIG_ENABLE_DRIVER_CRP==1
#include "crp.h"
/************************************************************/
/**** DANGER CRP3 WILL LOCK PART TO ALL READS and WRITES ****/
/*********** #define CRP3_MAGIC xxxx 0x43218765 *************/
/************************************************************/

#ifdef __GNUC__
__attribute__ ((section(".crp"))) const uint32_t CRP_WORD = CURRENT_CRP_SETTING;
#endif
#ifdef __IAR_SYSTEMS_ICC__
const __root uint32_t CRP_WORD @ ".crp" = CURRENT_CRP_SETTING;
#endif
#endif
