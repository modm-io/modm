/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PAT9125EL_TRANSPORT_HPP
#define MODM_PAT9125EL_TRANSPORT_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>
#include <array>

namespace modm
{

/**
 * PAT9125EL I2C transport layer
 *
 * @author Christopher Durand
 * @ingroup modm_driver_pat9125el
 */
template<class I2cMaster>
class Pat9125elI2cTransport : public I2cDevice<I2cMaster, 4>
{
protected:
	Pat9125elI2cTransport(uint8_t address);

	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t& value);

	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t* buffer, uint8_t length);

private:
	std::array<uint8_t, 2> buffer;
};

// TODO: implement SPI transport layer

}

#include "pat9125el_transport_impl.hpp"

#endif // MODM_PAT9125EL_TRANSPORT_HPP
