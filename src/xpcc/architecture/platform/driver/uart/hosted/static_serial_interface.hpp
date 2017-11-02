/*
 * Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HOSTED_STATIC_SERIAL_INTERFACE_HPP
#define XPCC_HOSTED_STATIC_SERIAL_INTERFACE_HPP

#include <xpcc/architecture/interface/uart.hpp>

#include "serial_interface.hpp"

namespace xpcc
{
	namespace hosted
	{
		/**
		 * Wrapper with static methods for the SerialInterface class.
		 *
		 * @warning Must be used with care as only a fixed number of different
		 *          instances can be generated. Using the same type (equal template
		 *          specialization number) will generate a shared object!
		 *
		 * @author  Fabian Greif
		 * @ingroup	linux
		 */
		template<int N>
		class StaticSerialInterface : public xpcc::Uart
		{
		public:
			/**
			 * Initializes the hardware and sets the baudrate.
			 *
			 * @tparam	baudrate
			 *		desired baud rate in Hz
			 */
			template<uint32_t baudrate>
			static bool
			initialize(SerialInterface& interface);

			/**
			 * Write byte.
			 *
			 * No blocking write available. Data will added to the normal
			 * transmit queue.
			 *
			 * @param data
			 *     Byte to transmit
			 */
			static void
			writeBlocking(uint8_t data);

			/**
			 * Block write.
			 *
			 * No blocking write available. Data will added to the normal
			 * transmit queue.
			 *
			 * @param data
			 *     Byte to transmit
			 */
			static void
			writeBlocking(const uint8_t *data, std::size_t length);

			static void
			flushWriteBuffer();


			static bool
			write(uint8_t data);

			static std::size_t
			write(const uint8_t *data, std::size_t length);

			static bool
			isWriteFinished();

			static bool
			read(uint8_t &data);

			static std::size_t
			read(uint8_t *buffer, std::size_t length);

			static std::size_t
			discardReceiveBuffer();

			static std::size_t
			discardTransmitBuffer();

		private:
			static SerialInterface* backend;
		};
	}
}

#include "static_serial_interface_impl.hpp"


#endif	// XPCC_HOSTED_STATIC_SERIAL_INTERFACE_HPP
