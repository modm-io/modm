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

#include <cstring>

#ifndef MODM_IXM42XXX_DATA_HPP
#	error  "Don't include this file directly, use 'ixm42xxx_data.hpp' instead!"
#endif

namespace modm
{

namespace ixm42xxxdata
{

Vector3li
FifoPacket::getAccel() const
{
    Vector3li a;
    if (header & HEADER_20)
    {
        a.x = accel[0] << 2 | ((extension[0] & 0xF0) >> 6);
        a.y = accel[1] << 2 | ((extension[1] & 0xF0) >> 6);
        a.z = accel[2] << 2 | ((extension[2] & 0xF0) >> 6);
    }
    else
    {
        a.x = accel[0];
        a.y = accel[1];
        a.z = accel[2];
    }
    return a;
}

Vector3li
FifoPacket::getGyro() const
{
    Vector3li g;
    if (header & HEADER_20)
    {
        g.x = ((gyro[0] << 3) | (extension[0] & 0x0F));
        g.y = ((gyro[1] << 3) | (extension[1] & 0x0F));
        g.z = ((gyro[2] << 3) | (extension[2] & 0x0F));
    }
    else
    {
        g.x = gyro[0];
        g.y = gyro[1];
        g.z = gyro[2];
    }
    return g;
}

uint16_t
FifoPacket::parse(std::span<const uint8_t> fifoData, FifoPacket &fifoPacket, uint16_t fifoIndex)
{
    if (fifoIndex < fifoData.size() && (fifoData[fifoIndex] & HEADER_MSG) == 0)
    {
        // Packet contains sensor data
        fifoPacket.header = fifoData[fifoIndex++];
        if (fifoPacket.header & HEADER_ACCEL)
        {
            // Packet is sized so that accel data have location in the packet
            std::memcpy(fifoPacket.accel, fifoData.data() + fifoIndex, 6);
            fifoIndex += 6;
        }

        if (fifoPacket.header & HEADER_GYRO)
        {
            // Packet is sized so that gyro data have location in the packet
            std::memcpy(fifoPacket.gyro, fifoData.data() + fifoIndex, 6);
            fifoIndex += 6;
        }

        if ((fifoPacket.header & HEADER_ACCEL) || (fifoPacket.header & HEADER_GYRO))
        {
            if (fifoPacket.header & HEADER_20)
            {
                std::memcpy(&fifoPacket.temp, fifoData.data() + fifoIndex, 2);
                fifoIndex += 2;
            }
            else
            {
                std::memcpy(&fifoPacket.temp, fifoData.data() + fifoIndex, 1);
                fifoIndex += 1;
            }
        }

        if ((fifoPacket.header & HEADER_TIMESTAMP_ODR) || (fifoPacket.header & HEADER_TIMESTAMP_FSYNC))
        {
            // Packet contains ODR or FSYNC Timestamp
            std::memcpy(&fifoPacket.timestamp, fifoData.data() + fifoIndex, 2);
            fifoIndex += 2;
        }

        if (fifoPacket.header & HEADER_20)
        {
            // Packet has a new and valid sample of extended 20-bit data for gyro and/or accel
            std::memcpy(fifoPacket.extension, fifoData.data() + fifoIndex, 3);
            fifoIndex += 3;
        }
    }
    else
    {
        // FIFO is empty
        fifoPacket = FifoPacket();
        fifoIndex = fifoData.size();
    }

    return fifoIndex;
}

constexpr bool
FifoPacket::operator==(const FifoPacket& rhs) const
{
    return ((this->header    == rhs.header)             &&
            (this->temp      == rhs.temp)               &&
            (this->timestamp == rhs.timestamp)          &&
            (std::equal(accel, accel + 3, rhs.accel))   &&
            (std::equal(gyro, gyro + 3, rhs.gyro))      &&
            (std::equal(extension, extension + 3, rhs.extension)));
}

} // ixm42xxxdata namespace

} // modm namespace