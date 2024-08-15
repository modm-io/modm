/*
 * Copyright (c) 2024, Elias H.
 * Copyright (c) 2024, Raphael Lehmann
 * Copyright (c) 2024, Michael Jossen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_DW3110_PHY_HPP
#define MODM_DW3110_PHY_HPP

#include <array>
#include <cstdint>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/processing/timer/timeout.hpp>
#include <span>

#include "dw3110_definitions.hpp"

namespace modm
{

/**
 * Hardware abstraction layer for DW3110
 * Unsupported Features: AES encryption, Double buffering, GPIO, Temperature and Voltage, Pulse
 * Generator calibration, RX antenna delay temp compensation, Soft reset, Packet filtering,
 * Sleep, Sniff mode, Delayed transmission, STS
 * @ingroup  modm_driver_dw3110
 * @author   Elias H.
 * @author   Raphael Lehmann
 * @author   Michael Jossen
 */
template<typename SpiMaster, typename Cs>
class Dw3110Phy : public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<6>
{
public:
	Dw3110Phy();

	// Set the UWB channel used
	modm::ResumableResult<void>
	setChannel(Dw3110::Channel channel);

	// Change Header format to non-standard to allow 1021 Byte payloads instead of default 125
	modm::ResumableResult<void>
	setEnableLongFrames(bool value);

	// Send the PHR at 6.8Mbps
	// By default the PHR is always sent at 850kb/s
	modm::ResumableResult<void>
	setSendHeaderFast(bool value);

	// Set the timeout to wait on arriving packets after startReceive()
	// 0 = No Timeout
	// Register resolution is ~1µs
	// TODO test
	modm::ResumableResult<void>
	setReceiveWaitTimeout(modm::chrono::micro_clock::duration duration);

	// Set whether to stay in receive mode after receive failure
	// TODO test
	modm::ResumableResult<void>
	setReenableOnRxFailure(bool value);

	// Set the time spent listening for competing transmissions on CCA commands
	// Unit is in counts of PAC symbols
	// TODO test
	modm::ResumableResult<void>
	setCCATimeout(uint16_t timeout);

	// Enable the device to automatically respond to any received frames with an ACK
	// TODO test
	modm::ResumableResult<void>
	setEnableAutoAcknowledge(bool value);

	// Enable a faster TX/RX turnaround.
	// The Frame will be set ready before computation of ToA data has finished and any ACKs will be
	// sent
	// Time of Arrival may not be ready when the frame is made available
	// TODO test
	modm::ResumableResult<void>
	setEnableFastTurnaround(bool value);

	// Read the value of the internal chip clock
	// TODO test
	modm::ResumableResult<uint32_t>
	readChipTime();

	// Set the number of preamble symbols sent with each transmission
	// Each symbol takes ~1µs to send
	// Note: Fine tuning of preamble length is currently unsupported by this driver
	modm::ResumableResult<void>
	setPreambleLength(Dw3110::PreambleLength len);

	// Get the timestamp of the last arrived packet.
	// This timestamp already has various correction factors applied to it.
	// It is given in ~15.65 picoseconds per unit
	modm::ResumableResult<uint64_t>
	getReceiveTimestamp();

	// Analogous to getReceiveTimestamp
	modm::ResumableResult<uint64_t>
	getTransmitTimestamp();

	// Set the time between RX of a packet and the TX of the acknowledgement
	// Specified in number of preamble symbols, so time depends on the PRF
	// TODO test
	modm::ResumableResult<void>
	setAcknowledgeTurnaround(uint8_t time);

	// Set the time between a transmission and the start of RX on any of the RX after TX commands
	// This can be used to delay turning on of the receiver after transmission to save on power.
	// TODO test
	modm::ResumableResult<void>
	setWaitForResponseTime(modm::PreciseClock::duration time);

	// Set the start frame delimiter used by the chip
	modm::ResumableResult<void>
	setSFD(Dw3110::StartFrameDelimiter sfd);

	// Set the preamble code used and looked for.
	// Note that this implicitly sets the PRF as codes >8 use the 64MHz PRF instead of 16MHz
	modm::ResumableResult<void>
	setPreambleCode(Dw3110::PreambleCode rx, Dw3110::PreambleCode tx);

	// Read the reported state of the chip
	modm::ResumableResult<Dw3110::SystemState>
	getChipState();

	// Runs the RX calibration, needs to be done after startup, wake and after 20C temperature
	// change
	// Needs to be run in IDLE_PLL state
	// Automatically run in initialize
	modm::ResumableResult<bool>
	calibrate();

	// Initialize the DW3000, make ready to receive/transmit
	// Call ideally after the IRQ Pin has gone high, signalling SPIRDY
	modm::ResumableResult<bool>
	initialize(Dw3110::Channel channel = Dw3110::Channel::Channel5,
			   Dw3110::PreambleCode pcode = Dw3110::PreambleCode::Code_64Mhz_9,
			   Dw3110::PreambleLength plen = Dw3110::PreambleLength::Preamble_64,
			   Dw3110::StartFrameDelimiter sfd = Dw3110::StartFrameDelimiter::IEEE802_15_4z_8);

	// Set the chip into receive mode
	modm::ResumableResult<bool>
	startReceive();

	// Check if a packet has been successfully received
	modm::ResumableResult<bool>
	packetReady();

	// Copy received packet into the provided payload buffer, clear packet received flags
	modm::ResumableResult<bool>
	fetchPacket(std::span<uint8_t> payload, size_t& payload_len);

	// Transmit a given package using the current configuration
	// fast parameter decides if the data portion is sent at 850kbps or 6.8Mbps
	template<size_t Len>
	modm::ResumableResult<bool>
	transmit(const std::span<const uint8_t, Len> payload, size_t payload_len, bool fast = true);

	// Transmit a given package using the current configuration and instantly enter a receiving
	// state
	// fast parameter decides if the data portion is sent at 850kbps or 6.8Mbps
	template<size_t Len>
	modm::ResumableResult<bool>
	transmitAndStartReceive(const std::span<const uint8_t, Len> payload, size_t payload_len,
							bool fast = true);

	// Read the current system status register
	modm::ResumableResult<Dw3110::SystemStatus>
	getStatus();

private:
	// Transmit a given package using the current configuration and a specific command
	// fast parameter decides if the data portion is sent at 850kbps or 6.8Mbps
	template<size_t Len, modm::Dw3110::FastCommand Cmd>
	modm::ResumableResult<bool>
	transmitGeneric(const std::span<const uint8_t, Len> payload, size_t payload_len, bool fast);

	// Only load configuration independent stuff, everything else should be initialized when
	// changing those parts
	modm::ResumableResult<void>
	loadOTP();

	// Perform some test writes and readbacks over the SPI interface
	modm::ResumableResult<bool>
	testSPIConnection();

	// Helper function checking equality of two spans
	template<size_t Len>
	bool
	checkResult(std::span<const uint8_t, Len> expected, std::span<const uint8_t, Len> got);

	// Send a command to the chip
	template<Dw3110::FastCommand Cmd>
	modm::ResumableResult<void>
	sendCommand();

	// Recompute the SFD TOC
	modm::ResumableResult<void>
	setRX_SFD_TOC();

	// Update the local system_status variable
	modm::ResumableResult<void>
	fetchSystemStatus();

	// Update the local chip_state variable
	modm::ResumableResult<void>
	fetchChipState();

	// Read a variable from the OTP memory
	template<Dw3110::OTPAddr Addr>
	modm::ResumableResult<void>
	readOTPMemory(std::span<uint8_t, 4> out);

	// Read a variable from a register
	template<Dw3110::Register Reg, size_t Len, size_t Offset = 0>
	modm::ResumableResult<void>
	readRegister(std::span<uint8_t, Len> out);

	// Read a number of bytes from a register bank, useful for RX buffers and other large read
	// transfers
	template<Dw3110::RegisterBank Reg>
	modm::ResumableResult<void>
	readRegisterBank(std::span<uint8_t> out, size_t len);

	// Write a variable to a register
	template<Dw3110::Register Reg, size_t Len, size_t Offset = 0>
	modm::ResumableResult<void>
	writeRegister(const std::span<const uint8_t, Len> val);

	// Simple implementation of read modify write using and and or masks
	// Use on registers that do not support native write register masked
	template<Dw3110::Register Reg, size_t Len, size_t Offset = 0>
	modm::ResumableResult<void>
	readModifyWriteRegister(const std::span<const uint8_t, Len> or_mask,
							const std::span<const uint8_t, Len> and_mask);

	// Do not use to clear "write 1 to clear" bits (2.3.1.2 Table 3)
	template<Dw3110::Register Reg, size_t Len, size_t Offset = 0>
	modm::ResumableResult<void>
	writeRegisterMasked(const std::span<const uint8_t, Len> or_mask,
						const std::span<const uint8_t, Len> and_mask);

	// Write a number of bytes to a register bank, useful for TX buffers and other large write
	// transfers
	template<Dw3110::RegisterBank Reg>
	modm::ResumableResult<void>
	writeRegisterBank(const std::span<const uint8_t> val, size_t len);

	PreciseTimeout timeout;

	Dw3110::SystemStatus system_status{0};
	uint16_t preamble_len{0}, sfd_len{0}, pac_len{0}, sfd_toc_val{0}, fcs_len{2};
	Dw3110::SystemState chip_state{Dw3110::SystemState::OFF};
	std::array<uint8_t, 16> scratch{};
	std::array<uint8_t, 6> sys_status{}, tx_info{};
	std::array<uint8_t, 2> tx_buffer{}, chan_ctrl{}, rx_sfd_toc{};
	std::array<uint8_t, 4> otp_read{}, rx_cal_res{}, sys_state{}, ldo_config{}, temp_rw{},
		rx_finfo{};
	std::array<uint8_t, 1> xtal{}, bias_ctrl{}, rx_cal_sts{};
};

}  // namespace modm

#include "dw3110_phy_impl.hpp"

#endif