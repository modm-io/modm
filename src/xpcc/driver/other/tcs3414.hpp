// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

/**
 * \file	tcs3414.hpp
 * \date	02 Mar 2013
 * \author	David Hebbeker
 */

#ifndef XPCC__TCS3414_HPP
#define XPCC__TCS3414_HPP

#include <stdint.h>

#include <xpcc/driver/ui/color.hpp>
#include <xpcc/driver/connectivity/i2c/write_read_adapter.hpp>

namespace xpcc
{
	/**
	 * \brief 	Settings to configure the digital color sensor Tcs3414
	 * \see		Tcs3414
	 * \ingroup	driver_other
	 */
	namespace tcs3414
	{
		/** @name Gain_Register
		 * @{
		 */

		//! \brief 	Analog gain control
		enum class Gain : uint8_t
		{
			X1	= 0b000000,	//!< x1 gain
			X4	= 0b010000,	//!< x4 gain
			X16	= 0b100000,	//!< x16 gain
			X64	= 0b110000,	//!< x64 gain
			DEFAULT = 0		//!< default value on chip reset
		};

		//! \brief 	Prescaler mode
		enum class Prescaler : uint8_t
		{
			D1	= 0b000,	//!< divide by 1
			D2	= 0b001,	//!< divide by 2
			D4	= 0b010,	//!< divide by 4
			D8	= 0b011,	//!< divide by 8
			D16	= 0b100,	//!< divide by 16
			D32	= 0b101,	//!< divide by 32
			D64	= 0b110,	//!< divide by 64
			DEFAULT = 0		//!< default value on chip reset
		};
		//! @}


		/** @name Timing register
		 * @{
		 */

		//! \brief 	Select mode how to choose the integration time
		enum class IntegrationMode : uint8_t
		{
			INTERNAL	= 0b000000,	//!< integrates with the free-running mode
			ADC_CTR		= 0b010000,	//!< use the ADC enable bit field in the control register to start and stop integration
			SYNC_NOM	= 0b100000,	//!< use synchronize signal to integrate over nominal integration time
			SYNC_COUNT	= 0b110000,	//!< integrate over pulse count pulses of the sync pin
			DEFAULT = 0				//!< default value on chip reset
		};

		//! \brief 	Integration for a fixed time
		enum class NominalIntegrationTime : uint8_t
		{
			MSEC_12		= 0b0000,	//!< integrate over 12 ms
			MSEC_100	= 0b0001,	//!< integrate over 100 ms
			MSEC_400	= 0b0010,	//!< integrate over 400 ms
			DEFAULT = 0				//!< default value on chip reset
		};

		//! \brief 	The number of pulses on sync pin to integrate over
		enum class SyncPulseCount : uint8_t
		{
			PULSES_1	= 0b0000,	//!< integrate over 1 pulses of sync pin
			PULSES_2	= 0b0001,	//!< integrate over 2 pulses of sync pin
			PULSES_4	= 0b0010,	//!< integrate over 4 pulses of sync pin
			PULSES_8	= 0b0011,	//!< integrate over 8 pulses of sync pin
			PULSES_16	= 0b0100,	//!< integrate over 16 pulses of sync pin
			PULSES_32	= 0b0101,	//!< integrate over 32 pulses of sync pin
			PULSES_64	= 0b0110,	//!< integrate over 64 pulses of sync pin
			PULSES_128	= 0b0111,	//!< integrate over 128 pulses of sync pin
			PULSES_256	= 0b1000,	//!< integrate over 256 pulses of sync pin
			DEFAULT = 0		//!< default value on chip reset
		};
		//! @}

		//! \brief 	Register addresses
		enum class RegisterAddress : uint8_t
		{
			CONTROL					= 0x00,	//!< Control of basic functions
			TIMING					= 0x01,	//!< Integration time control  @see Tcs3414::setIntegrationTime
			INTERRUPT				= 0x02,	//!< Interrupt settings
			INT_SOURCE				= 0x03,	//!< Interrupt source
			ID						= 0x04,	//!< Part number
			GAIN					= 0x07,	//!< Sensitivity settings @see Tcs3414::setGain
			LOW_THRESH_LOW_BYTE		= 0x08,	//!< Low byte of low interrupt threshold
			LOW_THRESH_HIGH_BYTE	= 0x09,	//!< High byte of low interrupt threshold
			HIGH_THRESH_LOW_BYTE	= 0x0A,	//!< Low byte of high interrupt threshold
			HIGH_THRESH_HIGH_BYTE	= 0x0B,	//!< High byte of high interrupt threshold
			// Data registers
			DATA1LOW				= 0x10,	//!< Low byte of ADC green channel
			DATA1HIGH				= 0x11,	//!< High byte of ADC green channel
			DATA2LOW				= 0x12,	//!< Low byte of ADC green channel
			DATA2HIGH				= 0x13,	//!< High byte of ADC green channel
			DATA3LOW				= 0x14,	//!< Low byte of ADC green channel
			DATA3HIGH				= 0x15,	//!< High byte of ADC green channel
			DATA4LOW				= 0x16,	//!< Low byte of ADC green channel
			DATA5HIGH				= 0x17	//!< High byte of ADC green channel
		};
	}
	
	using namespace tcs3414;

	/**
	 * \brief	TCS3414 Digital Color Sensors
	 *
	 * \todo	Not all features of the sensors are implemented in this driver
	 * 			yet.
	 *
	 * \tparam	I2CMaster	I2C interface which needs an \em initialized
	 * 						xpcc::i2c::Master
	 * \see		tcs3414
	 * \author	David Hebbeker
	 * \ingroup	driver_other
	 */
	template<typename I2cMaster>
	class Tcs3414
	{
	public:
		/**
		 * \brief 	Power up sensor and start conversions
		 * @return	false if an error occurred
		 */
		static inline bool
		initialize(){
			return writeRegister(RegisterAddress::CONTROL, 0b11);	// control to power up and start conversion
		}
		
		/**
		 * \brief 	Configures some of the most important settings for the sensor.
		 * @return	false if an error occurred
		 */
		static inline bool
		configure(
				const Gain gain = Gain::DEFAULT,
				const Prescaler prescaler = Prescaler::DEFAULT,
				const IntegrationMode mode = IntegrationMode::DEFAULT,
				const NominalIntegrationTime time = NominalIntegrationTime::DEFAULT) {
			return configure(gain, prescaler, mode, static_cast<uint8_t>(time));
		}

		/**
		 * \brief	Configures some of the most important settings for the sensor.
		 * @return	false if an error occurred
		 */
		static inline bool
		configure(
				const Gain gain = Gain::DEFAULT,
				const Prescaler prescaler = Prescaler::DEFAULT,
				const IntegrationMode mode = IntegrationMode::DEFAULT,
				const SyncPulseCount time = SyncPulseCount::DEFAULT) {
			return configure(gain, prescaler, mode, static_cast<uint8_t>(time));
		}

		/**
		 * \brief	The gain can be used to adjust the sensitivity of all ADC output
		 * 			channels.
		 * Uses RegisterAddress::GAIN.
		 *
		 * @param	gain
		 * @param	prescaler	reduces the sensitivity in order to avoid digital
		 * 			saturation by dividing all channels by the prescaler value
		 * @return	false if an error occurred
		 */
		static inline bool
		setGain(
				const Gain gain = Gain::DEFAULT,
				const Prescaler prescaler = Prescaler::DEFAULT) {
			return writeRegister(RegisterAddress::GAIN,
					static_cast<uint8_t>(gain) | static_cast<uint8_t>(prescaler));
		}

		/**
		 * \brief Sets the integration time for the ADCs.
		 * @return	false if an error occurred
		 */
		static inline bool
		setIntegrationTime(
				const IntegrationMode mode = IntegrationMode::DEFAULT,
				const NominalIntegrationTime time = NominalIntegrationTime::DEFAULT){
			return setIntegrationTime(mode, static_cast<uint8_t>(time));
		}

		/**
		 * \brief Sets the integration time for the ADCs.
		 * @return	false if an error occurred
		 */
		static inline bool
		setIntegrationTime(
				const IntegrationMode mode = IntegrationMode::DEFAULT,
				const SyncPulseCount time = SyncPulseCount::DEFAULT){
			return setIntegrationTime(mode, static_cast<uint8_t>(time));
		}

		/**
		 * @name Return already sampled color
		 * @{
		 */
		inline static uint16_t
		getOldColorGreen()	{ return data.green.get(); }

		inline static uint16_t
		getOldColorRed()		{ return data.red.get(); }

		inline static uint16_t
		getOldColorBlue()		{ return data.blue.get(); }

		inline static uint16_t
		getOldColorClear()	{ return data.clear.get(); }

		inline static xpcc::color::Rgb
		getOldColors()
		{ return {data.red.getMSB(), data.green.getMSB(), data.blue.getMSB() }; };

		// TODO use same function as getNewRelColors does!
		inline static xpcc::color::Rgb
		getOldRelColors()
		{
			return {
				data.red.getMSB()	/ data.clear.getMSB(),
				data.green.getMSB()	/ data.clear.getMSB(),
				data.blue.getMSB()	/ data.clear.getMSB()};
		};
		//!@}

		/**
		 * @name Sample and return fresh color values
		 * @{
		 */
		inline static uint16_t
		getNewColorGreen()
		{
			readRegisters(RegisterAddress::DATA1LOW, &(data.dataBytes[0]), 2);
			return data.green.get();
		}

		inline static uint16_t
		getNewColorRed()
		{
			readRegisters(RegisterAddress::DATA2LOW, &(data.dataBytes[2]), 2);
			return data.red.get();
		}

		inline static uint16_t
		getNewColorBlue()
		{
			readRegisters(RegisterAddress::DATA3LOW, &(data.dataBytes[4]), 2);
			return data.blue.get();
		}

		inline static uint16_t
		getNewColorClear()
		{
			readRegisters(RegisterAddress::DATA4LOW, &(data.dataBytes[6]), 2);
			return data.clear.get();
		}

		inline static xpcc::color::Rgb
		getNewColors()
		{
			refreshAllColors();
			return {data.red.getMSB(), data.green.getMSB(), data.blue.getMSB() };
		};

		inline static xpcc::color::Rgb
		getNewRelColors()
		{
			refreshAllColors();
			return {
				(static_cast<float>(data.red.get())	*100.0f)/ data.clear.get(),
				(static_cast<float>(data.green.get())*100.0f)/ data.clear.get(),
				(static_cast<float>(data.blue.get())	*100.0f)/ data.clear.get()};
		};
		//!@}

		inline static bool
		refreshAllColors() {
			return readRegisters(RegisterAddress::DATA1LOW,
					data.dataBytes, sizeof(data.dataBytes));
		}

		static bool
		readRegisters(
				const RegisterAddress address,
				uint8_t * const values,
				const uint8_t count = 1);

	private:

		static bool
		configure(
				const Gain gain = Gain::DEFAULT,
				const Prescaler prescaler = Prescaler::DEFAULT,
				const IntegrationMode mode = IntegrationMode::DEFAULT,
				const uint8_t time = 0);

		/**
		 * \brief Sets the integration time for the ADCs.
		 * Uses RegisterAddress::TIMING.
		 * @param	time	is written to the RegisterAddress::TIMING register
		 * @return	false if an error occurred
		 */
		static inline bool
		setIntegrationTime(
				const IntegrationMode mode = IntegrationMode::DEFAULT,
				const uint8_t time = 0){
			return writeRegister(RegisterAddress::TIMING,
					static_cast<uint8_t>(mode) | time);
		}

		static constexpr uint8_t ADDRESS = 0b0111001 << 1; // The address needs to be shifted by one the the left for the xpcc::i2c::WriteReadAdapter
		static i2c::WriteReadAdapter i2cWRadapter;
		
		class uint16_t_LOW_HIGH
		{
		private:
			uint8_t low;
			uint8_t high;
		public:
			uint16_t
			get() const
			{
				uint16_t value = low;
				value |= high << 8;
				return value;
			}
			inline uint8_t getLSB()	const { return low; }
			inline uint8_t getMSB()	const { return high; }
		} __attribute__ ((packed));

		static bool
		writeRegister(
				const RegisterAddress address,
				const uint8_t value);

		static union Data
		{
			uint8_t dataBytes[2*4];
			struct
			{
				uint16_t_LOW_HIGH green;
				uint16_t_LOW_HIGH red;
				uint16_t_LOW_HIGH blue;
				uint16_t_LOW_HIGH clear;
			} __attribute__ ((packed));
		} data;
	};
}

#include "tcs3414_impl.hpp"

#endif // XPCC__TCS3414_HPP
