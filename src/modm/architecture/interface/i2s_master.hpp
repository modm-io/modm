/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2017, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2021, Marton Lednczki
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2S_MASTER_HPP
#define MODM_INTERFACE_I2S_MASTER_HPP

#include <modm/processing/resumable.hpp>
#include "i2s.hpp"

namespace modm
{

/**
 * Interface for a I2S Master
 *
 * @author	Marton Ledneczki
 * @ingroup	modm_architecture_i2s
 */
class I2sMaster : public ::modm::PeripheralDriver, public I2s
{
#ifdef __DOXYGEN__
public:
	/**
	 * Connect GPIOs to the peripheral and configure them.
	 *
	 * This configures the CK (serial clock), MCK (master clock),
	 * SD (serial data) and WS (word select) signals and connects them.
	 *
	 * @tparam	Signals
	 *		One CK, SD, WS, MCK signal are required 
	 *      and can be passed out-of-order.
	 */
	template< class... Signals >
	static void
	connect();

	/**
	 * Initializes the hardware and sets the samplerate.
	 *
	 * @tparam	SystemClock
	 * 		the currently active system clock
	 * @tparam	samplerate
	 * 		the desired sample rate in Hz
	 * @tparam	tolerance
	 * 		the allowed relative tolerance for the resulting samplerate
	 */
	template< class SystemClock, frequency_t samplerate, percent_t tolerance=pct(0.019) >
	static void
	initialize();

#endif
};

} // namespace modm

#endif // MODM_INTERFACE_SPI_MASTER_HPP
