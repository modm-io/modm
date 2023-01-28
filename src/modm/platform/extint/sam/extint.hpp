/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <functional>
#include <modm/architecture/interface/interrupt.hpp>
#include <modm/platform/clock/gclk.hpp>
#include <modm/platform/device.hpp>
#include <modm/platform/gpio/pin.hpp>

#pragma once

namespace modm
{

namespace platform
{

MODM_ISR_DECL(EIC);

/// @ingroup		modm_platform_extint
enum class InputTrigger
{
	RisingEdge = EIC_CONFIG_SENSE0_RISE_Val,
	FallingEdge = EIC_CONFIG_SENSE0_FALL_Val,
	BothEdges = EIC_CONFIG_SENSE0_BOTH_Val,
	High = EIC_CONFIG_SENSE0_HIGH_Val,
	Low = EIC_CONFIG_SENSE0_LOW_Val,
};

/**
 * External Interrupt handler for SAMD devices.
 *
 * @author		Erik Henriksson
 * @ingroup		modm_platform_extint
 */
class ExternalInterrupt
{
	friend void EIC_IRQHandler(void);

public:
	/**
	 * Initializes the External Interrupt handler.
	 *
	 * @param clockGen
	 * 	The clock generator to use for the peripheral. If any interrupts are to
	 *  be used to akeup the CPU from standby mode, make sure this clock is
	 *  actually running in standby.
	 */
	static void
	initialize(ClockGenerator clockGen,
			   int priority = (1ul << __NVIC_PRIO_BITS) - 1ul);

protected:
	static std::array<std::function<void()>, 16> handlers_;
};

/**
 * External Interrupt instance for SAMD devices.
 *
 * @author		Erik Henriksson
 * @ingroup		modm_platform_extint
 */
template<int instance>
class ExtInt : private ExternalInterrupt
{
public:
	/**
	 * Initializes the External Interrupt instance.
	 *
	 * @param handler
	 * 	Function that will be called for any interrupts.
	 * @param trigger
	 * 	Specifies the edge detection trigger to use (default is rising edge).
	 * @param wakeupEnabled
	 * 	If true (default), allows the CPU to wakeup from interrupt
	 * 	from this instance.
	 */
	static void
	initialize(std::function<void()> handler, InputTrigger trigger = InputTrigger::RisingEdge,
			   bool wakeupEnabled = true);

	/**
	 * Connects a GPIO pin to this External Interrupt instance.
	 *
	 * @tparam Pin
	 * 	The GPIO pin to connect this instance to.
	 */
	template<class GpioPin>
	static void
	connect()
	{
		using Eic = Peripherals::Eic;
		using Pin = typename GpioPin::template As<PeripheralPin::ExtInt>;
		Pin::template Connector<Eic, Eic::Extint<instance>>::connect();
	}
};

}  // namespace platform

}  // namespace modm

#include "extint_impl.hpp"
