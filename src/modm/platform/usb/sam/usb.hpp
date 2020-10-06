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

	template<class... Pins>
	static void
	connect()
	{
		using DpPin = GetPin_t<PeripheralPin::Dp, Pins...>;
		using DmPin = GetPin_t<PeripheralPin::Dm, Pins...>;
		static_assert(!std::is_same_v<typename DpPin::Base, typename DmPin::Base>,
					  "Dp and Dm cannot use the same pin!");
		using Usb = Peripherals::Usb;
		using DpConnector = typename DpPin::template Connector<Usb, Usb::Dp>;
		using DmConnector = typename DmPin::template Connector<Usb, Usb::Dm>;
		DpConnector::connect();
		DmConnector::connect();
	}
};

} // namespace modm::platform
