/*
* Copyright (c) 2020, Erik Henriksson
* Copyright (c) 2020, Niklas Hauser
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#pragma once

#include <modm/platform/device.hpp>
#include <modm/platform/gpio/connector.hpp>

namespace modm::platform
{

class Usb
{
public:
	template< class SystemClock >
	static void
	initialize(uint8_t priority=3)
	{
		static_assert(SystemClock::Frequency == 48_MHz, "Usb must have a 48MHz clock!");
		PM->APBBMASK.reg |= PM_APBBMASK_USB;
		PM->AHBMASK.reg |= PM_AHBMASK_USB;
		GenericClockController::connect<ClockPeripheral::Usb>(ClockGenerator::System);
		NVIC_SetPriority(USB_IRQn, priority);
	}

	template< template<Peripheral _> class... Signals >
	static void
	connect()
	{
		using Connector = GpioConnector<Peripheral::Usb, Signals...>;
		using Dp = typename Connector::template GetSignal< Gpio::Signal::Dp >;
		using Dm = typename Connector::template GetSignal< Gpio::Signal::Dm >;
		static_assert(((Connector::template IsValid<Dp> and Connector::template IsValid<Dm>) and sizeof...(Signals) == 2),
					  "Usb::connect() requires one Dp and one Dm signal!");

		Connector::connect();
	}
};

} // namespace modm::platform
