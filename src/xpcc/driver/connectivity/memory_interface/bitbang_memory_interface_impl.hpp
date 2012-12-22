

#ifndef XPCC__BITBANG_MEMORY_INTERFACE_HPP
	#error "Don't include this file directly, use 'bitbang_memory_interface.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template <typename PORT, typename CS, typename CD, typename WR>
PORT xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::port;

template <typename PORT, typename CS, typename CD, typename WR>
CS xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::cs;

template <typename PORT, typename CS, typename CD, typename WR>
CD xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::cd;

template <typename PORT, typename CS, typename CD, typename WR>
WR xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::wr;

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::initialize()
{

}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeCommand(uint8_t command, uint16_t data)
{

}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeData(uint8_t data)
{

}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeRam(uint8_t * addr, uint16_t size)
{

}

