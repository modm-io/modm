/*
 * Copyright (c) 2017-2018, Niklas Hauser
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <type_traits>

#include "../device.hpp"
#include "config.hpp"
#include "modm/architecture/interface.hpp"

namespace modm::platform
{

enum class InputType
{
	Floating = 0x0,  ///< floating on input
	PullUp = 0x1,    ///< pull-up on input
	PullDown = 0x2,  ///< pull-down on input
};

enum class OutputType
{
	PushPull = 0x0,   ///< push-pull on output
	OpenDrain = 0x1,  ///< open-drain on output
};

enum class OutputSpeed
{
	Low = 0,
	Medium = 0x1,
	High = 0x2,
	VeryHigh = 0x3,  ///< 30 pF (80 MHz Output max speed on 15 pF)
	MHz2 = Low,
	MHz25 = Medium,
	MHz50 = High,
	MHz100 = VeryHigh,
};

enum class PeripheralPin
{
	BitBang,
	Rx,
	Tx,
	ExtInt,
	Dm,
	Dp,
	Wo,
};

template<typename... Tuples>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Tuples>()...));

template<class Predicate, class Tuple>
struct tuple_filter;

template<class Predicate, class Tuple>
using tuple_filter_t = typename tuple_filter<Predicate, Tuple>::type;

// Recursive case
template<class Predicate, class X, class... Xs>
struct tuple_filter<Predicate, std::tuple<X, Xs...>>
{
	using type =
		std::conditional_t<Predicate::template value<X>,
						   tuple_cat_t<std::tuple<X>, tuple_filter_t<Predicate, std::tuple<Xs...>>>,
						   tuple_filter_t<Predicate, std::tuple<Xs...>>>;
};

// Base case
template<class Predicate>
struct tuple_filter<Predicate, std::tuple<>>
{
	using type = std::tuple<>;
};

template<class P>
struct EqualsPeripheral
{
	template<class Signal>
	static constexpr bool value = std::is_same_v<P, typename Signal::peripheral>;
};

template<class S>
struct EqualsSignal
{
	template<class Signal>
	static constexpr bool value = std::is_same_v<S, typename Signal::signal>;
};

template<class... Signals>
struct OneOfSignals
{
	template<class Signal>
	static constexpr bool value = ((std::is_same_v<typename Signal::signal, Signals>) | ...);
};

template<class... PinConfigs>
struct PinCfgMixin
{
	inline static void set(uint8_t){};
};

template<class PinConfig, class... PinConfigs>
struct PinCfgMixin<PinConfig, PinConfigs...>
{
	inline static void
	set(uint8_t cfg)
	{
		if constexpr (PinConfig::port == PortName::A)
		{
			PORT->Group[0].PINCFG[PinConfig::pin].reg = cfg;
		} else if constexpr (PinConfig::port == PortName::B)
		{
			PORT->Group[1].PINCFG[PinConfig::pin].reg = cfg;
		}
		PinCfgMixin<PinConfigs...>::set(cfg);
	}
};

template<class... PinConfigs>
class GpioSet : protected PinCfgMixin<PinConfigs...>
{
protected:
	using PinCfg = PinCfgMixin<PinConfigs...>;

	static constexpr uint32_t
	mask(PortName port)
	{
		return (((PinConfigs::port == port) ? 1u << PinConfigs::pin : 0u) | ...);
	}

	inline static constexpr uint32_t*
	getPortReg(size_t offset)
	{
		return (uint32_t*)(&PORT->Group[0]) + offset / sizeof(uint32_t);
	}

	inline static constexpr void
	setPortReg(size_t offset)
	{
		if constexpr (mask(PortName::A) != 0) { *getPortReg(offset) = mask(PortName::A); }
		if constexpr (mask(PortName::B) != 0) { *getPortReg(offset) = mask(PortName::B); }
	}

	template<PortName port>
	inline static constexpr uint32_t
	readPortReg(size_t offset)
	{
		if constexpr (port == PortName::A)
		{
			static_assert(mask(PortName::A) != 0,
						  "Trying to read port which is not in the GpioSet!");
			return *getPortReg(offset) & mask(PortName::A);
		} else if constexpr (port == PortName::B)
		{
			static_assert(mask(PortName::B) != 0,
						  "Trying to read port which is not in the GpioSet!");
			return *getPortReg(offset) & mask(PortName::A);
		}
	}

public:
	inline static void
	setOutput()
	{
		setPortReg(PORT_DIRSET_OFFSET);
	}

	inline static void
	setOutput(bool status)
	{
		setOutput();
		set(status);
	}

	static void setOutput(OutputType, OutputSpeed) { setOutput(); }

	static void configure(OutputType, OutputSpeed = OutputSpeed::MHz50) {}

	static void
	setInput()
	{
		setPortReg(PORT_DIRCLR_OFFSET);
	}

	static void
	setInput(InputType type)
	{
		configure(type);
		setInput();
	}

	static void
	configure(InputType type)
	{
		set(type == InputType::PullUp);
		PinCfg::set(PORT_PINCFG_INEN | (type != InputType::Floating) << PORT_PINCFG_PULLEN_Pos);
	}

	static void
	setAnalogInput()
	{}

	static void
	set()
	{
		setPortReg(PORT_OUTSET_OFFSET);
	}

	static void
	set(bool status)
	{
		if (status)
			set();
		else
			reset();
	}

	static void
	reset()
	{
		setPortReg(PORT_OUTCLR_OFFSET);
	}

	static void
	toggle()
	{
		setPortReg(PORT_OUTTGL_OFFSET);
	}

	static void
	disconnect()
	{
		setInput(InputType::Floating);
	}
};

template<class PinConfig>
class Gpio : public GpioSet<PinConfig>, public ::modm::GpioIO
{
	using Base = GpioSet<PinConfig>;

public:
	// For backwards compability with bitbang API
	using InputType = ::modm::platform::InputType;
	using OutputType = ::modm::platform::OutputType;

	template<PeripheralPin peripheral_pin_v>
	struct As;

	using Rx = As<PeripheralPin::Rx>;
	using Tx = As<PeripheralPin::Tx>;
	using ExtInt = As<PeripheralPin::ExtInt>;
	using Dm = As<PeripheralPin::Dm>;
	using Dp = As<PeripheralPin::Dp>;

	inline static bool
	read()
	{
		return Base::readPortReg<PinConfig::port>(PORT_IN_OFFSET);
	}

	inline static void
	write(bool status)
	{
		Base::set(status);
	}
};

template<class PinConfig>
template<PeripheralPin peripheral_pin_v>
struct Gpio<PinConfig>::As : public Gpio<PinConfig>
{
	static constexpr PeripheralPin peripheral_pin = peripheral_pin_v;
	using Base = Gpio<PinConfig>;

	template<class Signals, class P>
	struct ValidatePeripheral
	{
		using type = tuple_filter_t<EqualsPeripheral<P>, Signals>;
		static_assert(!std::is_same_v<type, std::tuple<>>,
					  "Gpio pin does not connect to this peripheral! (see above)");
	};

	template<class Signals, class... RequiredSignals>
	struct ValidateSignal
	{
		using type = tuple_filter_t<OneOfSignals<RequiredSignals...>, Signals>;
		static_assert(!std::is_same_v<type, std::tuple<>>,
					  "Gpio pin does not connect to any of the required signals! "
					  "(see above)");
	};

	template<class Signals>
	struct GetSingleSignal
	{
		using crash = typename Signals::errorPeripheralInstanceMatchedMoreThanOneSignal;
	};

	template<class Signal>
	struct GetSingleSignal<std::tuple<Signal>>
	{
		using type = Signal;
	};

	template<class peripheral, class... RequiredSignals>
	struct Connector
	{
		using ValidatedPeripheral =
			typename ValidatePeripheral<typename PinConfig::Signals, peripheral>::type;
		using ValidatedSignals =
			typename ValidateSignal<ValidatedPeripheral, RequiredSignals...>::type;
		using PinSignal = typename GetSingleSignal<ValidatedSignals>::type;
		using Signal = typename PinSignal::signal;

		inline static void
		connect()
		{
			Base::PinCfg::set(PORT_PINCFG_PMUXEN);
			if constexpr (PinConfig::pin & 1u)
			{
				PORT->Group[uint32_t(PinConfig::port)].PMUX[PinConfig::pin >> 1].bit.PMUXO =
					uint32_t(PinSignal::function);
			} else
			{
				PORT->Group[uint32_t(PinConfig::port)].PMUX[PinConfig::pin >> 1].bit.PMUXE =
					uint32_t(PinSignal::function);
			}
		}
	};
};

template<PeripheralPin peripheral, class... Pins>
struct GetPin;

template<PeripheralPin peripheral_pin, class... Pins>
using GetPin_t = typename GetPin<peripheral_pin, Pins...>::type;

template<PeripheralPin peripheral_pin, class Pin, class... Pins>
struct GetPin<peripheral_pin, Pin, Pins...>
{
	using type = typename std::conditional_t<peripheral_pin == Pin::peripheral_pin, Pin,
											 GetPin_t<peripheral_pin, Pins...>>;
};

template<PeripheralPin peripheral_pin>
struct GetPin<peripheral_pin>
{
	using type = void;
};

}  // namespace modm::platform
