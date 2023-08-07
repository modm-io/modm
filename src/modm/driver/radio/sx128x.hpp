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
#define MODM_SX128X_HPP

#include "sx128x_definitions.hpp"
#include "sx128x_transport.hpp"

#include <modm/processing/timer.hpp>

namespace modm
{

/**
 * @tparam	Transport   Transpot Layer for SX128x
 *
 * @ingroup modm_driver_sx128x
 * @author	Lasse Alexander Jensen
 * @author  Rasmus Kleist Hørlyck Sørensen
 */
template< class Transport, class Reset, class Busy >
class Sx128x : public sx128x, public Transport
{
    using Command = Transport::Command;

public:
    Sx128x() = default;

    /// Determine if radio is busy
    bool
    isBusy();

    /// Reset the transceiver
    /// @attention It is necessary to reset each radio on a shared bus prior to starting the first SPI communicatio
    modm::ResumableResult<void>
    reset();

public:
    /// Get the transceiver status directly.
    /// @attention this command can be issued at any time as long as it is not the very first command send over the interface
    modm::ResumableResult<bool>
    getStatus(Status *status);

    /// Writes a single byte in a data memory space at the specified address
    modm::ResumableResult<bool>
    writeRegister(Register reg, uint8_t data);

    /// Writes a block of bytes in a data memory space starting at a specific address
    modm::ResumableResult<bool>
    writeRegister(Register reg, std::span<const uint8_t> data);

    /// Read a single byte of data at the given address
    modm::ResumableResult<bool>
    readRegister(Register reg, uint8_t *data);

    /// Read a block of data starting at a given address
    modm::ResumableResult<bool>
    readRegister(Register reg, std::span<uint8_t> data);

    /// This function is used to write the data payload to be transmitted
    /// @attention When the address exceeds 255 it wraps back to 0 due to the circular nature of data buffer
    modm::ResumableResult<bool>
    writeBuffer(uint8_t offset, std::span<const uint8_t> data);

    /// This function is used to read the received data payload
    modm::ResumableResult<bool>
    readBuffer(uint8_t offset, std::span<uint8_t> data);

    /// Set the transceiver to Sleep mode with the lowest current consumption possible
    /// @warning Depending on the specified level of memory retention not all instruction will be retained
    modm::ResumableResult<bool>
    setSleep(SleepConfig_t sleepConfig);

    /// Set the device in either STDBY_RC or STDBY_XOSC mode which are intermediate levels of power consumption
    /// By default in this state, the system is clocked by the 13 MHz RC oscillator to reduce power consumption.
    /// However if the application is time critical, the XOSC block can be turned or left ON.
    /// @attention In this Standby mode, the transceiver may be configured for future RF operations
    modm::ResumableResult<bool>
    setStandby(StandbyConfig standbyConfig = StandbyConfig::StdbyRc);

    /// Set the device in Frequency Synthesizer mode where the PLL is locked to the carrier frequency
    /// @attention In normal operation of the transceiver, the user does not normally need to use FS mode
    modm::ResumableResult<bool>
    setFs();

    /// Set the device in Transmit mode
    /// @warning The IRQ status must be cleared before using this command
    modm::ResumableResult<bool>
    setTx(PeriodBase periodBase, uint16_t periodBaseCount);

    /// Set the device in Receiver mode
    /// @warning The IRQ status must be cleared before using this command
    /// @attention Setting periodBaseCount = 0 puts the transceiver in RX Single Mode
    /// @attention Setting periodBaseCount = 0xFFFF puts the transceiver in Rx Continuous mode
    modm::ResumableResult<bool>
    setRx(PeriodBase periodBase, uint16_t periodBaseCount);

    /// Set the transceiver in sniff mode, so that it regularly looks for new packets
    /// @warning RxDone interrupt must be configured prior to enter Duty cycled operations
    /// @warning SetLongPreamble must be issued prior to setRxDutyCycle
    modm::ResumableResult<bool>
    setRxDutyCycle(PeriodBase periodBase, uint16_t rxPeriodBaseCount, uint16_t sleepPeriodBaseCount);

    /// Set the transceiver to use CAD (Channel Activity Detection) mode
    /// @warning The Channel Activity Detection is a LoRa specific mode of operation
    modm::ResumableResult<bool>
    setCad();

    /// Set the transceiver to generate a Continuous Wave (RF tone) at a selected frequency and output power
    /// @attention The device remains in Tx Continuous Wave until the host sends a mode confiquration command.
    modm::ResumableResult<bool>
    setTxContinuousWave();

    /// Set the transceiver to generate an infinite sequence of alternating 'O's and '1's in GFSK modulation and symbol 0 in LoRa
    /// @attention The device remains in transmit until the host sends a mode confiquration command
    modm::ResumableResult<bool>
    setTxContinuousPreamble();

    /// Set the transceiver radio frame out of a choice of 5 different packet types
    /// @attention the packet type must be set first in the radio configuration sequence
    modm::ResumableResult<bool>
    setPacketType(PacketType packetType);

    /// Get the current operating packet type of the radio
    modm::ResumableResult<bool>
    getPacketType(PacketType *packetType);

    /// Set the frequency of the RF frequency mode.
    /// @warning This must be called after SetPacket type.
    /// @attention The LSB of rfFrequency is equal to the PLL step i.e. 52e6/2^18 Hz, where 52e6 is the crystal frequency in Hz
    modm::ResumableResult<bool>
    setRfFrequency(uint32_t rfFrequency);

    /// Set the Tx output power and the Tx ramp time
    /// @attention The output power (Pout) is defined by the parameter power. Pout = -18 + power
    /// @attention PoutMin = -18 dBm (power = 0) and PoutMax = 13 dBm (power = 31)
    modm::ResumableResult<bool>
    setTxParams(uint8_t power, RampTime rampTime);

    /// Set the number of symbols on which which Channel Activity Detected (CAD) operates
    modm::ResumableResult<bool>
    setCadParams(CadSymbolNumber cadSymbolNumber);

    /// Set the base address for the packet handling operation in Tx and Rx mode for all packet types
    modm::ResumableResult<bool>
    setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);

    /// Set the modulation parameters of the radio
    /// @attention The modulation parameters will be interpreted depending on the frame type
    modm::ResumableResult<bool>
    setModulationParams(ModulationParams modulationParams);

    /// Set the parameters of the packet handling block
    /// @warning Interpretation by the transceiver of the packet parameters depends upon the chosen packet type
    modm::ResumableResult<bool>
    setPacketParams(PacketParams packetParams);

    /// Get length of the last received packet and the address of the first byte received
    modm::ResumableResult<bool>
    getRxBufferStatus(RxBufferStatus *rxBufferStatus);

    /// Retrieve information about the last received packet
    /// @attention The returned parameters are frame-dependent
    /// @attention Actual signal power is -(rssiSync) / 2 (dBm)
    /// @attention Actual SNR is (snr) / 4 (dBm)
    modm::ResumableResult<bool>
    getPacketStatus(PacketStatus *packetStatus);

    /// Get the instantaneous RSSI value during reception of the packet
    /// @attention Actual signal power is -(rssiSync) / 2 (dBm)
    modm::ResumableResult<bool>
    getRssiInst(uint8_t *rssiInst);

    /// Enable IRQs and to route IRQs to DIO pins.
    modm::ResumableResult<bool>
    setDioIrqParams(Irq_t irqMask, Irq_t dio1Mask = Irq_t(), Irq_t dio2Mask = Irq_t(), Irq_t dio3Mask = Irq_t());

    /// Get the value of the IRQ register
    modm::ResumableResult<bool>
    getIrqStatus(Irq_t *irqStatus);

    /// Clear IRQ flags in the IRQ register
    modm::ResumableResult<bool>
    clearIrqStatus(Irq_t irqMask);

    /// Specify if DC-DC or LDO is used for power regulation
    modm::ResumableResult<bool>
    setRegulatorMode(RegulatorMode regModeParam);

    /// Save the present context of the radio register values to the Data RAM
    modm::ResumableResult<bool>
    setSaveContext();

    /// Set the chip so that the state following a Rx or Tx operation is FS and not STDBY
    /// This feature is to be used to reduce the switching time between consecutive Rx and/or Tx operations
    modm::ResumableResult<bool>
    setAutoFs(bool enable = true);

    /// Set the device to be able to send back a response 150 us after a packet reception
    /// @attention The delay between the packet reception end and the next packet transmission start is defined by TxDelay = time + 33 us
    modm::ResumableResult<bool>
    setAutoTx(uint16_t time);

    /// Set the transceiver into Long Preamble mode
    /// @warning Long Preamble mode can only be used with either LoRa mode and GFSK mode
    modm::ResumableResult<bool>
    setLongPreamble(bool enable = true);

    /// Set UART speed
    /// @warning UART only
    /// @attention UART communication must be initiated with 115.2 kbps
    /// @attention The UartDividerRatio is given by UartDividerRatio = (Baud Rate * 2^20) / fClk
    modm::ResumableResult<bool>
    setUartSpeed(UartDividerRatio uartDividerRatio);

    /// Set the role of the radio in ranging operation
    modm::ResumableResult<bool>
    setRangingRole(RangingRole rangingRole);

    /// Enable advanced ranging
    modm::ResumableResult<bool>
    setAdvancedRanging(bool enable = true);

public:
    // The LSB of rfFrequency is equal to the PLL step i.e. 52e6/2^18 Hz, where 52e6 is the crystal frequency in Hz
    static constexpr float frequencyLsb = float(52_MHz) / 262144.f;

private:
    ShortPreciseTimeout timeout;
    uint8_t buffer[8];
};

}

#include "sx128x_impl.hpp"

#endif