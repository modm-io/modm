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

#ifndef MODM_SX128X_TRANSPORT_HPP
#	error  "Don't include this file directly, use 'sx128x_transport.hpp' instead!"
#endif

#include <cstring>

namespace modm
{

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
Sx128xTransportSpi<SpiMaster, Cs>::writeCommandSingleData(Command command, uint8_t *data)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());
    Cs::reset();

    command.getOpcode(commandBuffer);
    RF_CALL(SpiMaster::transfer(commandBuffer, data, 1));

    if (this->releaseMaster())
        Cs::set();

    RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
Sx128xTransportSpi<SpiMaster, Cs>::writeCommand(Command command, std::span<const uint8_t> data)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());
    Cs::reset();

    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
    }

    RF_CALL(SpiMaster::transfer(commandBuffer, nullptr, command.getVargsCount() + 1));
    RF_CALL(SpiMaster::transfer(&data[0], nullptr, data.size()));

    if (this->releaseMaster())
        Cs::set();

    RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
Sx128xTransportSpi<SpiMaster, Cs>::readCommand(Command command, std::span<uint8_t> data)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());
    Cs::reset();

    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
        commandBuffer[1 + vargs.size()] = 0x00;
    } else {
        commandBuffer[1] = 0x00;
    }

    RF_CALL(SpiMaster::transfer(commandBuffer, nullptr, command.getVargsCount() + 2));
    RF_CALL(SpiMaster::transfer(nullptr, &data[0], data.size()));

    if (this->releaseMaster())
        Cs::set();

    RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template < class Uart >
modm::ResumableResult<bool>
Sx128xTransportUart<Uart>::writeCommandSingleData(Command command, uint8_t *data)
{
    RF_BEGIN();

    command.getOpcode(commandBuffer);
    if (RF_CALL(this->write(commandBuffer[0]))) {
        if (data != nullptr) {
            RF_RETURN_CALL(this->read(data));
        } else {
            RF_RETURN(true);
        }
    }

    RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------

template < class Uart >
modm::ResumableResult<bool>
Sx128xTransportUart<Uart>::writeCommand(Command command, std::span<const uint8_t> data)
{
    RF_BEGIN();

    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
        commandBuffer[1 + vargs.size()] = data.size();
    } else {
        commandBuffer[1] = data.size();
    }

    if (RF_CALL(this->write(commandBuffer, 2 + command.getVargsCount()))) {
        RF_RETURN_CALL(this->write(&data[0], data.size()));
    }

    RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------

template < class Uart >
modm::ResumableResult<bool>
Sx128xTransportUart<Uart>::readCommand(Command command, std::span<uint8_t> data)
{
    RF_BEGIN();

    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
        commandBuffer[1 + vargs.size()] = data.size();
    } else {
        commandBuffer[1] = data.size();
    }

    if (RF_CALL(this->write(commandBuffer, 2 + command.getVargsCount()))) {
        RF_RETURN_CALL(this->read(&data[0], data.size()));
    }

    RF_END_RETURN(false);
}

} // namespace modm