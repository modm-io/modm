/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx RS485 example
 *
 *   Description:
 *     This file contains RS485 code example which include RS485 
 *     initialization, RS485 interrupt handler, and APIs for RS485
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
****************************************************************************/#include "driver_config.h"
#if CONFIG_ENABLE_DRIVER_RS485==1
#include "rs485.h"

volatile uint32_t UARTStatus;
volatile uint8_t  UART0TxEmpty = 1;
volatile uint8_t  UARTBuffer[BUFSIZE];
volatile uint32_t UARTCount = 0;
volatile uint32_t garbageCount = 0;

#if CONFIG_RS485_DEFAULT_UART_IRQHANDLER==1
/*****************************************************************************
** Function name:		UART_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART_IRQHandler(void)
{
  volatile uint8_t IIRValue, LSRValue;
  volatile uint8_t Dummy = Dummy;

  IIRValue = LPC_UART->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if (IIRValue == IIR_RLS)		/* Receive Line Status */
  {
    LSRValue = LPC_UART->LSR;
    /* Receive Line Status */
	if ( ((LSRValue & (LSR_PE|LSR_RDR)) == (LSR_PE|LSR_RDR)) 
						&& (LPC_UART->RS485CTRL & RS485_NMMEN) )
	{
	  Dummy = LPC_UART->RBR;
	  /* if address match, enable RX, otherwise, disable RX. */
	  if ( Dummy == LPC_UART->ADRMATCH )
	  {
		LPC_UART->RS485CTRL &= ~RS485_RXDIS;	/* Enable RX */
	  }
	  else
	  {
		LPC_UART->RS485CTRL |= RS485_RXDIS;		/* Disable RX */
		if ( (LPC_UART->FIFOLVL & 0x0F0F) != 0x0 )
		{
		  while ( LPC_UART->LSR & LSR_RDR )
		  {
			Dummy = LPC_UART->RBR;	/* Clear data from RX FIFO */
			garbageCount++;
		  }
		  return;
		}
	  }
	}
    else if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
    {
      /* There are errors or break interrupt */
      /* Read LSR will clear the interrupt */
      UARTStatus = LSRValue;
      Dummy = LPC_UART->RBR;		/* Dummy read on RX to clear 
							interrupt, then bail out */
      return;
    }
    else if (LSRValue & LSR_RDR)	/* Receive Data Ready */			
    {
      /* If no error on RLS, normal ready, save into the data buffer. */
      /* Note: read RBR will clear the interrupt */
      UARTBuffer[UARTCount++] = LPC_UART->RBR;
      if (UARTCount == BUFSIZE)
      {
        UARTCount = 0;		/* buffer overflow */
      }	
    }
  }
  else if (IIRValue == IIR_RDA)	/* Receive Data Available */
  {
    /* Receive Data Available */
    UARTBuffer[UARTCount++] = LPC_UART->RBR;
    if (UARTCount == BUFSIZE)
    {
      UARTCount = 0;		/* buffer overflow */
    }
  }
  else if (IIRValue == IIR_CTI)	/* Character timeout indicator */
  {
    /* Character Time-out indicator */
    UARTStatus |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if (IIRValue == IIR_THRE)	/* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    LSRValue = LPC_UART->LSR;		/* Check status in the LSR to see if
							valid data in U0THR or not */
    if (LSRValue & LSR_THRE)
    {
      UART0TxEmpty = 1;
    }
    else
    {
      UART0TxEmpty = 0;
    }
  }
  return;
}
#endif

/*****************************************************************************
** Function name:		RS485Init
**
** Descriptions:		Initialize UART0 port, setup pin select,
**				clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate
** Returned value:		None
** 
*****************************************************************************/
void RS485Init(uint32_t baudrate)
{
  uint32_t Fdiv;
  uint32_t regVal;

  UARTCount = 0;

#if CONFIG_RS485_ENABLE_UART_IRQHANDLER==1
  NVIC_DisableIRQ(UART_IRQn);
#endif

  LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
  LPC_IOCON->PIO1_7 &= ~0x07;	
  LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */
  /* Enable UART clock */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
  LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

  LPC_UART->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
  regVal = LPC_SYSCON->UARTCLKDIV;
  Fdiv = (((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/regVal)/16)/baudrate;	/*baud rate */

  LPC_UART->DLM = Fdiv / 256;							
  LPC_UART->DLL = Fdiv % 256;
  LPC_UART->LCR = 0x03;		/* DLAB = 0 */
  LPC_UART->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

  /* Read to clear the line status. */
  regVal = LPC_UART->LSR;
  /* Ensure a clean start, no data in either TX or RX FIFO. */
  while ( (LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
  while ( LPC_UART->LSR & LSR_RDR )
  {
	regVal = LPC_UART->RBR;	/* Dump data from RX FIFO */
  }

  LPC_UART->LCR |= ((0x3<<4)|(0x1<<3));		/* forced "0" sticky parity, parity enable */
  LPC_UART->ADRMATCH = RS485_SLAVE_ADR;		/* UARTA 485 address, 0xC0 */
#if RS485_NMM
  LPC_UART->RS485CTRL = RS485_NMMEN;		/* NMM mode */
#else
  LPC_UART->RS485CTRL = RS485_NMMEN|RS485_AADEN|RS485_RXDIS;	/* AAD mode */
#endif

  /* Below is the direction control setting. */
#if DIR_CTRL
  /* Either DTR or RTS can be used as direction pin out. */
#if 1
  LPC_IOCON->PIO1_5 &= ~0x07;	
  LPC_IOCON->PIO1_5 |= 0x01;		/* UART_DIR or UART_RTS(HB1) as direction control. */
  LPC_UART->RS485CTRL |= RS485_DCTRL;
#else
  LPC_IOCON->PIO2_0 &= ~0x07;	
  LPC_IOCON->PIO2_0 |= 0x01;		/* UART_DTR as direction contronl */
  LPC_UART->RS485CTRL |= (RS485_DCTRL|RS485_SEL);
#endif
#endif

#if CONFIG_RS485_ENABLE_UART_IRQHANDLER==1
  /* Enable the UART Interrupt */
  NVIC_EnableIRQ(UART_IRQn);
  LPC_UART->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART interrupt */
#endif
  return;
}

/*****************************************************************************
** Function name:		RS485Send
**
** Descriptions:		Send a block of data to the UART 0 port based
**				on the data length
**
** parameters:		buffer pointer, and data length
** Returned value:	None
** 
*****************************************************************************/
void RS485Send(uint8_t *BufferPtr, uint32_t Length)
{
  uint32_t i;
  
  for ( i = 0; i < Length; i++ )
  {
	/* THRE status, contain valid data */
    while ( !(UART0TxEmpty & 0x01) );
    LPC_UART->THR = *BufferPtr;
    UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */
	/* At this point, bit 5 of LCR is always 1 for RS485 test */
	if ( i == 0 )
	  LPC_UART->LCR &= ~(0x1<<4);		/* Sticky parity to 1 */
	else
	  LPC_UART->LCR |= (0x1<<4);		/* Sticky parity to 0 */
	LPC_UART->LCR |= (0x1<<3);
	BufferPtr++;
  }
  return;
}
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
