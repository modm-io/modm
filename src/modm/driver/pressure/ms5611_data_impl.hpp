// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MS5611_DATA_HPP
#	error  "Don't include this file directly, use 'ms5611_data.hpp' instead!"
#endif

namespace modm
{

namespace ms5611data
{

int32_t
Data::getPressure()
{
    if (not (meta & PRESSURE_CALCULATED))
    {
        calculateCalibratedValues();
    }
    return calibratedPressure;
}

int32_t
Data::getTemperature()
{
    if (not (meta & TEMPERATURE_CALCULATED))
    {
        calculateCalibratedValues();
    }
    return calibratedTemperature;
}

void
Data::calculateCalibratedValues()
{
    // Read uncompensated pressure and temperature from raw buffer
    uint32_t rawPres = uint32_t(raw[0] << 16 | raw[1] << 8 | raw[2]);
    uint32_t rawTemp = uint32_t(raw[3] << 16 | raw[4] << 8 | raw[5]);

    // Compute intermediate values used to calculate the pressure and temperature
    int32_t dT = rawTemp - (uint32_t(prom.data[5]) << 8);
    int64_t off = (int64_t(prom.data[2]) << 16) + ((int64_t(prom.data[4]) * int64_t(dT)) >> 7);
    int64_t sens = (int64_t(prom.data[1]) << 15) + ((int64_t(prom.data[3]) * int64_t(dT)) >> 8);

    // Compute temperature without second order compensation
    int32_t temp = 2000 + ((int64_t(dT) * prom.data[6]) >> 23);

    // Second order temperature compensation
    if (temp < 2000)
    {
        // Temperature is less then 20.00 degrees Centigrade
        int64_t temp2 = (int64_t(dT) * int64_t(dT)) >> 31;

        int64_t off2 = (5 * (temp - 2000) * (temp - 2000)) >> 1;
        int64_t sens2 = off2 >> 1;

        if (temp < -1500)
        {
            // Temperature is less then -15.00 degrees Centigrade
            off2 += 7 * (temp + 1500) * (temp + 1500);
            sens2 += (11 * (temp + 1500) * (temp + 1500)) >> 1;
        }

        temp -= temp2;
        off -= off2;
        sens -= sens2;
    }

    calibratedPressure = (((sens * int64_t(rawPres)) >> 21) - off) >> 15;
    meta |= PRESSURE_CALCULATED;

    calibratedTemperature = temp;
    meta |= TEMPERATURE_CALCULATED;
}

} // ms5611data

} // modm namespace