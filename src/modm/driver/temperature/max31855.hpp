// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2022, Sarah Vilete
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MAX31855_HPP
#define MODM_MAX31855_HPP

#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/spi_device.hpp>

namespace modm
{

/// @ingroup modm_driver_max31855
struct max31855
{
    /// Thermocouple fault bits
    enum class
    Fault : uint8_t
    {
        ShortCircuitVcc = Bit2,
        ShortCircuitGnd = Bit1,
        OpenCircuit     = Bit0
    };

    struct modm_packed
    Data
    {
        /// @return value associated with the respective fault
        constexpr Fault getFault() const
        {
            return static_cast<Fault>(data[3] & 0b111);
        }

        /// @return the thermocouple temperature scaled according to the device documentation
        constexpr float
        getThermocoupleTemperature() const
        {
            // convert raw 14 bit readout in 2's complement to a 16 bit signed
            const int16_t rawTemp = static_cast<int16_t>((data[0] << 8) | (data[1] & 0b11111100)) / 4;
            return 0.25f * rawTemp;
        }

        /// @return the reference junction temperature scaled according to the device documentation
        constexpr float
        getReferenceJunctionTemperature() const
        {
            // convert raw 12 bit readout in 2's complement to a 16 bit signed
            const int16_t rawTemp = static_cast<int16_t>((data[2] << 8) | (data[3] & 0b11110000)) / 16;
            return 0.0625f * rawTemp;
        }

        uint8_t data[4];
    };
}; // struct max31855

/**
 * @tparam SpiMaster
 * @tparam Cs
 *
 * @author  Sarah Vilete
 * @ingroup modm_driver_max31855
 */
template <typename SpiMaster, typename Cs>
class Max31855 : public max31855, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<1>
{
public:
    /**
     * @param data pointer to buffer of the internal data of type Data
     */
    Max31855(Data &data);

    /// Call this function once before using the device
    void
    initialize();

    /// Read the raw data from the sensor
    modm::ResumableResult<void>
    readout();

public:
    /// Get the data object for this sensor
    inline Data&
    getData()
    { return data; }

private:
    Data &data;
};

} // namespace modm

#include "max31855_impl.hpp"

#endif // MODM_MAX31855_HPP