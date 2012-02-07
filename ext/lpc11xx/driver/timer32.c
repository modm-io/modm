/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx 32-bit timer example
 *
 *   Description:
 *     This file contains 32-bit timer code example which include timer 
 *     initialization, timer interrupt handler, and related APIs for 
 *     timer setup.
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
#if CONFIG_ENABLE_DRIVER_TIMER32==1
#include "timer32.h"

/* ===================
 * CodeRed - Modified file to extract out interrupt handler related code,
 * which is really application project specific.
 * Set TIMER32_GENERIC_INTS to 1 to reenable original code.
 * =================== */
volatile uint32_t timer32_0_period = 0;
#ifdef CONFIG_TIMER32_DEFAULT_TIMER32_0_IRQHANDLER
volatile uint32_t timer32_0_counter = 0;
volatile uint32_t timer32_0_capture = 0;
#endif //CONFIG_TIMER32_DEFAULT_TIMER32_0_IRQHANDLER

volatile uint32_t timer32_1_period = 0;
#ifdef CONFIG_TIMER32_DEFAULT_TIMER32_1_IRQHANDLER
volatile uint32_t timer32_1_counter = 0;
volatile uint32_t timer32_1_capture = 0;
#endif //CONFIG_TIMER32_DEFAULT_TIMER32_1_IRQHANDLER

/*****************************************************************************
** Function name:		delay32Ms
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delay32Ms(uint8_t timer_num, uint32_t delayInMs)
{
  if (timer_num == 0)
  {
    /* setup timer #0 for delay */
    LPC_TMR32B0->TCR = 0x02;		/* reset timer */
    LPC_TMR32B0->PR  = 0x00;		/* set prescaler to zero */
    LPC_TMR32B0->MR0 = delayInMs * ((SystemCoreClock/(LPC_TMR32B0->PR+1)) / 1000);
    LPC_TMR32B0->IR  = 0xff;		/* reset all interrrupts */
    LPC_TMR32B0->MCR = 0x04;		/* stop timer on match */
    LPC_TMR32B0->TCR = 0x01;		/* start timer */
  
    /* wait until delay time has elapsed */
    while (LPC_TMR32B0->TCR & 0x01);
  }
  else if (timer_num == 1)
  {
    /* setup timer #1 for delay */
    LPC_TMR32B1->TCR = 0x02;		/* reset timer */
    LPC_TMR32B1->PR  = 0x00;		/* set prescaler to zero */
    LPC_TMR32B1->MR0 = delayInMs * ((SystemCoreClock/(LPC_TMR32B0->PR+1)) / 1000);
    LPC_TMR32B1->IR  = 0xff;		/* reset all interrrupts */
    LPC_TMR32B1->MCR = 0x04;		/* stop timer on match */
    LPC_TMR32B1->TCR = 0x01;		/* start timer */
  
    /* wait until delay time has elapsed */
    while (LPC_TMR32B1->TCR & 0x01);
  }
  return;
}


#if CONFIG_TIMER32_DEFAULT_TIMER32_0_IRQHANDLER==1
/******************************************************************************
** Function name:		TIMER32_0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER32_0_IRQHandler(void)
{  
  if ( LPC_TMR32B0->IR & 0x01 )
  {  
	LPC_TMR32B0->IR = 1;				/* clear interrupt flag */
	timer32_0_counter++;
  }
  if ( LPC_TMR32B0->IR & (0x1<<4) )
  {  
	LPC_TMR32B0->IR = 0x1<<4;			/* clear interrupt flag */
	timer32_0_capture++;
  }
  return;
}
#endif //CONFIG_TIMER32_DEFAULT_TIMER32_0_IRQHANDLER

#if CONFIG_TIMER32_DEFAULT_TIMER32_1_IRQHANDLER==1
/******************************************************************************
** Function name:		TIMER32_1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER32_1_IRQHandler(void)
{  
  if ( LPC_TMR32B1->IR & 0x01 )
  {    
	LPC_TMR32B1->IR = 1;			/* clear interrupt flag */
	timer32_1_counter++;
  }
  if ( LPC_TMR32B1->IR & (0x1<<4) )
  {  
	LPC_TMR32B1->IR = 0x1<<4;			/* clear interrupt flag */
	timer32_1_capture++;
  }
  return;
}
#endif //CONFIG_TIMER32_DEFAULT_TIMER32_1_IRQHANDLER

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void enable_timer32(uint8_t timer_num)
{
  if ( timer_num == 0 )
  {
    LPC_TMR32B0->TCR = 1;
  }
  else
  {
    LPC_TMR32B1->TCR = 1;
  }
  return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void disable_timer32(uint8_t timer_num)
{
  if ( timer_num == 0 )
  {
    LPC_TMR32B0->TCR = 0;
  }
  else
  {
    LPC_TMR32B1->TCR = 0;
  }
  return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void reset_timer32(uint8_t timer_num)
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
    regVal = LPC_TMR32B0->TCR;
    regVal |= 0x02;
    LPC_TMR32B0->TCR = regVal;
  }
  else
  {
    regVal = LPC_TMR32B1->TCR;
    regVal |= 0x02;
    LPC_TMR32B1->TCR = regVal;
  }
  return;
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		None
** 
******************************************************************************/
void init_timer32(uint8_t timer_num, uint32_t TimerInterval) 
{
  if ( timer_num == 0 )
  {
    /* Some of the I/O pins need to be carefully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);
    LPC_IOCON->PIO1_5 &= ~0x07;	/*  Timer0_32 I/O config */
    LPC_IOCON->PIO1_5 |= 0x02;	/* Timer0_32 CAP0 */
    LPC_IOCON->PIO1_6 &= ~0x07;
    LPC_IOCON->PIO1_6 |= 0x02;	/* Timer0_32 MAT0 */
    LPC_IOCON->PIO1_7 &= ~0x07;
    LPC_IOCON->PIO1_7 |= 0x02;	/* Timer0_32 MAT1 */
    LPC_IOCON->PIO0_1 &= ~0x07;	
    LPC_IOCON->PIO0_1 |= 0x02;	/* Timer0_32 MAT2 */
#ifdef __JTAG_DISABLED
    LPC_IOCON->JTAG_TDI_PIO0_11 &= ~0x07;	
    LPC_IOCON->JTAG_TDI_PIO0_11 |= 0x03;	/* Timer0_32 MAT3 */
#endif
#if CONFIG_TIMER32_DEFAULT_TIMER32_0_IRQHANDLER==1
    timer32_0_counter = 0;
    timer32_0_capture = 0;
#endif //TIMER32_0_DEFAULT_HANDLER
    LPC_TMR32B0->MR0 = TimerInterval;
#if TIMER_MATCH
	LPC_TMR32B0->EMR &= ~(0xFF<<4);
	LPC_TMR32B0->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8)|(0x3<<10));	/* MR0/1/2/3 Toggle */
#else
	/* Capture 0 on rising edge, interrupt enable. */
	LPC_TMR32B0->CCR = (0x1<<0)|(0x1<<2);
#endif
    LPC_TMR32B0->MCR = 3;			/* Interrupt and Reset on MR0 */

#if CONFIG_TIMER32_DEFAULT_TIMER32_0_IRQHANDLER==1
    /* Enable the TIMER0 Interrupt */
    NVIC_EnableIRQ(TIMER_32_0_IRQn);
#endif
  }
  else if ( timer_num == 1 )
  {
    /* Some of the I/O pins need to be clearfully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);
#ifdef __JTAG_DISABLED
    LPC_IOCON->JTAG_TMS_PIO1_0  &= ~0x07;	/*  Timer1_32 I/O config */
    LPC_IOCON->JTAG_TMS_PIO1_0  |= 0x03;	/* Timer1_32 CAP0 */
    LPC_IOCON->JTAG_TDO_PIO1_1  &= ~0x07;	
    LPC_IOCON->JTAG_TDO_PIO1_1  |= 0x03;	/* Timer1_32 MAT0 */
    LPC_IOCON->JTAG_nTRST_PIO1_2 &= ~0x07;
    LPC_IOCON->JTAG_nTRST_PIO1_2 |= 0x03;	/* Timer1_32 MAT1 */
    LPC_IOCON->ARM_SWDIO_PIO1_3  &= ~0x07;
    LPC_IOCON->ARM_SWDIO_PIO1_3  |= 0x03;	/* Timer1_32 MAT2 */
#endif
    LPC_IOCON->PIO1_4 &= ~0x07;
    LPC_IOCON->PIO1_4 |= 0x02;		/* Timer0_32 MAT3 */

#if CONFIG_TIMER32_DEFAULT_TIMER32_1_IRQHANDLER==1
    timer32_1_counter = 0;
    timer32_1_capture = 0;
#endif //TIMER32_1_DEFAULT_HANDLER

    LPC_TMR32B1->MR0 = TimerInterval;
#if TIMER_MATCH
	LPC_TMR32B1->EMR &= ~(0xFF<<4);
	LPC_TMR32B1->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8)|(0x3<<10));	/* MR0/1/2 Toggle */
#else
	/* Capture 0 on rising edge, interrupt enable. */
	LPC_TMR32B1->CCR = (0x1<<0)|(0x1<<2);
#endif
    LPC_TMR32B1->MCR = 3;			/* Interrupt and Reset on MR0 */

#if CONFIG_TIMER32_DEFAULT_TIMER32_1_IRQHANDLER==1
    /* Enable the TIMER1 Interrupt */
    NVIC_EnableIRQ(TIMER_32_1_IRQn);
#endif
  }
  return;
}
/******************************************************************************
** Function name:		init_timer32PWM
**
** Descriptions:		Initialize timer as PWM
**
** parameters:			timer number, period and match enable:
**										match_enable[0] = PWM for MAT0 
**										match_enable[1] = PWM for MAT1
**										match_enable[2] = PWM for MAT2
**			
** Returned value:		None
** 
******************************************************************************/
void init_timer32PWM(uint8_t timer_num, uint32_t period, uint8_t match_enable)
{
	
	disable_timer32(timer_num);
	if (timer_num == 1)
	{
	    /* Some of the I/O pins need to be clearfully planned if
	    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
	    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);

		/* Setup the external match register */
		LPC_TMR32B1->EMR = (1<<EMC3)|(1<<EMC2)|(2<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);

		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
		  	LPC_IOCON->R_PIO1_1  &= ~0x07;
  			LPC_IOCON->R_PIO1_1  |= 0x03;		/* Timer1_32 MAT0 */
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
		  LPC_IOCON->R_PIO1_2 &= ~0x07;
		  LPC_IOCON->R_PIO1_2 |= 0x03;		/* Timer1_32 MAT1 */
		}
		/* If match2 is enabled, set the output */
		if (match_enable & 0x04)
		{
		  LPC_IOCON->SWDIO_PIO1_3   &= ~0x07;
		  LPC_IOCON->SWDIO_PIO1_3   |= 0x03;		/* Timer1_32 MAT2 */
		}
		/* If match3 is enabled, set the output */
		if (match_enable & 0x08)
		{
		  LPC_IOCON->PIO1_4           &= ~0x07;
		  LPC_IOCON->PIO1_4           |= 0x02;		/* Timer1_32 MAT3 */
		}

//#ifdef __JTAG_DISABLED
//	  PIO1_0_JTAG_TMS  &= ~0x07;	/*  Timer1_32 I/O config */
//	  PIO1_0_JTAG_TMS  |= 0x03;		/* Timer1_32 CAP0 */
//#endif

		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR32B1->PWMC = (1<<3)|(match_enable);
 
		/* Setup the match registers */
		/* set the period value to a global variable */
		timer32_1_period = period;
		LPC_TMR32B1->MR3 = timer32_1_period;
		LPC_TMR32B1->MR0	= timer32_1_period/2;
		LPC_TMR32B1->MR1	= timer32_1_period/2;
		LPC_TMR32B1->MR2	= timer32_1_period/2;
		
		/* */
    	LPC_TMR32B1->MCR = 1<<10;				/* Reset on MR3 */
	}
	else
	{
	    /* Some of the I/O pins need to be clearfully planned if
	    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
	    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);

		/* Setup the external match register */
		LPC_TMR32B0->EMR = (1<<EMC3)|(2<<EMC2)|(1<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);
 
		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
//	 		LPC_IOCON->PIO1_6           &= ~0x07;
//		  	LPC_IOCON->PIO1_6           |= 0x02;		/* Timer0_32 MAT0 */
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON-> PIO1_7           &= ~0x07;
		  	LPC_IOCON->PIO1_7           |= 0x02;		/* Timer0_32 MAT1 */
		}
		/* If match2 is enabled, set the output */
		if (match_enable & 0x04)
		{
		  LPC_IOCON->PIO0_1           &= ~0x07;	
		  LPC_IOCON->PIO0_1           |= 0x02;		/* Timer0_32 MAT2 */
		}
		/* If match3 is enabled, set the output */
		if (match_enable & 0x08)
		{
		  LPC_IOCON->R_PIO0_11 &= ~0x07;
		  LPC_IOCON->R_PIO0_11 |= 0x03;		/* Timer0_32 MAT3 */
		}

		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR32B0->PWMC = (1<<3)|(match_enable);

		/* Setup the match registers */
		/* set the period value to a global variable */
		timer32_0_period = period;
		LPC_TMR32B0->MR3 = timer32_0_period;
		LPC_TMR32B0->MR0	= timer32_0_period;	///2;
		LPC_TMR32B0->MR1	= timer32_0_period/2;
		LPC_TMR32B0->MR2	= timer32_0_period/2;

    	LPC_TMR32B0->MCR = 1<<10;				/* Reset on MR3 */
	}


}
/******************************************************************************
** Function name:		pwm32_setMatch
**
** Descriptions:		Set the pwm32 match values
**
** parameters:			timer number, match numner and the value
**
** Returned value:		None
** 
******************************************************************************/
void setMatch_timer32PWM (uint8_t timer_num, uint8_t match_nr, uint32_t value)
{
	if (timer_num)
	{
		switch (match_nr)
		{
			case 0:
				LPC_TMR32B1->MR0 = value;
				break;
			case 1: 
				LPC_TMR32B1->MR1 = value;
				break;
			case 2:
				LPC_TMR32B1->MR2 = value;
				break;
			case 3: 
				LPC_TMR32B1->MR3 = value;
				break;
			default:
				break;
		}	

	}
	else 
	{
		switch (match_nr)
		{
			case 0:
				LPC_TMR32B0->MR0 = value;
				break;
			case 1: 
				LPC_TMR32B0->MR1 = value;
				break;
			case 2:
				LPC_TMR32B0->MR2 = value;
				break;
			case 3: 
				LPC_TMR32B0->MR3 = value;
				break;
			default:
				break;
		}	
	}

}
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
