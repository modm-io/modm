// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_CONFIG_HPP
#	error "Don't include this file directly, use 'nrf24_config.hpp' instead!"
#endif

#include "nrf24_config.hpp"

using namespace xpcc::nrf24;

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::powerUp()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::powerDown()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::setChannel(uint8_t channel)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::setMode(Mode mode)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::setSpeed(Speed speed)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void xpcc::nrf24::Nrf24Config<Nrf24Phy>::setCrc(Crc crc)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::setAddressWidth(AddressWidth width)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::setRfPower(RfPower power)
{
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::nrf24::Nrf24Config<Nrf24Phy>::setAutoRetransmitDelay(AutoRetransmitDelay delay)
{
}
