/*
 * Copyright (c) 2023, Lasse Alexander Jensen
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SX128X_HPP
#   error "Don't include this file directly, use 'sx128x.hpp' instead!"
#endif

#include <cstring>

namespace modm
{

// ----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
bool
Sx128x< Transport, Reset, Busy >::isBusy()
{
    return Busy::read();
}

// ----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<void>
Sx128x< Transport, Reset, Busy >::reset()
{
    RF_BEGIN();

    Reset::setOutput(modm::Gpio::Low);
    timeout.restart(std::chrono::milliseconds(50));
    RF_WAIT_UNTIL(timeout.isExpired());

    Reset::setOutput(modm::Gpio::High);
    timeout.restart(std::chrono::milliseconds(20));
    RF_WAIT_UNTIL(timeout.isExpired());

    RF_END();
}

// ----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::getStatus(sx128x::Status *status)
{
    // The GetStatus command can be issued at any time
    return this->writeCommandSingleData(Opcode::GetStatus, std::span{status, 1});
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::writeRegister(Register reg, uint8_t data)
{
    buffer[0] = data;
    return this->writeRegister(reg, std::span{buffer, 1});
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::writeRegister(Register reg, std::span<const uint8_t> data)
{
    RF_BEGIN();

    buffer[0] = (i(reg) >> 8) & 0xFF;
    buffer[1] = i(reg) & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Command(Opcode::WriteRegister, std::span{buffer, 2}), data));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::readRegister(Register reg, uint8_t *value)
{
    return this->readRegister(reg, std::span{value, 1});
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::readRegister(Register reg, std::span<uint8_t> data)
{
    RF_BEGIN();

    buffer[0] = (i(reg) >> 8) & 0xFF;
    buffer[1] = i(reg) & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->readCommand(Command(Opcode::ReadRegister, std::span{buffer, 2}), data));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::writeBuffer(uint8_t offset, std::span<const uint8_t> data)
{
    RF_BEGIN();

    buffer[0] = offset;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Command(Opcode::WriteBuffer, std::span{buffer, 1}), data));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::readBuffer(uint8_t offset, std::span<uint8_t> data)
{
    RF_BEGIN();

    buffer[0] = offset;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->readCommand(Command(Opcode::ReadBuffer, std::span{buffer, 1}), data));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setSleep(SleepConfig_t sleepConfig)
{
    RF_BEGIN();

    buffer[0] = sleepConfig.value;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetSleep, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setStandby(StandbyConfig standbyConfig)
{
    RF_BEGIN();

    buffer[0] = i(standbyConfig);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetStandby, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setFs()
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommandSingleData(Opcode::SetFs));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setTx(PeriodBase periodBase, uint16_t periodBaseCount)
{
    RF_BEGIN();

    buffer[0] = i(periodBase);
    buffer[1] = (periodBaseCount >> 8) & 0xFF;
    buffer[2] = periodBaseCount & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetTx, std::span{buffer, 3}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setRx(PeriodBase periodBase, uint16_t periodBaseCount)
{
    RF_BEGIN();

    buffer[0] = i(periodBase);
    buffer[1] = (periodBaseCount >> 8) & 0xFF;
    buffer[2] = periodBaseCount & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetRx, std::span{buffer, 3}) );
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setRxDutyCycle(PeriodBase periodBase, uint16_t rxPeriodBaseCount, uint16_t sleepPeriodBaseCount)
{
    RF_BEGIN();

    buffer[0] = i(periodBase);
    buffer[1] = (rxPeriodBaseCount >> 8) & 0xFF;
    buffer[2] = rxPeriodBaseCount & 0xFF;
    buffer[3] = (sleepPeriodBaseCount >> 8) & 0xFF;
    buffer[4] = sleepPeriodBaseCount & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetRxDutyCycle, std::span{buffer, 5}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setCad()
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommandSingleData(Opcode::SetCad));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setTxContinuousWave()
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommandSingleData(Opcode::SetTxContinuousWave));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setTxContinuousPreamble()
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommandSingleData(Opcode::SetTxContinuousPreamble));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setPacketType(PacketType packetType)
{
    RF_BEGIN();

    buffer[0] = i(packetType);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetPacketType, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::getPacketType(PacketType *packetType)
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());
    if (RF_CALL(this->readCommand(Opcode::GetPacketType, std::span{(uint8_t *) packetType, 1})))
    {
        *packetType = PacketType(buffer[0]);
        RF_RETURN(true);
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setRfFrequency(uint32_t rfFrequency)
{
    RF_BEGIN();

    buffer[0] = (rfFrequency >> 16) & 0xFF;
    buffer[1] = (rfFrequency >> 8) & 0xFF;
    buffer[2] = rfFrequency & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetRfFrequency, std::span{buffer, 3}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setTxParams(uint8_t power, RampTime rampTime)
{
    RF_BEGIN();

    buffer[0] = power;
    buffer[1] = i(rampTime);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetTxParams, std::span{buffer, 2}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setCadParams(CadSymbolNumber cadSymbolNumber)
{
    RF_BEGIN();

    buffer[0] = uint8_t(cadSymbolNumber);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetCadParams, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
    RF_BEGIN();

    buffer[0] = txBaseAddress;
    buffer[1] = rxBaseAddress;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetBufferBaseAddress, std::span{buffer, 2}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setModulationParams(ModulationParams modulationParams)
{
    RF_BEGIN();

    std::memcpy(buffer, (uint8_t *) &modulationParams, 3);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetModulationParams, std::span{buffer, 3}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setPacketParams(PacketParams packetParams)
{
    RF_BEGIN();

    std::memcpy(buffer, (uint8_t *) &packetParams, 7);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetPacketParams, std::span{buffer, 7}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::getRxBufferStatus(RxBufferStatus *rxBufferStatus)
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());
    if (RF_CALL(this->readCommand(Opcode::GetRxBufferStatus, std::span{buffer, 2})))
    {
        std::memcpy((uint8_t *) rxBufferStatus, buffer, 2);
        RF_RETURN(true);
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::getPacketStatus(PacketStatus *packetStatus)
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());
    if (RF_CALL(this->readCommand(Opcode::GetPacketStatus, std::span{buffer, 5})))
    {
        std::memcpy((uint8_t *) packetStatus, buffer, 5);
        RF_RETURN(true);
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::getRssiInst(uint8_t *rssiInst)
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());
    if (RF_CALL(this->readCommand(Opcode::GetRssiInst, std::span{buffer, 1})))
    {
        *rssiInst = buffer[0];
        RF_RETURN(true);
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setDioIrqParams(Irq_t irqMask, Irq_t dio1Mask, Irq_t dio2Mask, Irq_t dio3Mask)
{
    RF_BEGIN();

    buffer[0] = (irqMask.value >> 8) & 0xFF;
    buffer[1] = irqMask.value & 0xFF;
    buffer[2] = (dio1Mask.value >> 8) & 0xFF;
    buffer[3] = dio1Mask.value & 0xFF;
    buffer[4] = (dio2Mask.value >> 8) & 0xFF;
    buffer[5] = dio2Mask.value & 0xFF;
    buffer[6] = (dio3Mask.value >> 8) & 0xFF;
    buffer[7] = dio3Mask.value & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetDioIrqParams, std::span{buffer, 8}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::getIrqStatus(Irq_t *irqStatus)
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());
    if (RF_CALL(this->readCommand(Opcode::GetIrqStatus, std::span{buffer, 2})))
    {
        *irqStatus = Irq_t((buffer[0] << 8) | buffer[1]);
        RF_RETURN(true);
    }

    RF_END_RETURN(false);
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::clearIrqStatus(Irq_t irqMask)
{
    RF_BEGIN();

    buffer[0] = (irqMask.value >> 8) & 0xFF;
    buffer[1] = irqMask.value & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::ClrIrqStatus, std::span{buffer, 2}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setRegulatorMode(RegulatorMode regModeParam)
{
    RF_BEGIN();

    buffer[0] = i(regModeParam);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetRegulatorMode, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setSaveContext()
{
    RF_BEGIN();

    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommandSingleData(Opcode::SetSaveContext));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setAutoFs(bool enable)
{
    RF_BEGIN();

    buffer[0] = enable ? 0x01 : 0x00;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetAutoFs, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setAutoTx(uint16_t time)
{
    RF_BEGIN();

    buffer[0] = (time >> 8) & 0xFF;
    buffer[1] = time & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetAutoTx, std::span{buffer, 2}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setLongPreamble(bool enable)
{
    RF_BEGIN();

    buffer[0] = enable ? 0x01 : 0x00;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetLongPreamble, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setUartSpeed(UartDividerRatio uartDividerRatio)
{
    RF_BEGIN();

    buffer[0] = (i(uartDividerRatio) >> 8) & 0xFF;
    buffer[1] = i(uartDividerRatio) & 0xFF;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetLongPreamble, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setRangingRole(RangingRole rangingRole)
{
    RF_BEGIN();

    buffer[0] = i(rangingRole);
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetLongPreamble, std::span{buffer, 1}));
}

// -----------------------------------------------------------------------------

template < class Transport, class Reset, class Busy >
modm::ResumableResult<bool>
Sx128x< Transport, Reset, Busy >::setAdvancedRanging(bool enable)
{
    RF_BEGIN();

    buffer[0] = enable ? 0x01 : 0x00;
    RF_WAIT_WHILE(isBusy());

    RF_END_RETURN_CALL(this->writeCommand(Opcode::SetAdvancedRanging, std::span{buffer, 1}));
}

}