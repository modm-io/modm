/*
 * Copyright (c) 2013, Fabian Greif
 * Copyright (c) 2014, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HOSTED_STATIC_SERIAL_INTERFACE_HPP
#define MODM_HOSTED_STATIC_SERIAL_INTERFACE_HPP

#include <modm/architecture/interface/uart.hpp>

#include "serial_interface.hpp"

namespace modm
{
	namespace platform
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
		class StaticSerialInterface : public modm::Uart
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

#endif	// MODM_HOSTED_STATIC_SERIAL_INTERFACE_HPP
