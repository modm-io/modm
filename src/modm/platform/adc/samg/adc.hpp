/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <modm/architecture/interface/adc.hpp>
#include <modm/math/algorithm/prescaler.hpp>
#include <modm/platform/clock/clockgen.hpp>
#include <modm/platform/gpio/connector.hpp>


namespace modm::platform
{

/// @ingroup modm_platform_adc
class Adc : modm::Adc
{
	static const modm::frequency_t MaxAdcFrequency = modm::MHz(10);

	// Work around a name collision between 'struct modm::Adc' and 'struct Adc'
	// defined in the CMSIS headers
	static inline ::Adc*
	Regs()
	{
		return (::Adc*)ADC;
	};

public:
	enum class Channel : uint8_t
	{
		Ch0 = 0,
		Ch1,
		Ch2,
		Ch3,
		Ch4,
		Ch5,
		Ch6,
		Ch7
	};

	/** Analog settling time setting
	 *
	 * Controls how many periods of ADC clock are allowed for input to settle.
	 */
	enum class SettlingTime : uint8_t
	{
		AST3 = 0,
		AST5,
		AST9,
		AST17
	};

public:
	// start inherited documentation
	template<class Pin, class... Pins>
	static inline void
	connect()
	{
		// Pins are automatically connected to the ADC when they are enabled,
		// so all this does is set the pin mode to input.
		using Connector = typename Pin::template Connector<Peripherals::Adc, Peripherals::Adc::Ad>;
		static_assert(Connector::PinSignal::AdcChannel >= 0, "Pin cannot be used as ADC input");
		Pin::setInput();

		// Call recursively for all Pins
		if constexpr (sizeof...(Pins)) { connect<Pins...>(); }
	}

	template<class SystemClock, frequency_t frequency = MHz(10), percent_t tolerance = pct(10)>
	static inline void
	initialize()
	{
		constexpr auto result = modm::Prescaler::from_function(
			SystemClock::Frequency,                 // input frequency
			frequency,                              // desired adc frequency
			0,                                      // lowest prescaler value
			255,                                    // highest prescaler value
			[](uint32_t x) { return 2 * (x + 1); }  // transform function
		);
		static_assert(result.frequency <= MaxAdcFrequency,
					  "Generated ADC frequency is above maximum frequency!");
		assertBaudrateInTolerance<result.frequency, frequency, tolerance>();

		ClockGen::enable<ClockPeripheral::Adc>();

		Regs()->ADC_MR = ADC_MR_PRESCAL(result.index) | ADC_MR_TRANSFER(2);
	}

	static inline void
	startConversion()
	{
		Regs()->ADC_CR = ADC_CR_START;
	}

	static inline bool
	isConversionFinished()
	{
		return Regs()->ADC_ISR & ADC_ISR_DRDY;
	}

	static inline uint16_t
	getValue()
	{
		return (uint16_t)(Regs()->ADC_LCDR & 0xffff);
	}

	static inline uint16_t
	readChannel(uint8_t channel)
	{
		if (!setChannel(channel)) return 0;

		startConversion();
		while (!isConversionFinished()) {}

		return getValue();
	}

	static inline bool
	setChannel(Channel channel)
	{
		return setChannel((uint8_t)channel);
	}

	static inline bool
	setChannel(uint8_t channel)
	{
		if (channel > 7) return false;
		Regs()->ADC_CHDR = 0xff;
		Regs()->ADC_CHER = (1 << channel);
		return true;
	}

	static inline void
	enableFreeRunningMode()
	{
		Regs()->ADC_MR |= ADC_MR_FREERUN;
	}

	static inline void
	disableFreeRunningMode()
	{
		Regs()->ADC_MR &= ~ADC_MR_FREERUN;
	}

	// end inherited documentation

	/** Configure the amount of time the ADC input is allowed to settle before sampling
	 */
	static inline void
	setSettlingTime(SettlingTime time)
	{
		Regs()->ADC_MR = (Regs()->ADC_MR & ~ADC_MR_SETTLING_Msk) | ADC_MR_SETTLING((uint8_t)time);
	}

	template<class Pin>
	static inline constexpr uint8_t
	getPinChannel()
	{
		using Connector = typename Pin::Ad::template Connector<Peripherals::Adc, Peripherals::Adc::Ad>;
		static_assert(Connector::PinSignal::AdcChannel >= 0, "Pin cannot be used as ADC input");
		return Connector::PinSignal::AdcChannel;
	}
};

}  // namespace modm::platform
