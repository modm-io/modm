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
#   error "Don't include this file directly, use 'ixm42xxx.hpp' instead!"
#endif

#include "ixm42xxx.hpp"

namespace modm
{

template < class Transport >
Ixm42xxx< Transport >::Ixm42xxx(Data &data, uint8_t address) : Transport(address), data(data)
{
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<void>
Ixm42xxx< Transport >::initialize()
{
    RF_BEGIN();

    /// Synchronize the register bank selection
    RF_CALL(readRegister(Register::REG_BANK_SEL, &prevBank));

    /// Reset the device and wait 1 ms for the reset to be effective
    RF_CALL(writeRegister(Register::DEVICE_CONFIG, uint8_t(DeviceConfig::SOFT_RESET_CONFIG)));
    modm::delay_ms(1);

    /// Configure the device to use the endianess of the microcontroller
    if (isBigEndian())
    {
        RF_CALL(writeRegister(Register::INTF_CONFIG0, 0x30));
    }
    else
    {
        RF_CALL(writeRegister(Register::INTF_CONFIG0, 0x0));
    }

    /// Configure the device to use default full scale and odr for gyro and accel
    RF_CALL(updateRegister(Register::GYRO_CONFIG0, GyroFs_t(GyroFs::dps1000) | GyroOdr_t(GyroOdr::kHz1)));
    RF_CALL(updateRegister(Register::ACCEL_CONFIG0, AccelFs_t(AccelFs::g16) | AccelOdr_t(AccelOdr::kHz1)));

    RF_END();
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
Ixm42xxx< Transport >::readTempData()
{
    return readRegister(Register::TEMP_DATA1, data.sensorData.temp, 2);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
Ixm42xxx< Transport >::readAccelData()
{
    return readRegister(Register::ACCEL_DATA_X1, data.sensorData.accel, 6);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
Ixm42xxx< Transport >::readGyroData()
{
    return readRegister(Register::GYRO_DATA_X1, data.sensorData.gyro, 6);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
Ixm42xxx< Transport >::readSensorData()
{
    return readRegister(Register::TEMP_DATA1, reinterpret_cast<uint8_t*>(&data.sensorData), 14);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
modm::Ixm42xxx< Transport >::updateRegister(Register reg, Register_t setMask, Register_t clearMask)
{
    RF_BEGIN();

    if (RF_CALL(setRegisterBank(reg)) && RF_CALL(this->read(i(reg), readByte)))
    {
        readByte = (readByte & ~clearMask.value) | setMask.value;
        if (reg == Register::GYRO_CONFIG0)
        {
            GyroConfig0_t gyroConfig0 = GyroConfig0_t(readByte);
            data.gyroScale = 2000.0f / float(1 << uint8_t(GyroFs_t::get(gyroConfig0)));
        }
        else if (reg == Register::ACCEL_CONFIG0)
        {
            AccelConfig0_t accelConfig0 = AccelConfig0_t(readByte);
            data.accelScale = 16.0f / float(1 << uint8_t(AccelFs_t::get(accelConfig0)));
        }
        RF_RETURN_CALL( this->write(i(reg), readByte) );
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
modm::Ixm42xxx< Transport >::writeRegister(Register reg, uint8_t value)
{
    RF_BEGIN();

    if (RF_CALL(setRegisterBank(reg)))
    {
        RF_RETURN_CALL( this->write(i(reg), value) );
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
modm::Ixm42xxx< Transport >::readRegister(Register reg, uint8_t *value)
{
    RF_BEGIN();

    if (RF_CALL(setRegisterBank(reg)))
    {
        RF_RETURN_CALL( this->read(i(reg), value, 1) );
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
modm::Ixm42xxx< Transport >::readRegister(Register reg, uint8_t *buffer, std::size_t length)
{
    RF_BEGIN();

    if (RF_CALL(setRegisterBank(reg)))
    {
        RF_RETURN_CALL( this->read(i(reg), buffer, length) );
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport >
modm::ResumableResult<bool>
modm::Ixm42xxx< Transport >::setRegisterBank(Register regi)
{
    const uint8_t bank = (uint16_t(regi) >> 8) & 0xFF;

    RF_BEGIN();

    if (bank != prevBank)
    {
        RF_CALL( this->write(i(Register::REG_BANK_SEL), bank) );
        RF_CALL( this->read(i(Register::REG_BANK_SEL), prevBank) );
        RF_RETURN(bank == prevBank);
    }

    RF_END_RETURN(true);
}

} // namespace modm