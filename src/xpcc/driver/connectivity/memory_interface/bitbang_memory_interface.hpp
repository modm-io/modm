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
	 * \author	Strongly typed
	 */
	template < typename PORT,
				typename CS,
				typename CD,
				typename WR >
	class BitbangMemoryInterface
	{
	public:
		static void
		initialize();

		static void
		writeCommand(uint8_t command, uint16_t data);

		static void
		writeData(uint8_t data);

		static void
		writeRam(uint8_t * addr, uint16_t size);

	protected:
		static PORT port;
		static CS cs;
		static CD cd;
		static WR wr;
	};

}

#include "bitbang_memory_interface_impl.hpp"

#endif // XPCC__BITBANG_MEMORY_INTERFACE_HPP
