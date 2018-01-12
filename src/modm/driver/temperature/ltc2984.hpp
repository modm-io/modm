// coding: utf-8
/*
 * Copyright (c) 2017, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LTC2984_HPP
#define MODM_LTC2984_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

struct ltc2984
{
	struct modm_packed
	Data
	{
	public:
		enum class Status : uint8_t
		{
			ValidIf1				= Bit0,
			AdcOutOfRangeFault		= Bit1,
			SensorUnderRangeFault	= Bit2,
			SensorOverRangeFault	= Bit3,
			CJSoftFault				= Bit4,
			CJHardFault				= Bit5,
			AdcHardFault			= Bit6,
			SensorHardFault			= Bit7,
		};
		MODM_FLAGS8(Status);

		Data() : data(0)
		{
		}

		Data(uint32_t data) : data(data)
		{
		}

		uint32_t& operator=(uint32_t data)
		{
			return this->data = data;
		}

		/// @return check for a valid result
		bool
		isValid()
		{
			return bool(static_cast<Status>(this->data >> 24) & Status::ValidIf1);
		}

		/// @return status code of result
		Status
		getStatus() const
		{
			return static_cast<Status>(this->data >> 24);
		}

		/// @return the temperature as a signed float in the configured unit
		float
		getTemperatureFloat() const
		{
			return getTemperatureFixed() / 1024.f;
		}

		/// @return the temperature in the configured unit divided by 1024
		int32_t
		getTemperatureFixed() const
		{
			int32_t ret(this->data & 0xfffffful);
			return (this->data & 0x800000) ? -(((~ret) + 1) & 0xffffff) : ret;
		}

		/// @return the temperature rounded to nearest integer in the configured unit
		int16_t
		getTemperatureInteger() const
		{
			return ((getTemperatureFixed() + (1ul << 9)) / 1024);
		}

	public:
		uint32_t data;
	};

	enum class
	Register : uint16_t
	{
		CommandStatus		= 0x000,
		Results				= 0x010,
		//EepromKey			= 0x0b0,
		//EepromReadResult	= 0x0d0,
		GlobalConfiguration	= 0x0f0,
		MuxChannels			= 0x0f4,
		//EepromStatus		= 0x0f9,
		MuxConfigDelay		= 0x0f4,
		ChannelConfig		= 0x200,
		CustomDataTable		= 0x250,
	};

	enum class
	CommandStatus : uint8_t
	{
		// Channel selection 4:0
		Done = Bit6,
		Start = Bit7,

		DoneAndStart = Bit6 | Bit7,

		Sleep = 0x97,
	};
	MODM_FLAGS8(CommandStatus);

	/// List of available channels
	enum class Channel : uint8_t
	{
		Ch1 = 0,
		Ch2 = 1,
		Ch3 = 2,
		Ch4 = 3,
		Ch5 = 4,
		Ch6 = 5,
		Ch7 = 6,
		Ch8 = 7,
		Ch9 = 8,
		Ch10 = 9,
		Ch11 = 10,
		Ch12 = 11,
		Ch13 = 12,
		Ch14 = 13,
		Ch15 = 14,
		Ch16 = 15,
		Ch17 = 16,
		Ch18 = 17,
		Ch19 = 18,
		Ch20 = 19,
	};

	struct Configuration
	{
		enum class TemperatureUnit : uint8_t
		{
			Celsius			= 0b0 << 2,
			Fahrenheit		= 0b1 << 2,
		};

		enum class Rejection : uint8_t
		{
			Rejection_50Hz_60Hz				= 0b00,
			Rejection_60Hz					= 0b01,
			Rejection_50Hz					= 0b10,
			//Reserved						= 0b11,
		};

		enum class MuxChannel : uint32_t
		{
			Ch1 = Bit0,
			Ch2 = Bit1,
			Ch3 = Bit2,
			Ch4 = Bit3,
			Ch5 = Bit4,
			Ch6 = Bit5,
			Ch7 = Bit6,
			Ch8 = Bit7,
			Ch9 = Bit8,
			Ch10 = Bit9,
			Ch11 = Bit10,
			Ch12 = Bit11,
			Ch13 = Bit12,
			Ch14 = Bit13,
			Ch15 = Bit14,
			Ch16 = Bit15,
			Ch17 = Bit16,
			Ch18 = Bit17,
			Ch19 = Bit18,
			Ch20 = Bit19,
		};
		MODM_FLAGS32(MuxChannel);

		enum class SensorType : uint32_t
		{
			//Unassigned		= 0b00000ul << 27,
			ThermocoupleJ		= 0b00001ul << 27,
			ThermocoupleK		= 0b00010ul << 27,
			ThermocoupleE		= 0b00011ul << 27,
			ThermocoupleN		= 0b00100ul << 27,
			ThermocoupleR		= 0b00101ul << 27,
			ThermocoupleS		= 0b00110ul << 27,
			ThermocoupleT		= 0b00111ul << 27,
			ThermocoupleB		= 0b01000ul << 27,
			ThermocoupleCustom	= 0b01001ul << 27,
			Pt10				= 0b01010ul << 27,
			Pt50				= 0b01011ul << 27,
			Pt100				= 0b01100ul << 27,
			Pt200				= 0b01101ul << 27,
			Pt500				= 0b01110ul << 27,
			Pt1000				= 0b01111ul << 27,
			Rtd1000_000375		= 0b10000ul << 27,
			RtdNi120			= 0b10001ul << 27,
			RtdCustom			= 0b10010ul << 27,
			Thermistor2k252		= 0b10011ul << 27,
			Thermistor3k		= 0b10100ul << 27,
			Thermistor5k		= 0b10101ul << 27,
			Thermistor10k		= 0b10110ul << 27,
			Thermistor30k		= 0b10111ul << 27,
			Thermistor2k252_YSI400			= 0b11000ul << 27,
			Thermistor1k_Spectrum			= 0b11001ul << 27,
			ThermistorCustom_SteinhartHart	= 0b11010ul << 27,
			ThermistorCustom_Table			= 0b11011ul << 27,
			Diode				= 0b11100ul << 27,
			SenseResistor		= 0b11101ul << 27,
			DirectADC			= 0b11110ul << 27,
			//Reserved			= 0b11111ul << 27,
		};

		struct Rtd
		{

			enum class RsenseChannel : uint32_t
			{
				Ch2_Ch1		= 0b00010ul << 22,
				Ch3_Ch2		= 0b00011ul << 22,
				Ch4_Ch3		= 0b00100ul << 22,
				Ch5_Ch4		= 0b00101ul << 22,
				Ch6_Ch5		= 0b00110ul << 22,
				Ch7_Ch6		= 0b00111ul << 22,
				Ch8_Ch7		= 0b01000ul << 22,
				Ch9_Ch8		= 0b01001ul << 22,
				Ch10_Ch9	= 0b01010ul << 22,
				Ch11_Ch10	= 0b01011ul << 22,
				Ch12_Ch11	= 0b01100ul << 22,
				Ch13_Ch12	= 0b01101ul << 22,
				Ch14_Ch13	= 0b01110ul << 22,
				Ch15_Ch14	= 0b01111ul << 22,
				Ch16_Ch15	= 0b10000ul << 22,
				Ch17_Ch16	= 0b10001ul << 22,
				Ch18_Ch17	= 0b10010ul << 22,
				Ch19_Ch18	= 0b10011ul << 22,
				Ch20_Ch19	= 0b10100ul << 22,
			};

			enum class Wires : uint32_t
			{
				Wire2 					= 0b00ul << 20,
				Wire3					= 0b01ul << 20,
				Wire4					= 0b10ul << 20,
				Wire4_KelvinRsense		= 0b11ul << 20,
			};

			enum class ExcitationMode : uint32_t
			{
				NoRotation_NoSharing	= 0b00ul << 18,
				NoRotation_Sharing		= 0b01ul << 18,
				Rotation_Sharing		= 0b10ul << 18,
			};

			enum class ExcitationCurrent : uint32_t
			{
				//Reserved				= 0b0000ul << 14,
				Current_5uA				= 0b0001ul << 14,
				Current_10uA			= 0b0010ul << 14,
				Current_25uA			= 0b0011ul << 14,
				Current_50uA			= 0b0100ul << 14,
				Current_100uA			= 0b0101ul << 14,
				Current_250uA			= 0b0110ul << 14,
				Current_500uA			= 0b0111ul << 14,
				Current_1mA				= 0b1000ul << 14,
			};

			enum class RtdCurve : uint32_t
			{
				European				= 0b00ul << 12,
				American				= 0b01ul << 12,
				Japanese				= 0b10ul << 12,
				ITS_90					= 0b11ul << 12,
			};
		};

		struct Rsense
		{
			/*enum class SensorType : uint32_t
			 * {
			 *  SenseResistor = 0b11101ul << 27,
			};*/

			// Rsense resistance value in Resistance_t has a width of 27 bits
			// Unit: 1/1024 ohms
			typedef uint32_t Resistance_t;
		};

		static constexpr uint32_t disabled() { return 0x00000000; }

		static constexpr uint32_t rtd(
		    const ltc2984::Configuration::SensorType sensorType,
		    const ltc2984::Configuration::Rtd::RsenseChannel rsenseChannel,
		    const ltc2984::Configuration::Rtd::Wires wires,
		    const ltc2984::Configuration::Rtd::ExcitationMode excitationMode,
		    const ltc2984::Configuration::Rtd::ExcitationCurrent excitationCurrent,
		    const ltc2984::Configuration::Rtd::RtdCurve rtdCurve)
		{
			return static_cast<uint32_t>(sensorType)
			        | static_cast<uint32_t>(rsenseChannel)
			        | static_cast<uint32_t>(wires)
			        | static_cast<uint32_t>(excitationMode)
			        | static_cast<uint32_t>(excitationCurrent)
			        | static_cast<uint32_t>(rtdCurve);
		}

		static constexpr uint32_t rsense(const ltc2984::Configuration::Rsense::Resistance_t resistance)
		{
			return static_cast<uint32_t>(ltc2984::Configuration::SensorType::SenseResistor)
			        | resistance;
		}
	};
};

inline ltc2984::Register operator +(ltc2984::Register r, ltc2984::Channel c)
{
	return static_cast<ltc2984::Register>(static_cast<uint16_t>(r) + (static_cast<uint16_t>(c)*4));
}

inline ltc2984::CommandStatus operator |(ltc2984::CommandStatus r, ltc2984::Channel c)
{
	return static_cast<ltc2984::CommandStatus>(static_cast<uint8_t>(r) | (static_cast<uint8_t>(c) + 1));
}

/**
 * \brief	LTC2983/LTC2984: Integrated temperature measurement system (20 channel)
 *
 * The integrated EEPROM of the LTC2984 is not supported by now.
 * LTC2984 and LTC2983 are 100% compatible except the LTC2983 has no integrated EEPROM.
 *
 * \see		<a href="http://cds.linear.com/docs/en/datasheet/2984fb.pdf">Datasheet</a>
 *
 * \ingroup	driver_temperature
 * \author	Raphael Lehmann
 */
template < class SpiMaster, class Cs >
class Ltc2984 : public ltc2984, public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<3>
{
public:
	/**
	 * \brief	Initialize
	 *
	 * Sets used pins as output. SPI must be initialized by the user!
	 */
	Ltc2984();

	/**
	 * \brief	Ping the LTC298x device.
	 * \warning Do not use ping() while any measurement is running.
	 * The ping() function writes data to the Custom Sensor Table Data memory
	 * region but restores the data before finishing the device available check.
	 *
	 * \return	config	True if the device is reachable.
	 */
	modm::ResumableResult<bool>
	ping();

	/**
	 * \brief	Configure
	 *
	 * \param	rejection		LTC2983/LTC2984 hat built-in simultaneous 50Hz/60Hz Rejection,
	 *								default both are activated
	 * \param	temperatureUnit	Temperature unit to use, °C or °F
	 * \param	muxDelay		Mux configuration delay in 100 microseconds (maximum: 255 x 100us = 25.5ms)
	 */
	modm::ResumableResult<void>
	configure(ltc2984::Configuration::Rejection rejection,
	          ltc2984::Configuration::TemperatureUnit temperatureUnit,
	          uint8_t muxDelay = 0);


	/**
	 * \brief	Check if device is busy
	 *
	 * \return True if device is busy.
	 */
	modm::ResumableResult<bool>
	isBusy();

	/**
	 * \brief	Configure channel
	 *
	 * \param	channel	The channel to configure.
	 * \param	config	The configuration for the channel.
	 */
	modm::ResumableResult<void>
	configureChannel(ltc2984::Channel channel, uint32_t config);

	/**
	 * \brief	Initiate single conversion
	 *
	 * \param	channel	The channel to measure.
	 */
	modm::ResumableResult<void>
	initiateSingleMeasurement(ltc2984::Channel channel);

	/**
	 * \brief	Initiate measurements for all actived channels
	 */
	modm::ResumableResult<void>
	initiateMeasurements();

	/**
	 * \brief	Enable channel for multiplexed conversation.
	 *
	 * This has no effect until setChannels() is called
	 *
	 * \param	channel	The channel to activate.
	 */
	void
	enableChannel(ltc2984::Configuration::MuxChannel channel);

	/**
	 * \brief	Deactivate channel for multiplexed conversation.
	 *
	 * This has no effect until setChannels() is called
	 *
	 * \param	channel	The channel to disable.
	 */
	void
	disableChannel(ltc2984::Configuration::MuxChannel channel);

	/**
	 * \brief	Write actived channel configuartion to device
	 */
	modm::ResumableResult<void>
	setChannels();

	/**
	 * \brief	Enter sleep mode
	 */
	modm::ResumableResult<void>
	enterSleepMode();

	/**
	 * \brief	Read temperature value.
	 *
	 * \param	channel	The channel to read the temperature value from.
	 * \param	value	The temperature value as configured before.
	 * \return	True if result is valid.
	 */
	modm::ResumableResult<void>
	readChannel(ltc2984::Channel channel, ltc2984::Data& value);

protected:
	/// Write 32-bit word to ltc2984 memory
	modm::ResumableResult<void>
	writeData(Register address, uint8_t* word, size_t length);
	modm::ResumableResult<void>
	writeData(Register address, CommandStatus command);

	/// Read 32-bit word to ltc2984 memory
	modm::ResumableResult<void>
	readFourBytes(Register address, uint8_t* data);
	modm::ResumableResult<void>
	readByte(Register address, uint8_t& byte);

private:
	// write read bit on the address
	static constexpr uint8_t Read = 0x03;
	static constexpr uint8_t Write = 0x02;

	Configuration::MuxChannel_t enabledChannels;

	uint8_t buffer[7];
};

// ------------------------------------------------------------------------
// Output operators
IOStream&
operator << (IOStream& os, const ltc2984::Data& c);

IOStream&
operator << (IOStream& os, const ltc2984::Data::Status& c);

}

#include "ltc2984_impl.hpp"

#endif // MODM_LTC2984_HPP
