/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP4922_HPP
#define MODM_MCP4922_HPP

namespace modm
{
	/**
	 * \brief	Dual-channel 12-Bit Digital-to-Analog Converter (DAC)
	 *
	 * Features:
	 * - 12-Bit Resolution
	 * - 2.7 to 5.5V Supply Voltage
	 * - 6µs maximum settling time (0V to VREF)
	 * - 20Mhz SPI interface
	 * - Synchronous latching of both channels
	 *
	 * \code
	 * Vout = Vref * G * Din / 2^12 = Vref * G * Din / 4096
	 * \endcode
	 * with Din = 0..4095 and G = 1..2
	 *
	 * \see		<a href="http://ww1.microchip.com/downloads/en/devicedoc/21897a.pdf">Datasheet</a>
	 *
	 * \ingroup	driver_dac
	 * \author	Fabian Greif
	 */
	template <typename Spi, typename Cs, typename Ldac>
	class Mcp4922
	{
	public:
		/**
		 * \brief	Initialize
		 *
		 * Sets used pins as output. SPI must be initialized by the user!
		 */
		static void
		initialize();

		/**
		 * \brief	Set value for channel A
		 *
		 * \param	value		Raw value in range [0..4095]
		 * \param	doubleGain	If set it will increase the gain G from 1 to 2.
		 *
		 * \warning	Changes are not visible at the output until update() is called!
		 * \see		update()
		 */
		static void
		setChannelA(uint16_t value, bool doubleGain = false);

		/**
		 * \brief	Set value for channel B
		 *
		 * \param	value		Raw value in range [0..4095]
		 * \param	doubleGain	If set it will increase the gain G from 1 to 2.
		 *
		 * \warning	Changes are not visible at the output until update() is called!
		 * \see		update()
		 */
		static void
		setChannelB(uint16_t value, bool doubleGain = false);

		/**
		 * \brief	Update the outputs
		 *
		 * Afterwards the values set by setChannelA() and setChannelB() will
		 * be active at the two outputs.
		 */
		static void
		update();

	protected:
		/// Bit values of the configuration register
		enum Register
		{
			SHDN = (1 << 12),		///< Shutdown
			GA = (1 << 13),			///< Gain setting
			BUF = (1 << 14),		///< Buffered AREF
			CHANNEL_B = (1 << 15)	///< Select channel !A/B
		};

		/// Write a 16-bit configuration register
		static void
		writeRegister(uint16_t value);
	};
}

#include "mcp4922_impl.hpp"

#endif // MODM_MCP4922_HPP
