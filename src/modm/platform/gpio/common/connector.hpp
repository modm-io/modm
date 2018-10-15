/*
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PLATFORM_GPIO_CONNECTOR_HPP
#define MODM_PLATFORM_GPIO_CONNECTOR_HPP

#include "base.hpp"
#include "connector_detail.hpp"

namespace modm
{

namespace platform
{

/// @cond
template< Peripheral peripheral, template<Peripheral _> class... Signals >
struct GpioConnector
{
	template< class GpioQuery >
	static constexpr bool Contains = detail::GpioContains<peripheral, GpioQuery, Signals...>::value;
	template< class GpioQuery >
	static constexpr bool IsValid = not std::is_same_v<typename GpioQuery::Type, GpioUnused>;
	template< Gpio::Signal signal >
	using GetSignal = typename detail::GpioGetSignal<peripheral, signal, Signals...>::Gpio;

	inline static void
	connect()
	{
		detail::GpioSignalConnect<peripheral, Signals...>::connect();
	}
	inline static void
	disconnect()
	{
		detail::GpioSignalConnect<peripheral, Signals...>::disconnect();
	}
};
/// @endcond

} // namespace platform

} // namespace modm

#endif // MODM_PLATFORM_GPIO_CONNECTOR_HPP
