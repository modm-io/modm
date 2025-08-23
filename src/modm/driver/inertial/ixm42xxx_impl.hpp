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
void
Ixm42xxx< Transport >::initialize()
{
    /// Synchronize the register bank selection
    readRegister(Register::REG_BANK_SEL, &prevBank);

    /// Reset the device and wait 1 ms for the reset to be effective
    writeRegister(Register::DEVICE_CONFIG, uint8_t(DeviceConfig::SOFT_RESET_CONFIG));
    modm::this_fiber::sleep_for(1ms);

    /// Configure the device to use the endianess of the microcontroller
    if (isBigEndian())
    {
        writeRegister(Register::INTF_CONFIG0, 0x30);
    }
    else
    {
        writeRegister(Register::INTF_CONFIG0, 0x0);
    }

    /// Configure the device to use default full scale and odr for gyro and accel
    updateRegister(Register::GYRO_CONFIG0, GyroFs_t(GyroFs::dps1000) | GyroOdr_t(GyroOdr::kHz1));
    updateRegister(Register::ACCEL_CONFIG0, AccelFs_t(AccelFs::g16) | AccelOdr_t(AccelOdr::kHz1));
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readTempData()
{
    return readRegister(Register::TEMP_DATA1, data.sensorData.temp, 2);
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readAccelData()
{
    return readRegister(Register::ACCEL_DATA_X1, data.sensorData.accel, 6);
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readGyroData()
{
    return readRegister(Register::GYRO_DATA_X1, data.sensorData.gyro, 6);
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readSensorData()
{
    return readRegister(Register::TEMP_DATA1, reinterpret_cast<uint8_t*>(&data.sensorData), 14);
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readFsyncTimestamp(uint16_t *timestamp)
{
    /// TODO: Verify endianess
    return readRegister(Register::TMST_FSYNCH, reinterpret_cast<uint8_t*>(timestamp), 2);
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readFifoCount(uint16_t *count)
{
    /// TODO: Figure out why the endianess seem to be wrong (possibly like FIFO_CONFIG2-3)
    /// Read FIFO_COUNTL to latch new data for both FIFO_COUNTH and FIFO_COUNTL
    if (readRegister(Register::FIFO_COUNTL, &readByte))
    {
        *count = uint16_t(readByte) << 8;
        if (readRegister(Register::FIFO_COUNTH, &readByte))
        {
            *count |= uint16_t(readByte);
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::readFifoData()
{
    if (readFifoCount(&data.fifoCount))
    {
        data.fifoCount = std::min<uint16_t>(data.fifoCount, data.fifoBuffer.size());
        if (data.fifoCount > 0)
        {
            return readRegister(Register::FIFO_DATA, data.fifoBuffer.data(), data.fifoCount);
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
Ixm42xxx< Transport >::writeFifoWatermark(uint16_t watermark)
{
    /// TODO: Determine what endianess to use
    if (writeRegister(Register::FIFO_CONFIG2, watermark & 0xFF) &&
        writeRegister(Register::FIFO_CONFIG3, (watermark >> 8) & 0xFF))
    {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
modm::Ixm42xxx< Transport >::updateRegister(Register reg, Register_t setMask, Register_t clearMask)
{
    if (setRegisterBank(reg) && this->read(i(reg), readByte))
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
        return this->write(i(reg), readByte);
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
modm::Ixm42xxx< Transport >::writeRegister(Register reg, uint8_t value)
{
    if (setRegisterBank(reg))
    {
        return this->write(i(reg), value);
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
modm::Ixm42xxx< Transport >::readRegister(Register reg, uint8_t *value)
{
    if (setRegisterBank(reg))
    {
        return this->read(i(reg), value, 1);
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
modm::Ixm42xxx< Transport >::readRegister(Register reg, uint8_t *buffer, std::size_t length)
{
    if (setRegisterBank(reg))
    {
        return this->read(i(reg), buffer, length);
    }

    return false;
}

// -----------------------------------------------------------------------------

template < class Transport >
bool
modm::Ixm42xxx< Transport >::setRegisterBank(Register regi)
{
    const uint8_t bank = (uint16_t(regi) >> 8) & 0xFF;

    if (bank != prevBank)
    {
        this->write(i(Register::REG_BANK_SEL), bank);
        this->read(i(Register::REG_BANK_SEL), prevBank);
        return bank == prevBank;
    }

    return true;
}

} // namespace modm
