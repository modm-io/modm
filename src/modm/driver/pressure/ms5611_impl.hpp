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
#   error "Don't include this file directly, use 'ms5611.hpp' instead!"
#endif

#include <modm/math/utils/endianness.hpp>

namespace modm
{

template <typename SpiMaster, typename Cs>
Ms5611<SpiMaster,Cs>::Ms5611(DataBase &data) : data(data)
{
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
bool
Ms5611<SpiMaster,Cs>::initialize()
{
    // Command sequence must be clocked out with MSB first
    this->attachConfigurationHandler([]
    {
        SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
        SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
    });

    Cs::setOutput(modm::Gpio::High);

    // Reset sensor once after power-on to load calibration PROM into the internal registers
    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    buffer[0] = i(Command::Reset);
    SpiMaster::transfer(buffer, nullptr, 1);

    if(this->releaseMaster())
    {
        Cs::set();
    }

    // 2.8 ms reload
    modm::this_fiber::sleep_for(3ms);

    // Read the factory calibration from PROM
    data.prom.data[0] = modm::fromBigEndian(readProm(0));
    data.prom.data[1] = modm::fromBigEndian(readProm(1));
    data.prom.data[2] = modm::fromBigEndian(readProm(2));
    data.prom.data[3] = modm::fromBigEndian(readProm(3));
    data.prom.data[4] = modm::fromBigEndian(readProm(4));
    data.prom.data[5] = modm::fromBigEndian(readProm(5));
    data.prom.data[6] = modm::fromBigEndian(readProm(6));
    data.prom.data[7] = modm::fromBigEndian(readProm(7));

    return data.prom.calculateCrc() == (data.prom.data[7] & 0x000F);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Ms5611<SpiMaster,Cs>::readout(OversamplingRatio osrPressure, OversamplingRatio osrTemperature)
{
    // start a pressure conversion
    buffer[0] = i(Command::Convert) | i(Conversion::Pressure) | i(osrPressure);

    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    SpiMaster::transfer(buffer, nullptr, 1);

    if (this->releaseMaster())
    {
        Cs::set();
    }

    // Wait until pressure conversion has finished
    modm::this_fiber::sleep_for(std::chrono::milliseconds(conversionDelay[i(osrPressure) >> 1]));

    // Get the pressure conversion result from sensor
    buffer[0] = i(Command::AdcRead);

    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    SpiMaster::transfer(buffer, nullptr, 1);
    SpiMaster::transfer(nullptr, data.raw, 3);

    if (this->releaseMaster())
    {
        Cs::set();
    }

    // Notify data class about changed buffer
    data.rawPressureTouched();

    // start a temperature conversion
    buffer[0] = i(Command::Convert) | i(Conversion::Temperature) | i(osrTemperature);

    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    SpiMaster::transfer(buffer, nullptr, 1);

    if (this->releaseMaster())
    {
        Cs::set();
    }

    // Wait until temperature conversion has finished
    modm::this_fiber::sleep_for(std::chrono::milliseconds(conversionDelay[i(osrTemperature) >> 1]));

    // Get the temperature conversion result from sensor
    buffer[0] = i(Command::AdcRead);

    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    SpiMaster::transfer(buffer, nullptr, 1);
    SpiMaster::transfer(nullptr, data.raw + 3, 3);

    if (this->releaseMaster())
    {
        Cs::set();
    }

    // Notify data class about changed buffer
    data.rawTemperatureTouched();
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
uint16_t
Ms5611<SpiMaster,Cs>::readProm(uint8_t address)
{
    buffer[0] = i(Command::PromRead) | ((address & 0b111) << 1);

    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    SpiMaster::transfer(buffer, nullptr, 1);
    SpiMaster::transfer(nullptr, buffer, 2);

    if (this->releaseMaster())
    {
        Cs::set();
    }

    return static_cast<uint16_t>((buffer[1] << 8) | buffer[0]);
}

} // modm namespace
