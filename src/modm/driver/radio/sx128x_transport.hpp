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
#define MODM_SX128X_TRANSPORT_HPP

#include "sx128x_definitions.hpp"

#include <algorithm>
#include <span>

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/uart.hpp>
#include <modm/math/utils/endianness.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/processing/timer.hpp>

namespace modm
{

/**
 * SX128X Transport Layer Interface
 *
 * @ingroup modm_driver_sx128x_transport
 * @author	Rasmus Kleist Hørlyck Sørensen
 */
class Sx128xTransport
{
public:
    struct Command : sx128x
    {
        Command(Opcode opcode) : opcode(opcode) {}
        Command(Opcode opcode, std::span<const uint8_t> vargs) : opcode(opcode), vargs(vargs) {}

        // DATA ACCESS
        ///@{
        ///
        Opcode inline
        getOpcode() const
        { return opcode; }

        void inline
        getOpcode(Opcode *opcode) const
        { *opcode = getOpcode(); }

        void inline
        getOpcode(uint8_t *opcode) const
        { *opcode = i(getOpcode()); }

        std::span<const uint8_t> inline
        getVargs() const
        { return vargs; }
        ///@}

        bool inline
        hasVargs() const
        { return !vargs.empty(); }

        std::size_t inline
        getVargsCount() const
        { return vargs.size(); }

    private:
        Opcode opcode;
        std::span<const uint8_t> vargs;
    };
};

/**
 * SX128X SPI Transport Layer.
 *
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @ingroup modm_driver_sx128x_transport
 * @author	Rasmus Kleist Hørlyck Sørensen
 */
template < class SpiMaster, class Cs >
class Sx128xTransportSpi : public Sx128xTransport, public SpiDevice< SpiMaster >, protected NestedResumable<2>
{
public:
    Sx128xTransportSpi() = default;

protected:
    modm::ResumableResult<bool>
    writeCommandSingleData(Command command, uint8_t *data = nullptr);

    modm::ResumableResult<bool>
    writeCommand(Command command, std::span<const uint8_t> data);

    modm::ResumableResult<bool>
    readCommand(Command command, std::span<uint8_t> data);

private:
    uint8_t commandBuffer[4];
};

/**
 * SX128X UART Transport Layer.
 *
 * @tparam	Uart	UART interface
 *
 * @ingroup modm_driver_sx128x_transport
 * @author	Rasmus Kleist Hørlyck Sørensen
 */
template < class Uart >
class Sx128xTransportUart : public Sx128xTransport, public UartDevice< Uart, 2 >
{
public:
    Sx128xTransportUart() = default;

protected:
    modm::ResumableResult<bool>
    writeCommandSingleData(Command command, uint8_t *data = nullptr);

    modm::ResumableResult<bool>
    writeCommand(Command command, std::span<const uint8_t> data);

    modm::ResumableResult<bool>
    readCommand(Command command, std::span<uint8_t> data);

private:
    uint8_t commandBuffer[4];
};

} // namespace modm

#include "sx128x_transport_impl.hpp"

#endif	// MODM_SX128X_TRANSPORT_HPP
