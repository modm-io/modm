#include "uart_1.hpp"
#include <lpc11xx/cmsis/LPC11xx.h>

// ----------------------------------------------------------------------------

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

// -----  Register bits of Line status register (LPC_UART->LSR)  -----
#define LSR_RDR		0x01	///< Receiver Data Ready.
#define LSR_OE		0x02	///< Overrun Error.
#define LSR_PE		0x04	///< Parity Error.
#define LSR_FE		0x08	///< Framing Error.
#define LSR_BI		0x10	///< Break Interrupt.
#define LSR_THRE	0x20	///< Transmitter Holding Register Empty.
#define LSR_TEMT	0x40	///< Transmitter Empty.
#define LSR_RXFE	0x80	///< Error in RX FIFO.

// ----------------------------------------------------------------------------

void
xpcc::lpc::Uart1::write(uint8_t data)
{
	while ( !(LPC_UART->LSR & LSR_THRE) );
	LPC_UART->THR = data;
}

// ----------------------------------------------------------------------------

void
xpcc::lpc::Uart1::write(const uint8_t *buffer, uint8_t n)
{
	  while (n != 0)
	  {
			while ( !(LPC_UART->LSR & LSR_THRE) );
			LPC_UART->THR = *buffer;
			--n; ++buffer;
	  }
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Uart1::read(uint8_t & c)
{
	if (LPC_UART->LSR & LSR_RDR) {
		// Receive data available
		c = LPC_UART->RBR;
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------

uint8_t
xpcc::lpc::Uart1::read(uint8_t *buffer, uint8_t n)
{
	uint8_t ret = 0;

	while ((n != 0) && (LPC_UART->LSR & LSR_RDR))
	{
		// Receive data available and space in buffer left
		*buffer = LPC_UART->RBR;
		--n; ++buffer; ++ret;
	}

	return ret;
}

// ----------------------------------------------------------------------------

//void
//xpcc::lpc::Uart1::setBaudrate(uint32_t baudrate)
//{
//}

// ----------------------------------------------------------------------------

xpcc::lpc::Uart1::Uart1(uint32_t baudrate)
{
	/*  UART I/O config. PIO1_6 and PIO1_7 are always Rx/Tx */
	LPC_IOCON->PIO1_6 &= ~0x07;
	LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */

	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

	LPC_UART->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit, DLAB = 1 */
	uint32_t regVal = LPC_SYSCON->UARTCLKDIV;
	uint32_t   Fdiv = (((SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV)/regVal)/16)/baudrate ;	/*baud rate */
	LPC_UART->DLM = Fdiv >>    8;
	LPC_UART->DLL = Fdiv  & 0xff;

	LPC_UART->LCR = 0x03;		/* DLAB = 0 */
	LPC_UART->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

	/* Read to clear the line status. */
	regVal = LPC_UART->LSR;

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while (( LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART->LSR & LSR_RDR )
	{
		regVal = LPC_UART->RBR;	/* Dump data from RX FIFO */
	}

	  /* Enable the UART Interrupt */
	  NVIC_EnableIRQ(UART_IRQn);

//	#if CONFIG_UART_ENABLE_INTERRUPT==1
//	#if CONFIG_UART_ENABLE_TX_INTERRUPT==1
//	  LPC_UART->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART interrupt */
//	#else
//	  LPC_UART->IER = IER_RBR | IER_RLS;	/* Enable UART interrupt */
//	#endif
//	#endif

} // Uart1 constructor
