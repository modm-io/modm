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
bool
Sx128xTransportSpi<SpiMaster, Cs>::writeCommandSingleData(Command command, uint8_t *data)
{
    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    command.getOpcode(commandBuffer);
    SpiMaster::transfer(commandBuffer, data, 1);

    if (this->releaseMaster())
        Cs::set();

    return true;
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
bool
Sx128xTransportSpi<SpiMaster, Cs>::writeCommand(Command command, std::span<const uint8_t> data)
{
    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
    }

    SpiMaster::transfer(commandBuffer, nullptr, command.getVargsCount() + 1);
    SpiMaster::transfer(&data[0], nullptr, data.size());

    if (this->releaseMaster())
        Cs::set();

    return true;
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
bool
Sx128xTransportSpi<SpiMaster, Cs>::readCommand(Command command, std::span<uint8_t> data)
{
    modm::this_fiber::poll([&]{ return this->acquireMaster(); });
    Cs::reset();

    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
        commandBuffer[1 + vargs.size()] = 0x00;
    } else {
        commandBuffer[1] = 0x00;
    }

    SpiMaster::transfer(commandBuffer, nullptr, command.getVargsCount() + 2);
    SpiMaster::transfer(nullptr, &data[0], data.size());

    if (this->releaseMaster())
        Cs::set();

    return true;
}

// ----------------------------------------------------------------------------

template < class Uart >
bool
Sx128xTransportUart<Uart>::writeCommandSingleData(Command command, uint8_t *data)
{
    command.getOpcode(commandBuffer);
    if (this->write(commandBuffer[0])) {
        if (data != nullptr) {
            return this->read(data);
        } else {
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------

template < class Uart >
bool
Sx128xTransportUart<Uart>::writeCommand(Command command, std::span<const uint8_t> data)
{
    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
        commandBuffer[1 + vargs.size()] = data.size();
    } else {
        commandBuffer[1] = data.size();
    }

    if (this->write(commandBuffer, 2 + command.getVargsCount())) {
        return this->write(&data[0], data.size());
    }

    return false;
}

// ----------------------------------------------------------------------------

template < class Uart >
bool
Sx128xTransportUart<Uart>::readCommand(Command command, std::span<uint8_t> data)
{
    command.getOpcode(commandBuffer);
    if (command.hasVargs()) {
        auto vargs = command.getVargs();
        std::memcpy(commandBuffer + 1, vargs.data(), vargs.size());
        commandBuffer[1 + vargs.size()] = data.size();
    } else {
        commandBuffer[1] = data.size();
    }

    if (this->write(commandBuffer, 2 + command.getVargsCount())) {
        return this->read(&data[0], data.size());
    }

    return false;
}

} // namespace modm
