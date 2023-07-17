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

    Data() :
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

private:
    struct
    SensorData {
        int16_t temp;
        int16_t accel[3];
        int16_t gyro[3];
    } sensorData;

    float accelScale;
    float gyroScale;
};

} // ixm42xxxdata namespace

} // namespace modm

#endif // MODM_IXM42XXX_DATA_HPP