/*
 * Copyright (c) 2022, Andrey Kunitsyn
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

/// @ingroup modm_platform_usb
class Usb
{
public:
	template<class SystemClock>
	static void
	initialize(uint8_t priority = 3)
	{
		static_assert(SystemClock::UsbFrequency == 48_MHz, "USB must have a 48MHz clock!");
		NVIC_SetPriority(USBCTRL_IRQ_IRQn, priority);
	}

	template<class... Signals>
	static void
	connect()
	{
		using Connector = GpioConnector<Peripheral::Usb, Signals...>;
		// Dp, Dm use dedicated pins, but Vbus and Overcurrent can be connected
		Connector::connect();
	}
};

}  // namespace modm::platform
