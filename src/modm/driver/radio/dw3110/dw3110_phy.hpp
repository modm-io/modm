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
 *
 * @ingroup  modm_driver_dw3110
 * @author   Elias H.
 * @author   Raphael Lehmann
 * @author   Michael Jossen
 */
template<typename SpiMaster, typename Cs>
class Dw3110Phy : public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<6>
{
public:
	// Unsupported Features: AES encryption, Double buffering, GPIO, Temperature and Voltage, Pulse
	// Generator calibration, RX antenna delay temp compensation, Soft reset, Packet filtering,
	// Sleep, Sniff mode

	// Notes:
	// Communication is little endian!
	// IRQ Pin floats when in SLEEP/DEEPSLEEP
	// TODO Sts, Rx/Tx Timestamps
	// CMD_TRXOFF disabled TX/RX states
	// KICK for OTP may not be doing anything
	// TODO Find default value of RF_RX_CTRL_HI for setting to ch5
	// TODO Set RF_RX_CTRL_HI to 0x08B5A833 for ch9
	Dw3110Phy();

	// Set the UWB channel used
	modm::ResumableResult<void> setChannel(Dw3110::Channel);

	// Change Header format to non-standard to allow 1023 Byte payloads instead of default 127
	// TODO set PHR_MODE in SYS_CFG
	modm::ResumableResult<void>
	setEnableLongFrames(bool value);

	// Send the PHR at 6.8Mbps
	// By default the PHR is always sent at 850kb/s
	// TODO set PHR_6M8 in SYS_CFG
	modm::ResumableResult<void>
	setSendHeaderFast(bool value);

	// Set the timeout to wait on arriving packets after startReceive()
	//  0 = No Timeout
	// TODO Set RXWTOE in SYS_CFG
	// TODO Write timeout to RX_FWTO
	// TODO register resolution is ~1µs
	modm::ResumableResult<void>
	setReceiveWaitTimeout(modm::PreciseClock::duration duration);

	// Set whether to stay in receive mode after receive failure
	// TODO Set RXAUTR in SYS_CFG
	modm::ResumableResult<void>
	setReenableOnRxFailure(bool value);

	// Set the time spent listening for competing transmissions on CCA commands
	// Unit is in counts of PAC symbols
	// TODO write to PRE_TOC
	modm::ResumableResult<void>
	setCCATimeout(uint16_t timeout);

	// Enable the device to automatically respond to any received frames with an ACK
	// TODO set AUTO_ACK in SYS_CFG
	modm::ResumableResult<void>
	setEnableAutoAcknowledge(bool value);

	// Set whether the device expects and sends an STS in between SDF and PHR
	// Must be the same on receiving and transmitting devices.
	// Other STS modes are unsupported by this driver as of now
	// TODO set value 1/0 to CP_SPC in SYS_CFG
	modm::ResumableResult<void>
	setEnableSTS(bool value);

	// Set whether to use a user defined key for the STS or a predefined key
	// The predefined key is hard coded across all DW3000 and as such not secure!
	// The key is supposedly optimized for ToA performance (e.g Ranging)
	// TODO set CP_SDC in SYS_CFG
	modm::ResumableResult<void>
	setSTSIgnoreKey(bool value);

	// Enable a faster TX/RX turnaround.
	// The Frame will be set ready before computation of ToA data has finished and any ACKs will be
	// sent
	// Time of Arrival may not be ready when the frame is made available
	// TODO set FAST_AAT in SYS_CFG
	modm::ResumableResult<void>
	setEnableFastTurnaround(bool value);

	// Read the value of the internal chip clock
	// TODO write SYS_TIME to clear latched value
	// TODO read SYS_TIME
	modm::ResumableResult<uint32_t>
	readChipTime();

	// Set the number of preamble symbols sent with each transmission
	// Each symbol takes ~1µs to send
	// Note: Fine tuning of preamble length is currently unsupported by this driver
	modm::ResumableResult<void>
	setPreambleLength(Dw3110::PreambleLength len);

	// Get the timestamp of the last arrived packet.
	// This timestamp already has various correction factors applied to it.
	// TODO read RX_TIME
	// TODO the registers cannot be read in one! (assuming only the jump to Raw is invalid)
	modm::ResumableResult<uint64_t>
	getReceiveTimestamp();

	// Analogous to getReceiveTimestamp
	// TODO read TX_TIME
	modm::ResumableResult<uint64_t>
	getTransmitTimestamp();

	// Get the delay compensation for the antenna
	// TODO read TX_ANTD
	modm::ResumableResult<uint16_t>
	getAntennaDelay();

	// Set the delay compensating for the antenna
	// Useful for calibration
	// TODO write to TX_ANTD
	// TODO write to RX_ANTD in CIA_CONF
	modm::ResumableResult<void> setAntennaDelay(uint16_t);

	// Set the time between RX of a packet and the TX of the acknowledgement
	// Specified in number of preamble symbols, so time depends on the PRF
	// TODO write to ACK_TIM in ACK_RESP_T
	modm::ResumableResult<void>
	setAcknowledgeTurnaround(uint8_t time);

	// Set the time between a transmission and the start of RX on any of the RX after TX commands
	// This can be used to delay turning on of the receiver after transmission to save on power.
	// TODO write value to W4R_TIM in ACK_RESP_T
	modm::ResumableResult<void> setWaitForResponseTime(modm::PreciseClock::duration);

	// Set the start frame delimiter used by the chip
	modm::ResumableResult<void>
	setSFD(Dw3110::StartFrameDelimiter sfd);

	// Set the preamble code used and looked for.
	// Note that this implicitly sets the PRF as codes >8 use the 64MHz PRF instead of 16MHz
	modm::ResumableResult<void>
	setPreambleCode(Dw3110::PreambleCode rx, Dw3110::PreambleCode tx);

	// Set the STS length in multiples of 8, each block taking ~1µs to transmit
	// Note a value of 0 equals 8 blocks
	// Note values lower than 3 are not supported
	// TODO write to CPS_LEN in STS_CFG
	modm::ResumableResult<void>
	setSTSLength(uint8_t length);

	// Load the STS IV into the AES Block, resetting any counter
	// The STS transmission will cause the lower bits of the IV in the AES block to be incremented
	// If Receiver and Transmitter are out of sync, this will cause failures and degrade ToA
	// This resets the loaded value to the one stored in STS_IV
	// TODO set bit LOAD_IV in STS_CTRL
	modm::ResumableResult<void>
	loadSTSIV();

	// Reset the counter in the AES Block to before the last transmission
	// See loadSTSIV
	// TODO set bit RST_LAST in STS_CTRL
	modm::ResumableResult<void>
	useLastSTSCounter();

	// Read the quality of the last received STS
	// If this is degraded, ToA may be inaccurate
	// TODO find out what the values mean
	// TODO read ACC_QUAL in STS_STS
	modm::ResumableResult<uint16_t>
	getSTSQuality();

	// Set the AES key used for generating the STS
	// TODO explain and implement
	modm::ResumableResult<void>
	setSTSKey(const std::array<uint8_t, 16>&);

	// Set the AES IVs used for generating the STS
	// TODO explain and implement
	modm::ResumableResult<void>
	setSTSIV(const std::array<uint8_t, 16>&);

	// Get the STS time of arrival computet by the CIR
	// TODO read STS_TS
	modm::ResumableResult<uint64_t>
	getSTSTimestamp();

	// Get the associated status bits for the STS timestamp
	// true for an okay ToA, false if failed for any reason
	// TODO read STS_TOAST
	modm::ResumableResult<bool>
	getSTSStatus();

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

	modm::ResumableResult<void>
	startReceive();

	// TODO check RXFR and RXFCG
	modm::ResumableResult<bool>
	packetReady();  // TODO

	modm::ResumableResult<bool>
	fetchPacket(std::span<uint8_t> payload, size_t& payload_len);

	template<size_t Len>
	bool
	transmitBlocking(const std::span<const uint8_t, Len> payload, size_t payload_len);

	// TODO allow setting of ranging bit TR
	// TODO allow setting of bitrate TXBR
	// TODO set LOAD_IV in STS_CTRL if STS is enabled
	// TODO add 2 to TXFlen to allow for CRC since DIS_FCS_TX is not set
	// TODO allow delayed transmission
	template<size_t Len>
	modm::ResumableResult<bool>
	transmit(const std::span<const uint8_t, Len> payload, size_t payload_len);

	// Read the current system status register
	modm::ResumableResult<Dw3110::SystemStatus>
	getStatus();

	// TODO change enum
	// TODO write resulting value to SYS_ENABLE to change config of IRQ Pin
	modm::ResumableResult<void>
	setInterrupts(Dw3110::SystemStatus bits);

	// TODO write 1s to SYS_STATUS to reset any bits
	modm::ResumableResult<void>
	resetStatus(Dw3110::SystemStatus bits);

private:
	// Only load configuration independent stuff, everything else should be initialized when
	// changing those parts
	modm::ResumableResult<void>
	loadOTP();

	modm::ResumableResult<bool>
	testSPIConnection();

	template<size_t Len>
	bool
	checkResult(std::span<const uint8_t, Len> expected, std::span<const uint8_t, Len> got);

	template<Dw3110::FastCommand Cmd>
	modm::ResumableResult<void>
	sendCommand();

	modm::ResumableResult<void>
	setRX_SFD_TOC();

	modm::ResumableResult<void>
	fetchSystemStatus();

	modm::ResumableResult<void>
	fetchChipState();

	template<Dw3110::OTPAddr Addr>
	modm::ResumableResult<void>
	readOTPMemory(std::span<uint8_t, 4> out);

	template<Dw3110::Register Reg, size_t Len, size_t Offset = 0>
	modm::ResumableResult<void>
	readRegister(std::span<uint8_t, Len> out);

	template<Dw3110::RegisterBank Reg>
	modm::ResumableResult<void>
	readRegisterBank(std::span<uint8_t> out, size_t len);

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
	std::array<uint8_t, 4> otp_read{}, rx_cal_res{}, sys_state{}, ldo_config{}, temp_rw{}, rx_finfo{};
	std::array<uint8_t, 1> xtal{}, bias_ctrl{}, rx_cal_sts{};
};

}  // namespace modm

#include "dw3110_phy_impl.hpp"

#endif