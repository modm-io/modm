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
#error "Don't include this file directly, use 'mcp3008.hpp' instead!"
#endif

namespace modm
{

template <typename SpiMaster, typename Cs>
void
Mcp3008<SpiMaster, Cs>::initialize()
{
    this->attachConfigurationHandler([]() {
        SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    });

    Cs::setOutput(true);
}

template <typename SpiMaster, typename Cs>
modm::ResumableResult<uint16_t>
Mcp3008<SpiMaster, Cs>::read(Channel channel)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());
    Cs::reset();

    txBuffer_[1] = static_cast<uint8_t>(channel) << 4;

    RF_CALL(SpiMaster::transfer(txBuffer_, rxBuffer_, 3));

    if (this->releaseMaster()) {
        Cs::set();
    }

    RF_END_RETURN(uint16_t(((rxBuffer_[1] & 0b11) << 8) | rxBuffer_[2]));
}

} // namespace modm
