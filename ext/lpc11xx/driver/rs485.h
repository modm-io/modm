/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx RS485 example
 *
 *   Description:
 *     This file contains RS485 code header definition.
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

#ifndef __RS485_H 
#define __RS485_H
#if CONFIG_ENABLE_DRIVER_RS485==1

/* TO test RS485, program one board with RS485_RX to 1(RX), one
board with RS485_RX to 0(TX). reset TX board to send out a string
with a RS485 slave address first, and then check the content of 
UARTBuffer in the RX board. */
#ifndef RS485_RX
#define RS485_RX			1		/* 1 is RX, 0 is TX. */
#endif
#define DIR_CTRL			0
#define RS485_NMM			1		/* 1 is NMM mode, 0 is AAD mode. */

/*if RS485_ENABLED is set, set RS485 slave address. */
#define	RS485_SLAVE_ADR 0xC0

#define IER_RBR         (0x01<<0)
#define IER_THRE        (0x01<<1)
#define IER_RLS         (0x01<<2)

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01

#define LSR_RDR         (0x01<<0)
#define LSR_OE          (0x01<<1)
#define LSR_PE          (0x01<<2)
#define LSR_FE          (0x01<<3)
#define LSR_BI          (0x01<<4)
#define LSR_THRE        (0x01<<5)
#define LSR_TEMT        (0x01<<6)
#define LSR_RXFE        (0x01<<7)

#define BUFSIZE         0x40

/* RS485 mode definition. */
#define RS485_NMMEN		(0x1<<0)
#define RS485_RXDIS		(0x1<<1)
#define RS485_AADEN		(0x1<<2)
#define RS485_SEL		(0x1<<3)
#define RS485_DCTRL		(0x1<<4)
#define RS485_OINV		(0x1<<5)


void RS485Init(uint32_t Baudrate);

#if CONFIG_RS485_DEFAULT_UART_IRQHANDLER==1
void UART_IRQHandler(void);
#endif

void RS485Send(uint8_t *BufferPtr, uint32_t Length);

#endif
#endif /* end __RS485_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
