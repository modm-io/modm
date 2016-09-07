// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BITBANG_MEMORY_INTERFACE_HPP
#define XPCC__BITBANG_MEMORY_INTERFACE_HPP

namespace xpcc
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
		xpcc_always_inline static void
		initialize();

		xpcc_always_inline static void
		writeRegister(const uint8_t reg);

		xpcc_always_inline static void
		writeCommand(const uint8_t command, const uint16_t data);

		static void
		writeDataMult(const uint16_t data, const uint16_t count);

		static void
		writeRam(uint8_t * addr, const uint16_t size);

	protected:
		xpcc_always_inline static void
		writeData(const uint16_t data);

	protected:
		static PORT port;
		static CS cs;
		static CD cd;
		static WR wr;
	};

}

#include "bitbang_memory_interface_impl.hpp"

#endif // XPCC__BITBANG_MEMORY_INTERFACE_HPP
