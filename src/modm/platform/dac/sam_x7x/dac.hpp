/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SAMX7X_DAC_HPP
#define MODM_SAMX7X_DAC_HPP

#include <cstdint>
#include <modm/math/algorithm/prescaler.hpp>
#include <modm/platform/clock/clockgen.hpp>
#include <modm/platform/gpio/connector.hpp>

namespace modm::platform
{

/**
 * Digital-to-Analog Converter (DACC)
 *
 * @author Christopher Durand
 * @ingroup modm_platform_dac
 */
class Dac : public PeripheralDriver
{
public:
	enum class Channel : uint8_t
	{
		Channel0 = 0,
		Channel1 = 1
	};

	enum class Mode
	{
		/// DAC conversion is started by writing data register
		FreeRunning = 0,
		/// DAC conversion is started by external trigger event
		Trigger = 1,
		/// Continuous conversion at a rate of DAC clock / 12
		MaxSpeed = 2
	};

	enum class TriggerSource
	{
		Datrg = 0,
		Tc0 = 1,
		Tc1 = 2,
		Tc2 = 3,
		Pwm0Event0 = 4,
		Pwm0Event1 = 5,
		Pwm1Event0 = 6,
		Pwm1Event1 = 7
	};

	enum class ChannelStatus : uint32_t
	{
		Ch0Enabled = Bit0,
		Ch1Enabled = Bit1,
		Ch0Ready = Bit8,
		Ch1Ready = Bit9
	};
	MODM_FLAGS32(ChannelStatus);

	enum class Interrupt : uint32_t
	{
		/// Set if the peripheral is ready to accept new data for channel 0.
		TxReadyCh0 = Bit0,
		/// Set if the peripheral is ready to accept new data for channel 1.
		TxReadyCh1 = Bit1,
		/// A conversion has completed for channel 0. Cleared on read of DACC_ISR.
		EnfOfConversionCh0 = Bit4,
		/// A conversion has completed for channel 1. Cleared on read of DACC_ISR.
		EnfOfConversionCh1 = Bit5
	};
	MODM_FLAGS32(Interrupt);

	/// Maximum DAC peripheral clock. The maximum sampling rate is MaxClock / 12.
	static constexpr auto MaxClock = 12_MHz;
	static constexpr size_t ResolutionBits = 12;
	static constexpr size_t MaxOutput = ((1 << ResolutionBits) - 1);

	template<typename... Pins>
	static void
	connect();

	/**
	 * Initialize the D/A converter.
	 *
	 * @tparam SystemClock System clock struct
	 * @tparam frequency Max DAC clock, sampling rate in max speed mode is frequency / 12
	 */
	template<typename SystemClock, frequency_t frequency=12_MHz, percent_t tolerance=5_pct>
	static void
	initialize();

	/**
	 * Enable DAC output for the given channel
	 * The corresponding GPIO pin will be connected to the DAC if enabled.
	 *
	 * @param chan    The channel to be enabled
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static void
	enableChannel(Channel chan);

	/**
	 * Disable DAC output for the given channel
	 *
	 * @param chan    The channel to be disabled
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static void
	disableChannel(Channel chan);

	/**
	 * Configure the channel conversion mode
	 *
	 */
	static void
	setChannelMode(Channel chan, Mode mode);

	/**
	 * Set the output voltage for a DAC channel
	 *
	 * @param chan    The channel to set
	 * @param value   The 12-bit DAC output value, range 0-4095.
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static void
	setOutput(Channel chan, uint32_t value);

	/**
	 * Set output value for Channel1
	 *
	 * Equivalent to setOutput(modm::platform::Dac::Channel1, value)
	 *
	 * @param value   The 12-bit DAC output value, range 0-4095
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static void
	setOutput0(uint32_t value);

	/**
	 * Set output value for Channel2
	 *
	 * Equivalent to setOutput(modm::platform::Dac::Channel2, value)
	 *
	 * @param value   The 12-bit DAC output value, range 0-4095
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static void
	setOutput1(uint32_t value);

	/**
	 * Set word transfer mode.
	 *
	 * If enabled every write to the data register will store two 16 bit
	 * values to the channel FIFO. This is useful with DMA to reduce the number
	 * of bus accesses.
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static void
	setWordTransferMode(bool enabled);

	/**
	 * Read channel status flags
	 */
	static ChannelStatus_t
	channelStatus();

	/**
	 * Get channel 0 data register pointer for use with DMA.
	 */
	static volatile uint32_t*
	channel0DataRegister();

	/**
	 * Get channel 1 data register pointer for use with DMA.
	 */
	static volatile uint32_t*
	channel1DataRegister();

	/**
	 * Set DAC trigger source. The setting only has an effect
	 * if the respective channel is configured in trigger mode.
	 */
	static void
	setTriggerSource(Channel channel, TriggerSource source);

	static void
	enableInterruptFlag(Interrupt_t interrupt);

	static void
	disableInterruptFlag(Interrupt_t interrupt);

	/**
	 * Read interrupt status register.
	 *
	 * End of conversion flags are cleared after reading.
	 */
	static Interrupt_t
	readInterruptFlags();

	static void
	enableInterruptVector(uint32_t priority);

	static void
	disableInterruptVector();
};

} // namespace modm::platform

#include "dac_impl.hpp"

#endif // MODM_SAMX7X_DAC_HPP
