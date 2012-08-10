#ifndef XPCC_LPC11__UART_1_HPP
#define XPCC_LPC11__UART_1_HPP

#include <stdint.h>

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
		 */
		class Uart1
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

		private:
			static void
			setBaudrate(uint32_t baudrate);

			static void
			configurePins(void);
		}; // Uart1 class
	} // lpc namespace
} // xpcc namespace

#endif // XPCC_LPC11__UART_1_HPP
