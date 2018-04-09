/*
 * Copyright (c) 2012-2013, Sascha Schade
 * Copyright (c) 2014, 2016, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BITBANG_MEMORY_INTERFACE_HPP
#define MODM_BITBANG_MEMORY_INTERFACE_HPP

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/delay.hpp>

namespace modm
{
	/**
	 * \brief	Access external parallel memory with bit-bang mode.
	 *
	 * Usable for displays with 8080-like bus interface.
	 * Very specific
	 *
	 * \tparam	PORT	Parallel port of data pins
	 * \tparam	CS		Chip Select GPIO pin
	 * \tparam	CD		Command / Data GPIO pin
	 * \tparam	WR		Write strobe GPIO pin
	 *
	 * \author	strongly typed
	 * \ingroup	driver_bus
	 */
	template < typename PORT,
				typename CS,
				typename CD,
				typename WR >
	class BitbangMemoryInterface
	{
	public:
		modm_always_inline static void
		initialize();

		modm_always_inline static void
		writeRegister(const uint8_t reg);

		modm_always_inline static void
		writeCommand(const uint8_t command, const uint16_t data);

		static void
		writeDataMult(const uint16_t data, const uint16_t count);

		static void
		writeRam(uint8_t * addr, const uint16_t size);

	protected:
		modm_always_inline static void
		writeData(const uint16_t data);

	protected:
		static PORT port;
		static CS cs;
		static CD cd;
		static WR wr;
	};

}

#include "bitbang_memory_interface_impl.hpp"

#endif // MODM_BITBANG_MEMORY_INTERFACE_HPP
