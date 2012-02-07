/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx WDT(Watchdog timer) example
 *
 *   Description:
 *     This file contains WDT code example which include WDT 
 *     initialization, WDT interrupt handler, and APIs for WDT
 *     reading.
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
#include "driver_config.h"			/* LPC11xx Peripheral Registers */
#include "wdt.h"

#if CONFIG_ENABLE_DRIVER_WDT==1

volatile uint32_t wdt_counter;

#if CONFIG_WDT_DEFAULT_WDT_IRQHANDLER==1
/*****************************************************************************
** Function name:		WDTHandler
**
** Descriptions:		Watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDT_IRQHandler(void)
{
  uint32_t i;

#if PROTECT_MODE
  /* For WDPROTECT test */
  /* Make sure counter is below WDWARNINT */
  wdt_counter = LPC_WDT->TV;
  while (wdt_counter >= 0x1FF)
  {
    wdt_counter = LPC_WDT->TV;
  }
  /* For WDPROTECT test */
  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55;    
  /* Make sure feed sequence executed properly */
  for (i = 0; i < 0x80000; i++);

  /* Make sure counter is reloaded with WDTC */
  wdt_counter = LPC_WDT->TV;
  while (wdt_counter < 0x200)
  {
    wdt_counter = LPC_WDT->TV;
  }
#endif

  LPC_WDT->MOD &= ~WDTOF;		/* clear the time-out flag and interrupt flag */
  LPC_WDT->MOD &= ~WDINT;		/* clear the time-out flag and interrupt flag */
}
#endif

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**						watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTInit( void )
{
  uint32_t i;

  // Settings for LPC1114
  /* Enable clock to WDT */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);

  LPC_SYSCON->WDTOSCCTRL = 0x03F; /* ~8kHz */
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<6);

  LPC_SYSCON->WDTCLKSEL = 0x02;		/* Select watchdog osc */
  LPC_SYSCON->WDTCLKUEN = 0x01;		/* Update clock */
  LPC_SYSCON->WDTCLKUEN = 0x00;		/* Toggle update register once */
  LPC_SYSCON->WDTCLKUEN = 0x01;
  while ( !(LPC_SYSCON->WDTCLKUEN & 0x01) );		/* Wait until updated */
  LPC_SYSCON->WDTCLKDIV = 1;		/* Divided by 1 */
  wdt_counter = 0;

#if CONFIG_WDT_ENABLE_IRQ==1
  NVIC_EnableIRQ(WDT_IRQn);
#endif

  LPC_WDT->TC = WDT_FEED_VALUE;	/* once WDEN is set, the WDT will start after feeding */
#if CONFIG_WDT_PROTECT_MODE==1
  LPC_WDT->TC = 0x3FF;
#endif
#if CONFIG_WDT_WINDOW_MODE==1
  LPC_WDT->TC = 0x3FF;
#endif

#if CONFIG_WDT_WATCHDOG_RESET==1
  LPC_WDT->MOD = WDEN | WDRESET;
#else
  LPC_WDT->MOD = WDEN;
#endif

  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55;    
  /* Make sure feed sequence executed properly */
  for (i = 0; i < 0x80000; i++);

  /* For WDWARNINT test */
  LPC_WDT->WARNINT = 0x1FF;

#if CONFIG_WDT_PROTECT_MODE==1
  /* For WDPROTECT test */
  LPC_WDT->MOD = WDEN | WDPROTECT;

  LPC_WDT->WINDOW = 0x2FF;
#endif

#if CONFIG_WDT_WINDOW_MODE==1
  /* For WDWINDOW test */
  LPC_WDT->WINDOW = 0x2FF;
#endif

#if CONFIG_WDT_WINDOW_MODE==1
  /* For WDWINDOW test */
  while (1)
  {
    wdt_counter = LPC_WDT->TV;
    while (wdt_counter >= 0x0000027F)
    {
      wdt_counter = LPC_WDT->TV;
    }

    LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
    LPC_WDT->FEED = 0x55;    
    /* Make sure feed sequence executed properly */
    for (i = 0; i < 0x80000; i++);
  }
#endif

  return;
}

/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTFeed( void )
{
  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55;
  return;
}
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
