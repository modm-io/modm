/**
 * Bit definitions for LPC11 UART.
 *
 * May be added to ext directory.
 */

#ifndef XPCC__LPC11_UART_REGISTERS_HPP_
#define XPCC__LPC11_UART_REGISTERS_HPP_

// ----------------------------------------------------------------------------

#define IER_RBRIE		(1 << 0)		///< RBR Interrupt Enable. Enables the Receive Data Available 0 interrupt
#define IER_THREIE		(1 << 1)		///< THRE Interrupt Enable. Enables the Transmit Register empty Interrupt.
#define IER_RXLIE		(1 << 2)		///< RX Line Interrupt Enable.

#define IIR_INTSTATUS	(1 << 0)		///< Interrupt status.
#define IIR_INTID_RLS	(0x03 << 1)		///< Interrupt identification: Receive Line Status.
#define IIR_INTID_RDA	(0x02 << 1)		///< Interrupt identification: Receive Data Available.
#define IIR_INTID_CTI	(0x06 << 1)		///< Interrupt identification: Character Time-out Indicator.
#define IIR_INTID_THRE	(0x01 << 1)		///< Interrupt identification: THRE Interrupt.
#define IIR_INTID_MI	(0x00 << 1)		///< Interrupt identification: Modem Interrupt.
#define IIR_INTID_MASK	(IIR_INTID_RLS | IIR_INTID_RDA | IIR_INTID_CTI | IIR_INTID_THRE | IIR_INTID_MI)

// -----  Register bits of Line status register (LPC_UART->LSR)  -----
#define LSR_RDR		0x01	///< Receiver Data Ready.
#define LSR_OE		0x02	///< Overrun Error.
#define LSR_PE		0x04	///< Parity Error.
#define LSR_FE		0x08	///< Framing Error.
#define LSR_BI		0x10	///< Break Interrupt.
#define LSR_THRE	0x20	///< Transmitter Holding Register Empty. It is possible to write data the the FIFO.
#define LSR_TEMT	0x40	///< Transmitter Empty.
#define LSR_RXFE	0x80	///< Error in RX FIFO.

// -----  Register bits of Line control register (LPC_UART->LCR)  -----
#define LCR_WLS_5_BIT	((0x00) << 0)	///< 5-bit character length.
#define LCR_WLS_6_BIT	((0x01) << 0)	///< 6-bit character length.
#define LCR_WLS_7_BIT	((0x02) << 0)	///< 7-bit character length.
#define LCR_WLS_8_BIT	((0x03) << 0)	///< 8-bit character length.
#define LCR_SBS			(    1	<< 2)	///< Stop Bit Select.
#define LCR_PE			(    1 	<< 3)	///< Parity Enable.
#define LCR_PS_ODD		((0x00) << 4)	///< Odd parity.
#define LCR_PS_EVEN		((0x01) << 4)	///< Even parity.
#define LCR_PS_1_STICKY	((0x02) << 4)	///< Forced 1 stick parity.
#define LCR_PS_0_STICKY	((0x03) << 4)	///< Forced 0 stick parity.
#define LCR_BC			(    1 	<< 6)	///< Break control.
#define LCR_DLAB 		(    1 	<< 7)	///< Divisor Latch Access Bit.

// -----  Register bits of FIFO control register (LPC_UART->FCR)  -----
#define FCR_FIFOEN		(1 << 0)		///< FIFO Enable.
#define FCR_RXFIFORES	(1 << 1)		///< RX FIFO Reset.
#define FCR_TXFIFORES	(1 << 2)		///< TX FIFO Reset.
#define FCR_RXTL_1_CHR	((0x00) << 6)	///< RX Trigger level:  1 character
#define FCR_RXTL_4_CHR	((0x01) << 6)	///< RX Trigger level:  4 characters
#define FCR_RXTL_8_CHR	((0x02) << 6)	///< RX Trigger level:  8 characters
#define FCR_RXTL_14_CHR	((0x03) << 6)	///< RX Trigger level: 14 characters

#endif /* XPCC__LPC11_UART_REGISTERS_HPP_ */
