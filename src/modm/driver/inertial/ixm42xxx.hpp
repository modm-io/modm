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

#ifndef MODM_IXM42XXX_HPP
#define MODM_IXM42XXX_HPP

#include "ixm42xxx_data.hpp"
#include "ixm42xxx_definitions.hpp"
#include "ixm42xxx_transport.hpp"

namespace modm
{

/**
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 * @see Ixm42xxxTransportI2c
 * @see Ixm42xxxTransportSpi
 *
 * @ingroup modm_driver_ixm42xxx
 * @author	Rasmus Kleist Hørlyck Sørensen
 */
template < class Transport >
class Ixm42xxx : public ixm42xxx, public Transport
{
private:
    using Data = modm::ixm42xxxdata::Data;

public:
    /**
     * @brief Constructor
     *
     * @param data A ixm42xxx::data object
     * @param address the I2C address of the device
     */
    Ixm42xxx(Data &data, uint8_t address = 0b11010000);

    /**
     * @brief Initialize the device to use default settings and the endianess of the microcontroller
     * @warning Calling this functions resets the device and blocks for 1 ms
     */
    modm::ResumableResult<void>
    initialize();

    /**
     * @brief Read the temperature data from the device
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     */
    modm::ResumableResult<bool>
    readTempData();

    /**
     * @brief Read the accel data from the device
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     */
    modm::ResumableResult<bool>
    readAccelData();

    /**
     * @brief Read the gyro data from the device
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     */
    modm::ResumableResult<bool>
    readGyroData();

    /**
     * @brief Read the sensor data from the device
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     */
    modm::ResumableResult<bool>
    readSensorData();

public:
    /**
     * @brief update a single register.
     *
     * @param reg The register to be updated
     * @param setMask The bits to be setted in the register
     * @param clearMask The bits to be cleared in the register.
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     *
     * @warning Only the registers GYRO_CONFIG0, ACCEL_CONFIG0 and PWR_MGMT0
     * can be modified during sensor operation
     */
    modm::ResumableResult<bool>
    updateRegister(Register reg, Register_t setMask, Register_t clearMask = Register_t(0xff));

    /**
     * @brief Write a single register.
     *
     * @param reg The register to be read
     * @param value The value to write to the register
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     *
     * @warning Only the registers GYRO_CONFIG0, ACCEL_CONFIG0 and PWR_MGMT0
     * can be modified during sensor operation
     */
    modm::ResumableResult<bool>
    writeRegister(Register reg, uint8_t value);

    /**
     * @brief Read a single register.
     *
     * @param reg The register to be read
     * @param value The placeholder for the read value to be stored
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     */
    modm::ResumableResult<bool>
    readRegister(Register reg, uint8_t *value);

    /**
     * @brief Read consecutive registers.
     *
     * @param reg The register to start reading from
     * @param data The placeholder for the read values to be stored
     * @return False in case of any error, e.g. if some register access is not
     * permitted.
     */
    modm::ResumableResult<bool>
    readRegister(Register reg, uint8_t *buffer, std::size_t length);

public:
    /// Get the data object for this sensor.
    inline Data&
    getData()
    { return data; }

protected:
    inline modm::ResumableResult<bool>
    setRegisterBank(Register regi);

private:
    Data &data;
    uint8_t readByte;
    uint8_t prevBank;
};

} // namespace modm

#include "ixm42xxx_impl.hpp"

#endif // MODM_IXM42XXX_HPP