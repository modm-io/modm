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

#ifndef MODM_MS5611_HPP
#define MODM_MS5611_HPP

#include "ms5611_data.hpp"

#include <modm/processing/resumable.hpp>
#include <modm/processing/timer/timeout.hpp>
#include <modm/architecture/interface/spi_device.hpp>

namespace modm
{

/// @ingroup modm_driver_ms5611
struct ms5611
{

using Prom = modm::ms5611data::Prom;
using Data = modm::ms5611data::Data;
using DataBase = modm::ms5611data::DataBase;

protected:
    /// @cond
    enum class
    Command : uint8_t
    {
        Reset       = 0x1E,
        Convert     = 0x40,
        AdcRead     = 0x00,
        PromRead    = 0xA0,
    };

    enum class
    Conversion : uint8_t
    {
        Pressure = 0x00,
        Temperature = 0x10,
    };
    /// @endcond

public:
    enum class
    OversamplingRatio : uint8_t
    {
        Osr256  = 0x00,
        Osr512  = 0x02,
        Osr1024 = 0x04,
        Osr2048 = 0x06,
        Osr4096 = 0x08,
    };

protected:
    /// @cond
    static constexpr uint8_t
    i(Command cmd) { return uint8_t(cmd); }
    static constexpr uint8_t
    i(Conversion conv) { return uint8_t(conv); }
    static constexpr uint8_t
    i(OversamplingRatio osr) { return uint8_t(osr); }
    /// @endcond
};

/**
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Rasmus Kleist Hørlyck Sørensen
 * @ingroup modm_driver_ms5611
 */
template <typename SpiMaster, typename Cs>
class Ms5611 : public ms5611, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<2>
{
public:
    /**
	 * @param	data pointer to buffer of the internal data of type Data
	 */
    Ms5611(DataBase &data);

    /// Call this function before using the device to read the factory calibration
    /// @warning calls to this function resets the device
    modm::ResumableResult<bool>
    initialize();

    /// Do a readout sequence to convert and read temperature and then pressure from sensor
    modm::ResumableResult<void>
    readout(OversamplingRatio osrPressure = OversamplingRatio::Osr256,
            OversamplingRatio osrTemperature = OversamplingRatio::Osr256);

public:
    /// Get the data object for this sensor
    inline DataBase&
    getData() { return data; }

private:
    /// Read the PROM register at the address
    modm::ResumableResult<uint16_t>
    readProm(uint8_t addr);

private:
    DataBase &data;
    modm::ShortTimeout timeout;

    /**
     * Conversion time of the Analog Digital Convert for different oversampling ratios
     * The conversion times are taken from the application note AN520
     */
    static constexpr uint8_t conversionDelay[] = {1, 3, 4, 6, 10};

    /// Command buffer for writing to and reading from the device
    uint8_t buffer[2];
};

} // modm namespace

#include "ms5611_impl.hpp"

#endif // MODM_MS5611_HPP