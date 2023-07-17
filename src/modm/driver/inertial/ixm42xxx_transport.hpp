// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IXM42XXX_TRANSPORT_HPP
#define MODM_IXM42XXX_TRANSPORT_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/**
 * IXM42xxx I2C Transport Layer.
 *
 * @see Ixm42xxx
 *
 * @tparam I2cMaster I2C interface
 *
 * @ingroup modm_driver_ixm42xxx_transport
 * @author	Rasmus Kleist Hørlyck Sørensen
 */
template < class I2cMaster >
class Ixm42xxxTransportI2c : public modm::I2cDevice< I2cMaster, 4 >
{
public:
    Ixm42xxxTransportI2c(uint8_t address);

protected:
    // RAW REGISTER ACCESS
    /// write a 8bit value
    modm::ResumableResult<bool>
    write(uint8_t reg, uint8_t value);

    /// read a 8bit value
    modm::ResumableResult<bool>
    read(uint8_t reg, uint8_t &value);

    /// read multiple 8bit values from a start register
    modm::ResumableResult<bool>
    read(uint8_t reg, uint8_t *buffer, std::size_t length);

private:
    uint8_t buffer[2];
};

/**
 * IXM42xxx SPI Transport Layer.
 *
 * @see Ixm42xxx
 *
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @ingroup modm_driver_ixm42xxx_transport
 * @author	Rasmus Kleist Hørlyck Sørensen
 */
template < class SpiMaster, class Cs >
class Ixm42xxxTransportSpi : public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable< 4 >
{
public:
    Ixm42xxxTransportSpi(uint8_t /*address*/);

    /// pings the sensor
    modm::ResumableResult<bool>
    ping();

protected:
    // RAW REGISTER ACCESS
    /// write a 8bit value
    modm::ResumableResult<bool>
    write(uint8_t reg, uint8_t value);

    /// read a 8bit value
    modm::ResumableResult<bool>
    read(uint8_t reg, uint8_t &value);

    /// read multiple 8bit values from a start register
    modm::ResumableResult<bool>
    read(uint8_t reg, uint8_t *buffer, std::size_t length);

private:
    uint8_t whoAmI;

    // write read bit on the address
    static constexpr uint8_t Read = Bit7;
    static constexpr uint8_t Write = 0x00;
};

} // namespace modm

#include "ixm42xxx_transport_impl.hpp"

#endif	// MODM_IXM42XXX_TRANSPORT_HPP
