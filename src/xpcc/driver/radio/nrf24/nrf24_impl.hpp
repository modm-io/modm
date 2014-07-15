#ifndef XPCC__NRF24_HPP
#	error "Don't include this file directly, use 'nrf24.hpp' instead!"
#endif

#include "nrf24.hpp"

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24<Spi, Csn, Ce>::writeCommand(nrf24::Command command, uint8_t unsignedChar)
{
	Csn::reset();

	status = Spi::writeReadBlocking(command);
}
