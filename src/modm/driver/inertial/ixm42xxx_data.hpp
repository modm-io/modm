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

#ifndef MODM_IXM42XXX_DATA_HPP
#define MODM_IXM42XXX_DATA_HPP

#include <span>

#include <modm/math.hpp>

namespace modm
{

template < class Transport >
class Ixm42xxx;

namespace ixm42xxxdata
{

/// @ingroup modm_driver_ixm42xxx
struct
Data
{
    template < class Transport >
    friend class ::modm::Ixm42xxx;

    Data(std::span<uint8_t> fifoBuffer = std::span<uint8_t>()) :
        fifoCount(0),
        fifoBuffer(fifoBuffer),
        accelScale(16.f),
        gyroScale(2000.f)
    {
    }

    // DATA ACCESS
    ///@{
    int16_t inline
    getTemp() const
    { return sensorData.temp; }

    void inline
    getTemp(int16_t *temp) const
    { *temp = getTemp(); }

    void inline
    getTemp(float *temp) const
    { float t = getTemp(); *temp = t / 132.48f + 25.f; }

    Vector3i inline
    getAccel() const
    { return Vector3i(sensorData.accel); }

    void inline
    getAccel(Vector3i *accel) const
    { Vector3li a = getAccel(); *accel = a; }

    void inline
    getAccel(Vector3f *accel) const
    { Vector3f a = getAccel(); a *= accelScale / INT16_MAX; *accel = a; }

    Vector3i inline
    getGyro() const
    { return Vector3i(sensorData.gyro); }

    void inline
    getGyro(Vector3i *gyro) const
    { *gyro = getGyro(); }

    void inline
    getGyro(Vector3f *gyro) const
    { Vector3f g = getGyro(); g *= gyroScale / INT16_MAX; *gyro = g; }
    ///@}

    constexpr float
    getAccelScale() const
    { return accelScale; }

    constexpr float
    getGyroScale() const
    { return gyroScale; }

    constexpr uint16_t
    getFifoCount() const
    { return fifoCount; }

    constexpr uint16_t
    getFifoBufferSize() const
    { return fifoBuffer.size(); }

    constexpr std::span<const uint8_t>
    getFifoData() const
    { return fifoBuffer.subspan(0, fifoCount); }

private:
    struct
    SensorData {
        int16_t temp;
        int16_t accel[3];
        int16_t gyro[3];
    } sensorData;

    uint16_t fifoCount;
    std::span<uint8_t> fifoBuffer;

    float accelScale;
    float gyroScale;
};

/// @ingroup modm_driver_ixm42xxx
struct
FifoPacket
{
    FifoPacket() : header(0), accel(0), gyro(0), temp(0), timestamp(0), extension(0) {}

    // DATA ACCESS
    ///@{
    int16_t
    getTemp() const
    { return temp; }

    void
    getTemp(int16_t *temp) const
    { *temp = getTemp(); }

    void
    getTemp(float *temp) const
    { float t = getTemp(); *temp = (header & HEADER_20) ? t / 132.48f + 25.f : t / 2.07f + 25.f; }

    Vector3li
    getAccel() const;

    void
    getAccel(Vector3li *accel) const
    { Vector3li a = getAccel(); *accel = a; }

    void
    getAccel(Vector3f *accel, float scale = 16.f) const
    { Vector3f a = getAccel(); *accel = (header & HEADER_20) ? a / 8192.f : a * scale / INT16_MAX; }

    Vector3li
    getGyro() const;

    void
    getGyro(Vector3li *gyro) const
    { Vector3li g = getGyro(); *gyro = g; }

    void
    getGyro(Vector3f *gyro, float scale = 2000.f) const
    { Vector3f g = getGyro(); *gyro = (header & HEADER_20) ? g / 131.f : g * scale / INT16_MAX; }

    uint16_t
    getTimestamp() const
    { return timestamp; }
    ///@}

    constexpr bool
    containsSensorData() const
    { return (header & HEADER_MSG) == 0; }

    constexpr bool
    containsAccelData() const
    { return header & HEADER_ACCEL; }

    constexpr bool
    containsGyroData() const
    { return header & HEADER_GYRO; }

    constexpr bool
    containsOdrTimestamp() const
    { return header & HEADER_TIMESTAMP_ODR; }

    constexpr bool
    containsFsyncTimestamp() const
    { return header & HEADER_TIMESTAMP_FSYNC; }

    constexpr bool
    isExtended() const
    { return header & HEADER_20; }

public:

    static uint16_t
    parse(std::span<const uint8_t> fifoData, FifoPacket &fifoPacket, uint16_t fifoIndex = 0);

private:

    int header;
    int16_t accel[3];
    int16_t gyro[3];
    int16_t temp;
    uint16_t timestamp;
    int8_t extension[3];

    enum
    {
        HEADER_MSG = Bit7,
        HEADER_ACCEL = Bit6,
        HEADER_GYRO = Bit5,
        HEADER_20 = Bit4,
        HEADER_TIMESTAMP_ODR = Bit3,
        HEADER_TIMESTAMP_FSYNC = Bit2,
        HEADER_ODR_ACCEL = Bit1,
        HEADER_ODR_GYRO = Bit0,
    };

public:
    constexpr bool operator==(const FifoPacket& rhs) const;
};

/// @ingroup modm_driver_ixm42xxx
template <size_t FifoBufferSize>
struct
FifoData : public Data
{
    FifoData() : Data(std::span<uint8_t>{fifoBuffer}) {}

    struct iterator
    {
    public:
        typedef std::input_iterator_tag iterator_category;
        typedef FifoPacket value_type;
        typedef FifoPacket& reference;
        typedef FifoPacket* pointer;

        constexpr iterator(std::span<const uint8_t> data, uint16_t index) : fifoData(data), fifoIndex(index)
        {
            fifoIndex = FifoPacket::parse(fifoData, fifoPacket, fifoIndex);
        }

        constexpr iterator operator++() { fifoIndex = FifoPacket::parse(fifoData, fifoPacket, fifoIndex); return *this; }
        constexpr reference operator*() { return fifoPacket; }
        constexpr pointer operator->() { return &fifoPacket; }
        constexpr bool operator==(const iterator& rhs) { return fifoIndex == rhs.fifoIndex && fifoPacket == rhs.fifoPacket; }

    private:
        std::span<const uint8_t> fifoData;
        uint16_t fifoIndex;

        FifoPacket fifoPacket;
    };

    constexpr iterator begin() const { return iterator(getFifoData(), 0); }
    constexpr iterator end()   const { return iterator(getFifoData(), getFifoCount()); }

private:
    uint8_t fifoBuffer[FifoBufferSize];
};

} // ixm42xxxdata namespace

} // namespace modm

#include "ixm42xxx_data_impl.hpp"

#endif // MODM_IXM42XXX_DATA_HPP