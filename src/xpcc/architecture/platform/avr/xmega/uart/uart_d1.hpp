// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__UART_D1_HPP
#define XPCC_ATXMEGA__UART_D1_HPP

#ifdef USARTD1

#include <stdint.h>

/*
 * Error messages for linker.  
 */
extern void dummy_YOUR_BAUDRATE_IS_NOT_CONSTANT(void);
extern void dummy_YOUR_BAUDRATE_IS_TOO_HIGH_FOR_CPU_FREQUENCY(void);

namespace xpcc
{
	namespace xmega
	{
		/**
		 * \brief		UARTD1 Baud rate setting
		 * \ingroup		xmega_uart
		 * 
		 * Baud rate setting for Uart and BufferedUart
		 */
		class UartBaudD1 : public UartBase
		{
		public:
			ALWAYS_INLINE static void
			setBaudrate(uint32_t baudrate)
			{
				/*
				 * Check if baudrate is a constant value at compile time so
				 * that all the optimisations can be performed. Calculating
				 * the baud rate registers at runtime is a no-go.
				 * 
				 * The idea to use an external function to generate messages
				 * at link time is from:
				 * http://stackoverflow.com/questions/6491163/is-it-possible-to-write-these-pure-assert-and-const-assert-macros
				 */
				if (!__builtin_constant_p(baudrate))
				{
					dummy_YOUR_BAUDRATE_IS_NOT_CONSTANT();
				}
				
				/*
				 * Assert that the baud rate is less or equal F_CPU/16 which is the maximum
				 * for normal speed mode.
				 */
				if ((F_CPU / 16) < baudrate)
				{
					dummy_YOUR_BAUDRATE_IS_TOO_HIGH_FOR_CPU_FREQUENCY();
				}
		
				uint16_t bsel = bSel(baudrate);
				uint8_t  bscale = bScaleUnsigned(baudrate);
		
				USARTD1.BAUDCTRLA = static_cast<uint8_t>(bsel & 0xff);
				USARTD1.BAUDCTRLB = static_cast<uint8_t>((bscale << USART_BSCALE0_bp) | (bsel >> 8));
			}
		};
			
		/**
		 * \brief		UARTD1
		 * \ingroup		xmega_uart
		 */
		class UartD1 : public UartBase
		{
		public:
			/**
			 * \brief	Initialise the UART and set baud rate
			 *
			 * The constructor takes a constant value as a parameter and the
			 * compiler will calculate the baud rate register including the
			 * fractional baud rate register for optimal performance and
			 * accuracy.
			 * All the calculation is done by the compiler, so no 32-bit
			 * arithmetic is needed at run-time!
			 *
			 * If you do not provide a constant baud rate or your baud
			 * rate is too high the linker (!) will emit error messages
			 * by complaining about undefined functions. The function name
			 * is the error message. 
			 *
			 * \param	baudrate	desired baud rate, must be constant at compile time
			 */
			UartD1(uint32_t baudrate)
			{
				UartBaudD1::setBaudrate(baudrate);
				this->initialize(); 
			}
			
			/* **************************************************** *
			 * The four standard IO methods for all UARTs and alike *
			 * **************************************************** */
			
			/**
			 * \brief	Write a single byte
			 *
			 * \param	data	Single byte to write
			 */
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to buffer with data to write
			 * \param	nbyte	Number of bytes to write
			 */
			static void
			write(const uint8_t *buffer, uint8_t nbyte);

			static bool
			read(uint8_t& data);

			/**
			 * \brief	Read a block of bytes
			 *
			 * This is non-blocking. If no data is available this function waits
			 * about 1.5 times of a frame length.
			 * This is only correct at a baud rate of 115200.
			 *
			 * \param	*buffer	Pointer to a buffer big enough to store \a n bytes
			 * \param	nbyte	Number of bytes to be read
			 *
			 * \return	Number of bytes which could be read, maximum \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t nbyte);
			
		protected:
			/**
			 * \brief	Initialise UART module
			 * 
			 * Set the output and input pins, the interrupts and enables transceiver and receiver. 
			 * Does *not* set the baudrate. This should be done *before*.
			 */
			static void
			initialize();
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Buffered Uart
		 * 
		 * This implementation uses a ringbuffer. The size of the ringbuffer
		 * can be changed in the \c project.cfg file.
		 * 
		 * Just add add a value for UARTD1_TX_BUFFER_SIZE and/or 
		 * UARTD1_RX_BUFFER_SIZE in the \c defines section. The size can
		 * be any value between 1 and 254.
		 * 
		 * Example:
		 * \code
		 * [defines]
		 * UARTD1_TX_BUFFER_SIZE = 20
		 * UARTD1_RX_BUFFER_SIZE = 30
		 * \endcode
		 * 
		 * \ingroup	xmega_uart
		 */
		class BufferedUartD1 : public UartBase
		{
		public:
			BufferedUartD1(uint32_t baudrate)
			{
				UartBaudD1::setBaudrate(baudrate);
				this->initialize();
			}
			
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to buffer with data to write
			 * \param	nbyte	Number of bytes to write
			 */
			static void
			write(const uint8_t *buffer, uint8_t nbyte);
			
			static bool
			read(uint8_t& data);
			
			static uint8_t
			read(uint8_t *buffer, uint8_t nbyte);
			
			/**
			 * \brief	Check whether any errors occurred during receiving
			 *			Be aware that these indicate an error that occurred
			 *			somewhere since resetting the Error Flags (with 
			 *			resetErrorFlags()), so you cannot tell which byte had
			 *			the error.
			 *
			 * \return	\c 0 if no errors occurred, otherwise a value that
			 *			corresponds to the Error Flags in the status register.
			 */
			static uint8_t
			readErrorFlags();

			static void
			resetErrorFlags();

			/**
			 * \brief	Empty the receive FIFO queue and UART buffer.
			 *
			 * \return	the size of the deleted FIFO queue.
			 */
			static uint8_t
			flushReceiveBuffer();
			
//			static uint8_t
//			flushTransmitBuffer();
			
		protected:
			static void			
			initialize();
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief		UARTD1 buffered with RTS/CTS flow control
		 * 
		 * CTS is always an output and should be connected to the RTS input of
		 * the neighbour. 
		 * 
		 * \verbatim
		 * 
		 * AVR                                Device
		 * ===                                ======
		 * 
		 * RTS  (IN) ---<<<---, ,--->>>--- (IN)  RTS
		 *                     X
		 * CTS (OUT) --->>>---' '---<<<--- (OUT) CTS
		 * 
		 * \endverbatim
		 * 
		 * HIGH indicates that the receiver is NOT ready to accept data. 
		 * 
		 * \ingroup		xmega_uart
		 */
		class BufferedUartFlowD1 : public UartBase
		{
		public:
			BufferedUartFlowD1(uint32_t baudrate)
			{
				UartBaudD1::setBaudrate(baudrate);
				this->initialize();
			}
			
			static void
			write(uint8_t data);
			
			static void
			write(const uint8_t *buffer, uint8_t nbyte);
			
			static bool
			read(uint8_t& data);
			
			static uint8_t
			read(uint8_t *buffer, uint8_t nbyte);
			
			static uint8_t
			readErrorFlags();

			static void
			resetErrorFlags();

			static uint8_t
			flushReceiveBuffer();
			
//			static uint8_t
//			flushTransmitBuffer();

		protected:
			static void			
			initialize();
		};
	}
}

#endif // USARTD1
#endif // XPCC_ATXMEGA__UART_D1_HPP