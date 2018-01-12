// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AD7928_HPP
#define MODM_AD7928_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/accessor.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/processing/processing.hpp>

namespace modm
{

struct ad7928
{
public:
	/// Control register
	enum class
	ControlRegister : uint16_t
	{
		/// Enables writing to the control register
		WriteControlReg = Bit15,

		// bit 14: SEQ bit, see SequenceMode
		// bit 13: don't care
		// bit 12,11,10: channel selection, see InputChannel
		// bit 9,8: power mode, see PowerMode
		// bit 7: SHADOW bit, see SequenceMode
		// bit 6: don't care

		/// If this bit is set, the input range is 0..Vref,
		/// else 0..2*Vref (4.75V < AVdd < 5.25V required)
		VrefRange = Bit5,

		/// Unsigned output, else two's complement signed output for
		/// differential signals (output 0 for input Vref/2)
		UnsignedOutput = Bit4
	};

	MODM_FLAGS16(ControlRegister);

	/// Sequence mode
	enum class
	SequenceMode : uint16_t
	{
		/// Sequence mode disabled
		NoSequence = 0,

		/// If this bit is set, the next 2 bytes written to the device
		/// will be clocked into the shadow register and non-continuous
		/// sequence mode will be enabled
		ProgramShadowRegister = Bit7,

		/// Continue the current sequence, conversion settings can be changed
		ContinueSequence = Bit14,

		/// Enable continuous sequence mode, not supported
		ContinuousSequence = Bit14 | Bit7
	};
	typedef modm::Configuration<ControlRegister_t, SequenceMode, Bit14 | Bit7> SequenceMode_t;

	/// ADC input channels
	enum class
	InputChannel : uint8_t
	{
		Ch0 = 0,
		Ch1 = Bit0,
		Ch2 = Bit1,
		Ch3 = Bit1 | Bit0,
		Ch4 = Bit2,
		Ch5 = Bit2 | Bit0,
		Ch6 = Bit2 | Bit1,
		Ch7 = Bit2 | Bit1 | Bit0
	};
	typedef modm::Configuration<ControlRegister_t, InputChannel, 0b111, 10> InputChannel_t;

	/// Power mode
	enum class
	PowerMode : uint8_t
	{
		Normal = Bit1 | Bit0,
		FullShutdown = Bit1,
		AutoShutdown = Bit0
	};
	typedef modm::Configuration<ControlRegister_t, PowerMode, 0b11, 8> PowerMode_t;

	/// Shadow register, used for channel selection in sequence mode
	enum class
	ShadowRegister : uint16_t
	{};
	MODM_FLAGS16(ShadowRegister);

	/// Configuration to select input channels for sequence conversion
	enum class
	SequenceChannels : uint8_t
	{
		Ch0 = Bit7,
		Ch1 = Bit6,
		Ch2 = Bit5,
		Ch3 = Bit4,
		Ch4 = Bit3,
		Ch5 = Bit2,
		Ch6 = Bit1,
		Ch7 = Bit0
	};
	MODM_FLAGS8(SequenceChannels);

	typedef modm::Configuration<ShadowRegister_t, SequenceChannels, 0xFF, 8> Sequence1Channels_t;
	typedef modm::Configuration<ShadowRegister_t, SequenceChannels, 0xFF, 0> Sequence2Channels_t;

	using Register_t = modm::FlagsGroup<ControlRegister_t, ShadowRegister_t>;

	struct modm_packed
	Data
	{
		/// @return 12 bit result, for AD7918 and AD7908, respectively, 4 or 2 LSB are 0
		inline uint16_t
		value() const
		{
			return (data[1] & 0xFF) | ((static_cast<uint16_t>(data[0] & 0b1111)) << 8);
		}

		/// @return adc input channel
		inline InputChannel
		channel() const
		{
			return static_cast<InputChannel>((data[0] & 0b01110000) >> 4);
		}

		uint8_t data[2];
	};
}; // struct ad7928

/**
 * Driver for AD7928/AD7918/AD7908 ADC
 *
 * The AD7928/AD7918/AD7908 are, respectively, 12/10/8 bit analog-digital converters.
 * The conversion time is determined by the Spi clock frequency. A maximum Spi clock
 * of 20 Mhz is supported.
 *
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Christopher Durand
 * @ingroup driver_adc
 */
template <typename SpiMaster, typename Cs>
class Ad7928 : public ad7928, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<3>
{
public:
	Ad7928();

	/// Call this function once before using the device
	modm::ResumableResult<void>
	initialize();

	/// Initiate a single conversion and return the result of the previous conversion
	/// A running sequence will be aborted.
	/// If the device is in full shutdown, it will be woken up.
	modm::ResumableResult<Data>
	singleConversion(InputChannel channel);

	/// Start a conversion sequence.
	/// The device will automatically cycle through the specified channels, starting
	/// with the lowest channel index in sequence1, when nextSequenceConversion() is called.
	modm::ResumableResult<void>
	startSequence(SequenceChannels_t channels1, SequenceChannels_t channels2 = SequenceChannels_t(0));

	/// Perform the next sequence conversion
	/// The result is undefined if the device is not in sequence mode or not in normal power mode.
	modm::ResumableResult<Data>
	nextSequenceConversion();

	/// Enable extended range mode (0V < input < 2*Vref)
	/// The configuration will be applied after the next conversion
	/// Default mode: (0V < input < Vref)
	void
	setExtendedRange(bool enabled);

	/// Test if extended range mode is enabled
	bool
	isExtendedRange();

	/// Shutdown device after each conversion, not supported in sequence mode
	void
	setAutoShutdownEnabled(bool enabled);

	/// Test if auto-shutdown is enabled
	bool
	isAutoShutdownEnabled();

	/// Shutdown device
	/// Calling wakeup() or initiating a conversion will wake up the device
	modm::ResumableResult<void>
	fullShutdown();

	/// Wake up the device from full shutdown mode
	modm::ResumableResult<void>
	wakeup();

private:
	modm::ResumableResult<void>
	transfer(Register_t reg);

	ControlRegister_t config;
	ShadowRegister_t sequenceChannels;

	uint8_t outBuffer[2];
	Data data;

	PowerMode currentPowerMode;
};

IOStream&
operator<<(IOStream& out, const ad7928::Data& data);

} // namespace modm

#include "ad7928_impl.hpp"

#endif // MODM_AD7928_HPP
