/*
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2013-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AD7280A_HPP
#define MODM_AD7280A_HPP

#include <stdint.h>

#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/delay.hpp>

#include <modm/io/iostream.hpp>

// Forward declaration for the Unit-tests
class Ad7280aTest;

namespace modm
{
	namespace ad7280a
	{
		enum Cell
		{
			BALANCER_OFF = 0,

			CB1 = (1 << 2),
			CB2 = (1 << 3),
			CB3 = (1 << 4),
			CB4 = (1 << 5),
			CB5 = (1 << 6),
			CB6 = (1 << 7),

			BALANCER_ALL = 0xfc,
		};

		enum Channel
		{
			// The cell voltage registers store the conversion result from
			// each cell input. The conversion result is in 12-bit straight
			// binary format.
			CELL_VOLTAGE_1 = 0x0,	// D11 to D0, Read only
			CELL_VOLTAGE_2 = 0x1,	// D11 to D0, Read only
			CELL_VOLTAGE_3 = 0x2,	// D11 to D0, Read only
			CELL_VOLTAGE_4 = 0x3,	// D11 to D0, Read only
			CELL_VOLTAGE_5 = 0x4,	// D11 to D0, Read only
			CELL_VOLTAGE_6 = 0x5,	// D11 to D0, Read only

			// The AUX ADC registers store the conversion result from each
			// auxiliary ADC input. The conversion result is in 12-bit straight
			// binary format.
			AUX_ADC_1 = 0x6,	// D11 to D0, Read only
			AUX_ADC_2 = 0x7,	// D11 to D0, Read only
			AUX_ADC_3 = 0x8,	// D11 to D0, Read only
			AUX_ADC_4 = 0x9,	// D11 to D0, Read only
			AUX_ADC_5 = 0xA,	// D11 to D0, Read only
			AUX_ADC_6 = 0xB,	// D11 to D0, Read only

			// The self-test register stores the conversion result of the ADC
			// self-test. The conversion result is in 12-bit straight binary
			// format.
			SELF_TEST = 0xC,	// D11 to D0, Read only
		};

		enum Register
		{
			// The control register is a 16-bit register that is used to
			// configure the AD7280A
			CTRL_HB = 0xD,	// D15 to D8, Read/write
			CTRL_LB = 0xE,	// D7 to D0, Read/write

			CELL_OVERVOLTAGE = 0xF,			// D7 to D0, Read/write
			CELL_UNDERVOLTAGE = 0x10,		// D7 to D0, Read/write
			AUX_ADC_OVERVOLTAGE = 0x11,		// D7 to D0, Read/write
			AUX_ADC_UNDERVOLTAGE = 0x12,	// D7 to D0, Read/write
			ALERT = 0x13,					// D7 to D0, Read/write
			CELL_BALANCE = 0x14,			// D7 to D0, Read/write

			// [D7:D3] 5-bit binary code to set the CB timer to a value
			//         from 0 minutes to 36.9 minutes, Resolution is 71,5 s.
			// [D2:D0] Reserved; set to 000
			CB1_TIMER = 0x15,	// D7 to D0, Read/write
			CB2_TIMER = 0x16,	// D7 to D0, Read/write
			CB3_TIMER = 0x17,	// D7 to D0, Read/write
			CB4_TIMER = 0x18,	// D7 to D0, Read/write
			CB5_TIMER = 0x19,	// D7 to D0, Read/write
			CB6_TIMER = 0x1A,	// D7 to D0, Read/write

			// [D7:D3] 5-bit binary code to set the PD timer to a value
			//         from 0 minutes to 36.9 minutes, Resolution is 71,5 s.
			// [D2:D0] Reserved; set to 000
			PD_TIMER = 0x1B,	// D7 to D0, Read/write

			// [D7:D2] 6-bit binary address for the register to be read,
			// [D1:D0] Reserved; set to 00.
			READ = 0x1C,			// D7 to D0, Read/write
			CNVST_CONTROL = 0x1D,	// D7 to D0, Read/write
		};

		enum Device
		{
			MASTER = 0,
		};

		struct RegisterValue
		{
			uint8_t device;
			uint8_t registerAddress;
			uint8_t value;
			bool acknowledge;
		};

		struct ConversionValue
		{
			uint8_t device;
			uint8_t channel;
			uint16_t value;			///< 12-Bit => 0..4095
			bool acknowledge;
		};

		enum Average
		{
			NO_AVERAGE = (0 << 1),		///< Single conversion only
			AVERAGE_BY_2 = (1 << 1),	///< Average over 2 samples
			AVERAGE_BY_4 = (2 << 1),	///< Average over 4 samples
			AVERAGE_BY_8 = (3 << 1),	///< Average over 8 samples
		};
	}

	/**
	 * Lithium Ion Battery Monitoring System.
	 *
	 * The AD7280A is monitor for lithium ion battery packs with up to six
	 * cells. It contains six 12-ADCs with 1µs conversion per channel and
	 * a cell balancing interface.
	 * Up to 7 devices can be added through a Daisy-Chain interface (chain of
	 * 8 devices).
	 *
	 * - cell voltage accuracy of ±1.6 mV.
	 * - Supply voltage 8-30V
	 * - six auxiliary ADC input channels
	 *
	 * Recommended SPI Clk frequency is 1 MHz. With a 1 MHz Clk, it takes
	 * approximately 1.54 ms to read back the voltage conversions on 48 channels.
	 *
	 * When addressing devices in the chain directly the frequency needs to
	 * be lower than 1 MHz because delays introduced in each stage of the chain.
	 *
	 * \tparam	Spi		SPI interface
	 * \tparam	Cs		Chip-Select Pin
	 * \tparam	Cnvst	Conversion Start Pin
	 * \tparam	N		Number of devices in a daisy chain (1..8)
	 *
	 * @ingroup driver_adc
	 */
	template <typename Spi, typename Cs, typename Cnvst, int N>
	class Ad7280a
	{
		// used for Unittests
		friend class ::Ad7280aTest;

	public:
		static void
		initialize(ad7280a::Average average = ad7280a::NO_AVERAGE);

		/*
		 * Initialize daisy chain.
		 */
		static bool
		chainSetup();

		/**
		 * Enable/Disable the six cell balance outputs.
		 */
		static void
		enableBalancer(uint8_t device, uint8_t cells);

		static bool
		performSelftest();

		static void
		softwareReset();

		/**
		 * Read a single channel
		 */
		static bool
		readChannel(uint8_t device, ad7280a::Channel channel, uint16_t *value);

		/**
		 * Perform a conversion and read the results back.
		 *
		 * \param[out]	values		Array containing the results
		 */
		static bool
		readAllChannels(uint16_t *values);

	private:
		/**
		 * Calculate the CRC for one byte
		 */
		static uint8_t
		updateCrc(uint8_t data);

		/**
		 * Calculate the CRC value for an entire message.
		 *
		 * The message needs to be right aligned, bit 0 is the MSB:
		 * \code
		 * // CRC for write operation
		 * uint8_t crc = calculateCrc(reg >> 11);
		 *
		 * // CRC for read operation
		 * uint8_t crc = calculateCrc(reg >> 10);
		 * \endcode
		 */
		static uint8_t
		calculateCrc(uint32_t data);

		static bool
		write(uint8_t device, ad7280a::Register reg, bool addressAll, uint8_t value);

		static bool
		read(uint32_t *value);


		static bool
		readRegister(ad7280a::RegisterValue* result);

		static bool
		readConversionResult(ad7280a::ConversionValue* result);


		/*static void
		dumpRegisterRead(uint32_t value);

		static void
		dumpConversion(uint32_t value);*/

		static uint8_t controlHighByte;
	};

	// ------------------------------------------------------------------------
	// Output operators
	IOStream&
	operator << (IOStream& os, const ad7280a::RegisterValue& c);

	IOStream&
	operator << (IOStream& os, const ad7280a::ConversionValue& c);
}

#include "ad7280a_impl.hpp"

#endif // MODM_AD7280A_HPP
