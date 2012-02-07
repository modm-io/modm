/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx 16-bit timer example
 *
 *   Description:
 *     This file contains 16-bit timer code example which include timer 
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
#include "timer16.h"

#if CONFIG_ENABLE_DRIVER_TIMER16==1

volatile uint32_t timer16_0_counter = 0;
volatile uint32_t timer16_1_counter = 0;
volatile uint32_t timer16_0_capture = 0;
volatile uint32_t timer16_1_capture = 0;
volatile uint32_t timer16_0_period = 0;
volatile uint32_t timer16_1_period = 0;

/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs(uint8_t timer_num, uint32_t delayInMs)
{
  if (timer_num == 0)
  {
    /*
    * setup timer #0 for delay
    */
    LPC_TMR16B0->TCR = 0x02;		/* reset timer */
    LPC_TMR16B0->PR  = MHZ_PRESCALE; /* set prescaler to get 1 M counts/sec */
    LPC_TMR16B0->MR0 = delayInMs * TIME_INTERVALmS;
    LPC_TMR16B0->IR  = 0xff;		/* reset all interrrupts */
    LPC_TMR16B0->MCR = 0x04;		/* stop timer on match */
    LPC_TMR16B0->TCR = 0x01;		/* start timer */
    /* wait until delay time has elapsed */
    while (LPC_TMR16B0->TCR & 0x01);
  }
  else if (timer_num == 1)
  {
    /*
    * setup timer #1 for delay
    */
    LPC_TMR16B1->TCR = 0x02;		/* reset timer */
    LPC_TMR16B1->PR  = MHZ_PRESCALE; /* set prescaler to get 1 M counts/sec */
    LPC_TMR16B1->MR0 = delayInMs * TIME_INTERVALmS;
    LPC_TMR16B1->IR  = 0xff;		/* reset all interrrupts */
    LPC_TMR16B1->MCR = 0x04;		/* stop timer on match */
    LPC_TMR16B1->TCR = 0x01;		/* start timer */
    /* wait until delay time has elapsed */
    while (LPC_TMR16B1->TCR & 0x01);
  }
  return;
}

/******************************************************************************
** Function name:		TIMER16_0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER16_0_IRQHandler(void)
{  
  if ( LPC_TMR16B0->IR & 0x1 )
  {
  LPC_TMR16B0->IR = 1;			/* clear interrupt flag */
  timer16_0_counter++;
  }
  if ( LPC_TMR16B0->IR & (0x1<<4) )
  {
	LPC_TMR16B0->IR = 0x1<<4;		/* clear interrupt flag */
	timer16_0_capture++;
  }
  return;
}

/******************************************************************************
** Function name:		TIMER16_1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER16_1_IRQHandler(void)
{  
  if ( LPC_TMR16B1->IR & 0x1 )
  {  
  LPC_TMR16B1->IR = 1;			/* clear interrupt flag */
  timer16_1_counter++;
  }
  if ( LPC_TMR16B1->IR & (0x1<<4) )
  {
	LPC_TMR16B1->IR = 0x1<<4;		/* clear interrupt flag */
	timer16_1_capture++;
  }
  return;
}

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void enable_timer16(uint8_t timer_num)
{
  if ( timer_num == 0 )
  {
    LPC_TMR16B0->TCR = 1;
  }
  else
  {
    LPC_TMR16B1->TCR = 1;
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
void disable_timer16(uint8_t timer_num)
{
  if ( timer_num == 0 )
  {
    LPC_TMR16B0->TCR = 0;
  }
  else
  {
    LPC_TMR16B1->TCR = 0;
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
void reset_timer16(uint8_t timer_num)
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
    regVal = LPC_TMR16B0->TCR;
    regVal |= 0x02;
    LPC_TMR16B0->TCR = regVal;
  }
  else
  {
    regVal = LPC_TMR16B1->TCR;
    regVal |= 0x02;
    LPC_TMR16B1->TCR = regVal;
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
void init_timer16(uint8_t timer_num, uint16_t TimerInterval)
{
  if ( timer_num == 0 )
  {
    /* Some of the I/O pins need to be clearfully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
    LPC_IOCON->PIO0_2           &= ~0x07;	/*  Timer0_16 I/O config */
    LPC_IOCON->PIO0_2           |= 0x02;		/* Timer0_16 CAP0 */
    LPC_IOCON->PIO0_8           &= ~0x07;	
    LPC_IOCON->PIO0_8           |= 0x02;		/* Timer0_16 MAT0 */
    LPC_IOCON->PIO0_9           &= ~0x07;
    LPC_IOCON->PIO0_9           |= 0x02;		/* Timer0_16 MAT1 */
#ifdef __JTAG_DISABLED
    LPC_IOCON->JTAG_TCK_PIO0_10 &= ~0x07;
    LPC_IOCON->JTAG_TCK_PIO0_10 |= 0x03;		/* Timer0_16 MAT2 */
#endif	

    timer16_0_counter = 0;
	timer16_0_capture = 0;

    LPC_TMR16B0->PR  = MHZ_PRESCALE; /* set prescaler to get 1 M counts/sec */
    LPC_TMR16B0->MR0 = TIME_INTERVALmS * 10; /* Set up 10 mS interval */
#if TIMER_MATCH
	LPC_TMR16B0->EMR &= ~(0xFF<<4);
	LPC_TMR16B0->EMR |= ((0x3<<4)|(0x3<<6));
#else
	/* Capture 0 on rising edge, interrupt enable. */
	LPC_TMR16B0->CCR = (0x1<<0)|(0x1<<2);
#endif
    LPC_TMR16B0->MCR = 3;				/* Interrupt and Reset on MR0 and MR1 */

    /* Enable the TIMER0 Interrupt */
    NVIC_EnableIRQ(TIMER_16_0_IRQn);
  }
  else if ( timer_num == 1 )
  {
    /* Some of the I/O pins need to be clearfully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);
    LPC_IOCON->PIO1_8           &= ~0x07;	/*  Timer1_16 I/O config */
    LPC_IOCON->PIO1_8           |= 0x01;		/* Timer1_16 CAP0 */
    LPC_IOCON->PIO1_9           &= ~0x07;	
    LPC_IOCON->PIO1_9           |= 0x01;		/* Timer1_16 MAT0 */
    LPC_IOCON->PIO1_10          &= ~0x07;
    LPC_IOCON->PIO1_10          |= 0x02;		/* Timer1_16 MAT1 */	

    timer16_1_counter = 0;
	timer16_1_capture = 0;
    LPC_TMR16B0->PR  = MHZ_PRESCALE; /* set prescaler to get 1 M counts/sec */
    LPC_TMR16B0->MR0 = TIME_INTERVALmS * 10; /* Set up 10 mS interval */
#if TIMER_MATCH
	LPC_TMR16B1->EMR &= ~(0xFF<<4);
	LPC_TMR16B1->EMR |= ((0x3<<4)|(0x3<<6));
#else
	/* Capture 0 on rising edge, interrupt enable. */
	LPC_TMR16B1->CCR = (0x1<<0)|(0x1<<2);
#endif
    LPC_TMR16B1->MCR = 3;				/* Interrupt and Reset on MR0 and MR1 */

    /* Enable the TIMER1 Interrupt */
    NVIC_EnableIRQ(TIMER_16_1_IRQn);
  }
  return;
}
/******************************************************************************
** Function name:		init_timer16PWM
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
void init_timer16PWM(uint8_t timer_num, uint32_t period, uint8_t match_enable, uint8_t cap_enabled)
{
	
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	disable_timer16(timer_num);

	if (timer_num == 1)
	{

		/* Some of the I/O pins need to be clearfully planned if
		you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);
		
		/* Setup the external match register */
		LPC_TMR16B1->EMR = (1<<EMC3)|(1<<EMC2)|(1<<EMC1)|(2<<EMC0)|(1<<3)|(match_enable);
		
		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
			LPC_IOCON->PIO1_9           &= ~0x07;	
			LPC_IOCON->PIO1_9           |= 0x01;		/* Timer1_16 MAT0 */
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON->PIO1_10          &= ~0x07;
			LPC_IOCON->PIO1_10          |= 0x02;		/* Timer1_16 MAT1 */
		}
		
		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR16B1->PWMC = (1<<3)|(match_enable);
		
		/* Setup the match registers */
		/* set the period value to a global variable */
		timer16_1_period 	= period;
		LPC_TMR16B1->MR3 	= timer16_1_period;
		LPC_TMR16B1->MR0	= timer16_1_period/2;
		LPC_TMR16B1->MR1	= timer16_1_period/2;
		LPC_TMR16B1->MR2	= timer16_1_period/2;
		
		/* Set match control register */
		LPC_TMR16B1->MCR = 1<<10;// | 1<<9;				/* Reset on MR3 */
		
		if (cap_enabled)
		{
			LPC_IOCON->PIO1_8 &= ~0x07;						/*  Timer1_16 I/O config */
			LPC_IOCON->PIO1_8 |= 0x01 | (2<<3);				/* Timer1_16 CAP0 */
			LPC_GPIO1->DIR &= ~(1<<8); 
			LPC_TMR16B1->IR = 0xF;							/* clear interrupt flag */
			
			/* Set the capture control register */
			LPC_TMR16B1->CCR = 7;
			
		}
		/* Enable the TIMER1 Interrupt */
		NVIC_EnableIRQ(TIMER_16_1_IRQn);
	}
	else
	{
		/* Some of the I/O pins need to be clearfully planned if
		you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
		
		/* Setup the external match register */
		LPC_TMR16B0->EMR = (1<<EMC3)|(1<<EMC2)|(1<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);
		
		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
			LPC_IOCON->PIO0_8           &= ~0x07;	
			LPC_IOCON->PIO0_8           |= 0x02;		/* Timer0_16 MAT0 			*/
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON->PIO0_9           &= ~0x07;
			LPC_IOCON->PIO0_9           |= 0x02;		/* Timer0_16 MAT1 			*/
		}
		/* If match2 is enabled, set the output */
		if (match_enable & 0x04)
		{
			LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
			LPC_IOCON->SWCLK_PIO0_10 |= 0x03;		/* Timer0_16 MAT2 */
		}
		
		//	  PIO0_2           &= ~0x07;	/* Timer0_16 I/O config */
		//	  PIO0_2           |= 0x02;		/* Timer0_16 CAP0 			*/
		
		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR16B0->PWMC = (1<<3)|(match_enable);
		
		/* Setup the match registers */
		/* set the period value to a global variable */
		timer16_0_period = period;
		LPC_TMR16B0->MR3 = timer16_0_period;
		LPC_TMR16B0->MR0	= timer16_0_period/2;
		LPC_TMR16B0->MR1	= timer16_0_period/2;
		LPC_TMR16B0->MR2	= timer16_0_period/2;
		
		/* Set the match control register */
		LPC_TMR16B0->MCR = 1<<10;				/* Reset on MR3 */
		
		/* Enable the TIMER1 Interrupt */
		NVIC_EnableIRQ(TIMER_16_0_IRQn);
	}

}
/******************************************************************************
** Function name:		pwm16_setMatch
**
** Descriptions:		Set the pwm16 match values
**
** parameters:			timer number, match numner and the value
**
** Returned value:		None
** 
******************************************************************************/
void setMatch_timer16PWM (uint8_t timer_num, uint8_t match_nr, uint32_t value)
{
	if (timer_num)
	{
		switch (match_nr)
		{
			case 0:
				LPC_TMR16B1->MR0 = value;
				break;
			case 1: 
				LPC_TMR16B1->MR1 = value;
				break;
			case 2:
				LPC_TMR16B1->MR2 = value;
				break;
			case 3: 
				LPC_TMR16B1->MR3 = value;
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
				LPC_TMR16B0->MR0 = value;
				break;
			case 1: 
				LPC_TMR16B0->MR1 = value;
				break;
			case 2:
				LPC_TMR16B0->MR2 = value;
				break;
			case 3: 
				LPC_TMR16B0->MR3 = value;
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
