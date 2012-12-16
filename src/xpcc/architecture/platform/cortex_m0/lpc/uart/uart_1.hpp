#ifndef XPCC_LPC11__UART_1_HPP
#define XPCC_LPC11__UART_1_HPP

#include <stdint.h>

#include "uart_base.hpp"

namespace xpcc
{
	namespace lpc
	{
		/**
		 * \brief	Universal asynchronous receiver transmitter (UART1)
		 *
		 * Simple implementation without interrupts.
		 * It is buffered in hardware with 16-bytes FIFOs (one for receive and
		 * one for transmit)
		 * 
		 * \ingroup	lpc1100
		 */
		class Uart1 : public UartBase
		{
		public:
			Uart1(uint32_t baudrate);

			/**
			 * \brief	Write a single bytes
			 *
			 * \param	data	Data to be written
			 */
			static void
			write(uint8_t data);

			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer
			 * \param	n		Number of bytes to be read
			 */
			static void
			write(const uint8_t *buffer, uint8_t n);

			/**
			 * \brief	Read a single byte
			 */
			static bool
			read(uint8_t& c);

			/**
			 * \brief	Read a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer big enough to storage \a n bytes
			 * \param	n	Number of bytes to be read
			 *
			 * \return	Number of bytes which could be read, maximal \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t n);

		protected:
//			static void inline
//			setBaudrate(uint32_t baudrate);

			static void
			configurePins(void);
		}; // Uart1 class

		/**
		 * \brief	Universal asynchronous receiver transmitter (UART1)
		 *
		 * This implementation uses the hardware buffer and the software buffer.
		 * A software buffer is only used if more than 16 bytes of buffering
		 * is requested.
		 *
		 * The hardware implementation by NXP is flawed. See
		 *   http://knowledgebase.nxp.com/showthread.php?t=2231
		 *
		 * There is no FIFO-not-full flag. It's only possible to check if the
		 * FIFO is completely empty. This makes it impossible to add data
		 * to the FIFO after the first byte is put into the FIFO.
		 *
		 * After detecting that the FIFO is empty (THRE interrupt) the
		 * charsLeft variable is set to 16 (= size of FIFO) and some accounting
		 * is done in this class.
		 *
		 * The interrupt occurence is strongly reduced compared to STM devices.
		 * In STM devices after each char transmitted an interrupt is called.
		 * In this implementation only every 16 bytes an interrupt is generated.
		 *
		 */
		class BufferedUart1 : public UartBase
		{
		public:
			BufferedUart1(uint32_t baudrate);

			/**
			 * \brief	Write a single bytes
			 *
			 * \param	data	Data to be written
			 */
			static void
			write(uint8_t data);

			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer
			 * \param	n		Number of bytes to be read
			 */
			static void
			write(const uint8_t *buffer, uint8_t n);

			/**
			 * \brief	Read a single byte
			 */
			static bool
			read(uint8_t& c);

			/**
			 * \brief	Read a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer big enough to storage \a n bytes
			 * \param	n	Number of bytes to be read
			 *
			 * \return	Number of bytes which could be read, maximal \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t n);


		};

	} // lpc namespace
} // xpcc namespace

#endif // XPCC_LPC11__UART_1_HPP
