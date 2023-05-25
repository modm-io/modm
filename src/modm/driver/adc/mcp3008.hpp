/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP3008_HPP
#define MODM_MCP3008_HPP

#include <array>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/// @ingroup modm_driver_mcp3008
struct mcp3008
{
    /**
     * ADC channels
     * For MCP3004 only channels 0-3 are valid.
     */
    enum class
    Channel : uint8_t
    {
        Ch0 = 0b1000,
        Ch1 = 0b1001,
        Ch2 = 0b1010,
        Ch3 = 0b1011,
        Ch4 = 0b1100,
        Ch5 = 0b1101,
        Ch6 = 0b1110,
        Ch7 = 0b1111,
        Ch0Ch1Diff = 0b0000,
        Ch1Ch0Diff = 0b0001,
        Ch2Ch3Diff = 0b0010,
        Ch3Ch2Diff = 0b0011,
        Ch4Ch5Diff = 0b0100,
        Ch5Ch4Diff = 0b0101,
        Ch6Ch7Diff = 0b0110,
        Ch7Ch6Diff = 0b0111
    };
};

/**
 * Simple driver for MCP3004/MCP3008 10-bit SAR SPI ADCs.
 * Max SPI frequency 3.6 MHz at 5V, 1.36 MHz at 2.7V.
 *
 * @author  Christopher Durand
 * @ingroup modm_driver_mcp3008
 */
template <typename SpiMaster, typename Cs>
class Mcp3008 : public mcp3008, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<1>
{
public:
    Mcp3008() = default;

    /// Call initialize() before reading ADC
    void initialize();

    /// Read ADC channel
    modm::ResumableResult<uint16_t>
    read(Channel channel);

private:
    uint8_t rxBuffer_[3]{};
    uint8_t txBuffer_[3]{1, 0, 0};
};

} // namespace modm

#include "mcp3008_impl.hpp"

#endif // MODM_MCP3008_HPP
