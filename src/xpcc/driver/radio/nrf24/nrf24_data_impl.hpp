// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_DATA_HPP
#	error "Don't include this file directly, use 'nrf24_data.hpp' instead!"
#endif

#include "nrf24_data.hpp"

using namespace xpcc::nrf24;

// --------------------------------------------------------------------------------------------------------------------


template<typename Nrf24Phy>
BaseAddress Nrf24Data<Nrf24Phy>::baseAddress;

template<typename Nrf24Phy>
Address Nrf24Data<Nrf24Phy>::broadcastAddress;

template<typename Nrf24Phy>
Address Nrf24Data<Nrf24Phy>::connections[4];

// --------------------------------------------------------------------------------------------------------------------

/* typedef config layer for simplicity */
template<typename Nrf24Phy>
typedef Nrf24Config<Nrf24Phy> ConfigLayer;

// --------------------------------------------------------------------------------------------------------------------


template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Data<Nrf24Phy>::initialize(BaseAddress base_address, Address broadcast_address)
{
	baseAddress = base_address;
	broadcastAddress = broadcast_address;

	// ... configure phy

	// ConfigLayer::setXyz();

	// ... setup broadcast pipe
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::nrf24::Nrf24Data<Nrf24Phy>::sendPacket(packet_t& packet)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::nrf24::Nrf24Data<Nrf24Phy>::getPacket(packet_t& packet)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::nrf24::Nrf24Data<Nrf24Phy>::isReadyToSend()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::nrf24::Nrf24Data<Nrf24Phy>::isPacketAvailable()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
SendingState
xpcc::nrf24::Nrf24Data<Nrf24Phy>::getSendingFeedback()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
Address
xpcc::nrf24::Nrf24Data<Nrf24Phy>::getAddress()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Data<Nrf24Phy>::setAddress(Address address)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::nrf24::Nrf24Data<Nrf24Phy>::establishConnection()
{
	// not yet implemented
	return false;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::nrf24::Nrf24Data<Nrf24Phy>::destroyConnection()
{
	// not yet implemented
	return false;
}
