/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_SPI_DEVICE_HPP
#define MODM_INTERFACE_SPI_DEVICE_HPP

#include "spi.hpp"
#include "spi_master.hpp"

namespace modm
{

/**
 * Base class of an SPI Device.
 *
 * This class provides functions for configuring the peripheral.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup modm_architecture_spi_device
 */
template < class SpiMaster >
class SpiDevice
{
	Spi::ConfigurationHandler configuration;

public:
	SpiDevice()
	:	configuration(nullptr)
	{
	}

	void inline
	attachConfigurationHandler(Spi::ConfigurationHandler handler)
	{
		configuration = handler;
	}

protected:
	bool inline
	acquireMaster()
	{
		return (SpiMaster::acquire(this, configuration) != 0);
	}

	bool inline
	releaseMaster()
	{
		return (SpiMaster::release(this) == 0);
	}
};

}	// namespace modm

#endif // MODM_INTERFACE_SPI_DEVICE_HPP
