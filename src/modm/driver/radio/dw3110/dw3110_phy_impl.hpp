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
#error "Don't include this file directly, use 'dw3110_phy.hpp' instead!"
#endif

#include <modm/debug/logger.hpp>

#include "dw3110_definitions.hpp"
#include "dw3110_phy.hpp"

template<typename SpiMaster, typename Cs>
modm::Dw3110Phy<SpiMaster, Cs>::Dw3110Phy()
{
	this->attachConfigurationHandler([]() {
		SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
		SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
	});
	Cs::setOutput(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::initialize(Dw3110::Channel channel, Dw3110::PreambleCode pcode,
										   Dw3110::PreambleLength plen,
										   Dw3110::StartFrameDelimiter sfd)
{
	using namespace std::chrono_literals;
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	// Check if SPIRDY returns a correct value
	RF_CALL(fetchSystemStatus());
	if (!((uint64_t)Dw3110::SystemStatusBits::SPIRDY & system_status))  // Check SPIRDY
	{
		this->releaseMaster();
		RF_RETURN(false);
	}

	// Loop until valid state
	// TODO add exit on failure
	RF_CALL(fetchChipState());
	while (chip_state != Dw3110::SystemState::IDLE_RC &&
		   chip_state != Dw3110::SystemState::IDLE_PLL)
	{
		RF_YIELD();
		fetchSystemStatus();
	}

	RF_CALL(loadOTP());

	// Load magic constants
	constexpr static uint8_t rf_tx_ctrl_1_magic[] = {0x0E};
	RF_CALL(writeRegister<Dw3110::RF_TX_CTRL_1, 1>(rf_tx_ctrl_1_magic));

	constexpr static uint8_t ldo_rload_magic[] = {0x14};
	RF_CALL(writeRegister<Dw3110::LDO_RLOAD, 1>(ldo_rload_magic));

	constexpr static uint8_t res_b0_magic[] = {0x9B};
	RF_CALL(writeRegister<Dw3110::STS_CONF_1, 1>(res_b0_magic));

	// constexpr static uint8_t dtune3_magic[] = {0xCC, 0x35, 0x5F, 0xAF};
	// Magic value from the user manual (8.2.7.4)

	// RF_CALL(writeRegister<Dw3110::DTUNE3, 4>(dtune3_magic));
	// TODO maybe only when sending packets with no payload,
	// investigate (github.com/egnor/DW3000_notes.md))

	// Change to IDLE_PLL by setting SEQ_CTRL:AINIT2IDLE
	constexpr static uint8_t and_mask[] = {0xFF};
	constexpr static uint8_t or_mask[] = {0x01};
	RF_CALL(writeRegisterMasked<Dw3110::SEQ_CTRL, 1, 1>(or_mask, and_mask));

	// Wait for a bit to reach IDLE_PLL
	timeout.restart(1ms);
	RF_WAIT_UNTIL(timeout.execute());
	RF_CALL(fetchSystemStatus());
	// Check if PLL is locked and ready
	if (!((uint64_t)Dw3110::SystemStatusBits::CPLOCK & system_status))
	{
		this->releaseMaster();
		RF_RETURN(false);
	}

	// Loop until valid state
	// TODO add exit on failure
	RF_CALL(fetchChipState());
	while (chip_state != Dw3110::SystemState::IDLE_PLL)
	{
		RF_YIELD();
		fetchSystemStatus();
	}

	if (!RF_CALL(calibrate())) { RF_RETURN(false); }
	RF_CALL(setChannel(channel));
	RF_CALL(setPreambleCode(pcode, pcode));
	RF_CALL(setPreambleLength(plen));
	RF_CALL(setSFD(sfd));

	this->releaseMaster();
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<modm::Dw3110::SystemStatus>
modm::Dw3110Phy<SpiMaster, Cs>::getStatus()
{
	RF_BEGIN();
	RF_CALL(fetchSystemStatus());
	RF_END_RETURN(system_status);
}
template<typename SpiMaster, typename Cs>
modm::ResumableResult<modm::Dw3110::SystemState>
modm::Dw3110Phy<SpiMaster, Cs>::getChipState()
{
	RF_BEGIN();
	RF_CALL(fetchChipState());
	RF_END_RETURN(chip_state);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::loadOTP()
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	// Check LDO and kick if not 0
	RF_CALL(readOTPMemory<Dw3110::LDOTUNE_CAL_1>(otp_read));
	if (otp_read[0] == 0 && otp_read[1] == 0 && otp_read[2] == 0 && otp_read[3] == 0)
	{
		RF_CALL(readOTPMemory<Dw3110::LDOTUNE_CAL_2>(otp_read));
	}
	if (otp_read[0] != 0 || otp_read[1] != 0 || otp_read[2] != 0 || otp_read[3] != 0)
	{
		// Set LDO_KICK
		constexpr static uint8_t or_mask[] = {0xC0};
		constexpr static uint8_t and_mask[] = {0xF0};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask, and_mask));
	} else
	{
		// TODO find default if 0
	}

	// Load from OTP according to (github.com/egnor/DW3000_notes.md))
	// XTAL_TRIM = 0x13 |  Len = 2, need bits 0-5 -> XTAL:0-5
	RF_CALL(readOTPMemory<Dw3110::XTAL_TRIM>(otp_read));
	RF_CALL(readRegister<Dw3110::XTAL, 1>(xtal));
	if (xtal[0] & 0x1F)
	{
		xtal[0] = (xtal[0] & 0xE0) | (otp_read[0] & 0x1F);
		RF_CALL(writeRegister<Dw3110::XTAL, 1>(xtal));
	}

	// BIASTUNE_CAL = 0x0A | Len = ?? need bits 16-20 -> BIAS_CTRL:0-5
	RF_CALL(readOTPMemory<Dw3110::BIASTUNE_CAL>(otp_read));
	if (otp_read[0] != 0 || otp_read[1] != 0 || otp_read[2] != 0 || otp_read[3] != 0)
	{
		// Set BIAS_KICK
		constexpr static uint8_t or_mask[] = {0x01};
		constexpr static uint8_t and_mask[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1, 1>(or_mask, and_mask));
	} else
	{
		// TODO find default if 0
	}

	// Fix incomplete bias initialization
	RF_CALL(readRegister<Dw3110::BIAS_CTRL, 1>(bias_ctrl));
	if (otp_read[2] & 0x1F)
	{
		bias_ctrl[0] = (bias_ctrl[0] & 0xE0) | (otp_read[2] & 0x1F);
		RF_CALL(writeRegister<Dw3110::BIAS_CTRL, 1>(bias_ctrl));
	}
	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::OTPAddr Addr>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::readOTPMemory(std::span<uint8_t, 4> out)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	constexpr static uint8_t or_mask_one[] = {0x01};
	constexpr static uint8_t and_mask_one[] = {0xF1};
	RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask_one, and_mask_one));
	RF_CALL(writeRegisterMasked<Dw3110::OTP_ADDR, 2>(Addr.or_mask, Addr.and_mask));
	constexpr static uint8_t or_mask_read[] = {0x02};
	constexpr static uint8_t and_mask_read[] = {0xF2};
	RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask_read, and_mask_read));
	RF_CALL(readRegister<Dw3110::OTP_RDATA>(out));
	constexpr static uint8_t or_mask_zero[] = {0x00};
	constexpr static uint8_t and_mask_zero[] = {0xF0};
	RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask_zero, and_mask_zero));
	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::calibrate()
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	// Save LDO configuration
	RF_CALL(readRegister<Dw3110::LDO_CTRL, 4>(ldo_config));

	// Setup calibration powersupply
	constexpr static uint8_t or_mask_ldo[] = {0x5, 0x1};
	constexpr static uint8_t and_mask_ldo[] = {0xFF, 0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::LDO_CTRL, 2>(or_mask_ldo, and_mask_ldo));

	// Reset calibration done flag
	constexpr static uint8_t one[] = {0x1};
	RF_CALL(writeRegister<Dw3110::RX_CAL_STS, 1>(one));

	// Set calibration modes and set COMP_DLY to 0x2
	constexpr static uint8_t or_mask_1[] = {0x1, 0x0, 0x2, 0x0};
	constexpr static uint8_t and_mask_1[] = {0xED, 0xFF, 0xFD, 0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::RX_CAL, 4>(or_mask_1, and_mask_1));

	// Enable calibration
	constexpr static uint8_t or_mask_2[] = {0x1};
	constexpr static uint8_t and_mask_2[] = {0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::RX_CAL, 1, 1>(or_mask_2, and_mask_2));

	// Wait until calibration is done
	RF_CALL(readRegister<Dw3110::RX_CAL_STS, 1>(rx_cal_sts));
	while ((rx_cal_sts[0] & 0x1) == 0)
	{
		RF_YIELD();
		RF_CALL(readRegister<Dw3110::RX_CAL_STS, 1>(rx_cal_sts));
	}

	RF_CALL(readRegister<Dw3110::RX_CAL_RESI, 4>(rx_cal_res));
	if (rx_cal_res[0] == 0xFF && rx_cal_res[1] == 0xFF && rx_cal_res[2] == 0xFF &&
		(rx_cal_res[3] & 0x1F) == 0x1F)
	{
		RF_RETURN(false);
	}

	RF_CALL(readRegister<Dw3110::RX_CAL_RESQ, 4>(rx_cal_res));
	if (rx_cal_res[0] == 0xFF && rx_cal_res[1] == 0xFF && rx_cal_res[2] == 0xFF &&
		(rx_cal_res[3] & 0x1F) == 0x1F)
	{
		RF_RETURN(false);
	}

	// Restore LDO config
	RF_CALL(writeRegister<Dw3110::LDO_CTRL, 4>(ldo_config));
	this->releaseMaster();
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::isCalibrated()
{
	RF_BEGIN()
	RF_WAIT_UNTIL(this->acquireMaster());
	RF_CALL(readRegister<Dw3110::RX_CAL_STS>(rx_cal_sts));
	if (!(rx_cal_sts[0] & 0x01))
	{
		this->releaseMaster();
		RF_RETURN(false);
	}
	RF_CALL(readRegister<Dw3110::RX_CAL_RESI>(rx_cal_res));
	if (rx_cal_res[0] == 0xFF && rx_cal_res[1] == 0xFF && rx_cal_res[2] == 0xFF &&
		rx_cal_res[3] == 0x1F)
	{
		this->releaseMaster();
		RF_RETURN(false);
	}
	RF_CALL(readRegister<Dw3110::RX_CAL_RESQ>(rx_cal_res));
	if (rx_cal_res[0] == 0xFF && rx_cal_res[1] == 0xFF && rx_cal_res[2] == 0xFF &&
		rx_cal_res[3] == 0x1F)
	{
		this->releaseMaster();
		RF_RETURN(false);
	}
	this->releaseMaster();
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setChannel(Dw3110::Channel channel)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	if (channel == Dw3110::Channel::Channel9)
	{
		// Set DGC_KICK and DGC_SEL appropriately
		constexpr static uint8_t or_mask[] = {0x40, 0x20};
		constexpr static uint8_t and_mask[] = {0xF0, 0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 2>(or_mask, and_mask));
		// TODO check if 0 and load magic instead (see 8.2.4)

		// Magic
		constexpr static uint8_t rf_tx_ctrl_2_magic[] = {0x34, 0x00, 0x01, 0x1c};
		RF_CALL(writeRegister<Dw3110::RF_TX_CTRL_2, 4>(rf_tx_ctrl_2_magic));

		constexpr static uint8_t pll_cfg_magic[] = {0x3c, 0x0f};
		RF_CALL(writeRegister<Dw3110::PLL_CFG, 2>(pll_cfg_magic));
	} else if (channel == Dw3110::Channel::Channel5)
	{
		// Set DGC_KICK and DGC_SEL appropriately
		constexpr static uint8_t or_mask[] = {0x40, 0x00};
		constexpr static uint8_t and_mask[] = {0xF0, 0xDF};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 2>(or_mask, and_mask));
		// TODO check if 0 and load magic instead (see 8.2.4)

		// Magic
		constexpr static uint8_t rf_tx_ctrl_2_magic[] = {0x34, 0x11, 0x07, 0x1c};
		RF_CALL(writeRegister<Dw3110::RF_TX_CTRL_2, 4>(rf_tx_ctrl_2_magic));

		constexpr static uint8_t pll_cfg_magic[] = {0x3c, 0x1f};
		RF_CALL(writeRegister<Dw3110::PLL_CFG, 2>(pll_cfg_magic));
	}

	constexpr static uint8_t pll_cfg_ld_magic[] = {0x81};
	RF_CALL(writeRegister<Dw3110::PLL_CAL, 1>(pll_cfg_ld_magic));

	constexpr static uint8_t cal_enable_or[] = {0x01};
	constexpr static uint8_t cal_enable_and[] = {0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::PLL_CAL, 1, 1>(cal_enable_or, cal_enable_and));
	this->releaseMaster();
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setPreambleCode(Dw3110::PreambleCode rx, Dw3110::PreambleCode tx)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	// Read modify write pcodes into CHAN_CTRL
	RF_CALL(readRegister<Dw3110::CHAN_CTRL, 2>(chan_ctrl));
	chan_ctrl[1] = ((uint8_t)rx & 0x1F) | (chan_ctrl[1] & 0xE0);
	chan_ctrl[0] = (((uint8_t)tx & 0x1F) << 3) | (chan_ctrl[0] & 0x07);
	RF_CALL(writeRegister<Dw3110::CHAN_CTRL, 2>(chan_ctrl));

	// Set RX_TUNE_EN and a magic value in THR_64
	if ((uint8_t)rx > 8)
	{
		constexpr static uint8_t rx_tune_en_or[] = {0x01, 0xE4};
		constexpr static uint8_t rx_tune_en_and[] = {0xFF, 0xE5};
		RF_CALL(writeRegisterMasked<Dw3110::DGC_CFG, 2>(rx_tune_en_or, rx_tune_en_and));
	} else
	{
		constexpr static uint8_t rx_tune_en_or[] = {0x00, 0xE4};
		constexpr static uint8_t rx_tune_en_and[] = {0xFE, 0xE5};
		RF_CALL(writeRegisterMasked<Dw3110::DGC_CFG, 2>(rx_tune_en_or, rx_tune_en_and));
	}

	this->releaseMaster();
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setPreambleLength(Dw3110::PreambleLength plen)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	// Write length to TX_FCTRL
	RF_CALL(readRegister<Dw3110::TX_FCTRL, 6>(tx_info));
	tx_info[1] = (tx_info[1] & 0x0F) | (((uint8_t)plen << 4) & 0xF0);
	RF_CALL(writeRegister<Dw3110::TX_FCTRL, 6>(tx_info));

	// Set Preamble acquisition window to expect packets of the same configuration
	if (plen == Dw3110::PreambleLength::Preamble_32)
	{
		// Set PAC to be 4, also clear DTOB4
		constexpr static uint8_t pac[] = {0x0F};
		RF_CALL(writeRegister<Dw3110::DTUNE0, 1>(pac));
		preamble_len = 32;
		pac_len = 4;
	} else if (plen == Dw3110::PreambleLength::Preamble_64)
	{
		//// Set PAC to be 8, also clear DTOB4
		constexpr static uint8_t pac[] = {0x0C};
		RF_CALL(writeRegister<Dw3110::DTUNE0, 1>(pac));
		preamble_len = 64;
		pac_len = 8;
	} else
	{
		// Set PAC to be 16, also clear DTOB4
		constexpr static uint8_t pac[] = {0x0D};
		RF_CALL(writeRegister<Dw3110::DTUNE0, 1>(pac));
		pac_len = 16;
		if (plen == Dw3110::PreambleLength::Preamble_128)
			preamble_len = 128;
		else if (plen == Dw3110::PreambleLength::Preamble_256)
			preamble_len = 256;
		else if (plen == Dw3110::PreambleLength::Preamble_512)
			preamble_len = 512;
		else if (plen == Dw3110::PreambleLength::Preamble_1024)
			preamble_len = 1024;
		else if (plen == Dw3110::PreambleLength::Preamble_1536)
			preamble_len = 1536;
		else if (plen == Dw3110::PreambleLength::Preamble_2048)
			preamble_len = 2048;
		else if (plen == Dw3110::PreambleLength::Preamble_4096)
			preamble_len = 4096;
	}
	RF_CALL(setRX_SFD_TOC());

	// Load the appropriate OPS values from OTP using OPS_KICK
	if (preamble_len >= 256)
	{
		constexpr static uint8_t otp_val_or[] = {0x04};
		constexpr static uint8_t otp_val_and[] = {0xE7};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1, 1>(otp_val_or, otp_val_and));
	} else
	{
		constexpr static uint8_t otp_val_or[] = {0x14};
		constexpr static uint8_t otp_val_and[] = {0xF7};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1, 1>(otp_val_or, otp_val_and));
	}

	this->releaseMaster();
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setSFD(Dw3110::StartFrameDelimiter sfd)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	RF_CALL(readRegister<Dw3110::CHAN_CTRL, 2>(chan_ctrl));
	chan_ctrl[0] = (chan_ctrl[0] & 0xF9) | (((uint8_t)sfd & 0x03) << 1);
	RF_CALL(writeRegister<Dw3110::CHAN_CTRL, 2>(chan_ctrl));
	if (sfd == Dw3110::StartFrameDelimiter::Decawave_16)
		sfd_len = 16;
	else
		sfd_len = 8;
	RF_CALL(setRX_SFD_TOC());
	this->releaseMaster();
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setRX_SFD_TOC()
{

	RF_BEGIN();
	if (pac_len == 0 || preamble_len == 0 || sfd_len == 0) RF_RETURN();
	RF_WAIT_UNTIL(this->acquireMaster());
	sfd_toc_val = preamble_len + 1 - pac_len + sfd_len;
	rx_sfd_toc[0] = (uint8_t)(sfd_toc_val & 0xFF);
	rx_sfd_toc[1] = (uint8_t)((sfd_toc_val >> 8) & 0xFF);
	RF_CALL(writeRegister<Dw3110::RX_SFD_TOC, 2>(rx_sfd_toc));
	this->releaseMaster();
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
template<size_t Len>
bool
modm::Dw3110Phy<SpiMaster, Cs>::transmitBlocking(const std::span<const uint8_t, Len> payload,
												 size_t payload_len)
{
	return RF_CALL_BLOCKING(transmit(payload, payload_len));
}

template<typename SpiMaster, typename Cs>
template<size_t Len>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::transmit(const std::span<const uint8_t, Len> payload,
										 size_t payload_len)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	RF_CALL(writeRegister<Dw3110::TX_BUFFER>(payload));
	RF_CALL(readRegister<Dw3110::TX_FCTRL, 6>(tx_info));

	tx_info[0] = (uint8_t)(0xFF & payload_len);  // Set Payload length
	tx_info[1] = (tx_info[1] & 0xFC) | ((uint8_t)(payload_len >> 8) & 0x03);

	tx_info[2] = 0;  // Clear TXB_OFFSET
	tx_info[3] &= 0x3;

	RF_CALL(writeRegister<Dw3110::TX_FCTRL, 6>(tx_info));
	RF_CALL(sendCommand<Dw3110::FastCommand::CMD_TX>());
	this->releaseMaster();
	RF_END_RETURN(true);
}
template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::startReceive()
{
	return sendCommand<Dw3110::FastCommand::CMD_RX>();
}

template<typename SpiMaster, typename Cs>
template<size_t Len>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::fetchPacket(std::span<uint8_t, Len> payload, size_t& payload_len)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	this->releaseMaster();
	RF_END_RETURN(false);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::packetReady()
{

	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());
	this->releaseMaster();
	RF_END_RETURN(false);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::fetchSystemStatus()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::SYS_STATUS, 6>(sys_status));
	system_status = (uint64_t)sys_status[0] | ((uint64_t)sys_status[1] << 8) |
					((uint64_t)sys_status[2] << 16) | ((uint64_t)sys_status[3] << 24) |
					((uint64_t)sys_status[4] << 32) | ((uint64_t)sys_status[5] << 40);
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::fetchChipState()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::SYS_STATE, 4>(sys_state));
	if (sys_state[3] == 0x00)
	{
		chip_state = Dw3110::SystemState::WAKEUP;
	} else if (sys_state[3] <= 0x02)
	{
		chip_state = Dw3110::SystemState::IDLE_RC;
	} else if (sys_state[3] == 0x03)
	{
		chip_state = Dw3110::SystemState::IDLE_PLL;
	} else if (sys_state[3] >= 0x08 && sys_state[3] <= 0x0F)
	{
		chip_state = Dw3110::SystemState::TX;
	} else if (sys_state[3] >= 0x12 && sys_state[3] <= 0x19)
	{
		chip_state = Dw3110::SystemState::RX;
	} else
	{
		chip_state = Dw3110::SystemState::INVALID;
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::FastCommand Cmd>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::sendCommand()
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] = 0x80 | (uint8_t)Cmd;

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 1));
	Cs::setOutput(true);

	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::Register Reg, size_t Len, size_t Offset>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::readRegister(std::span<uint8_t, Len> out)
{
	static_assert(Len <= Reg.length + Offset, "Size of read is too large for this register!");
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] =
		(uint8_t)(0x40 | ((Reg.bank.addr << 1) & 0x3E) | (((Reg.offset + Offset) >> 6) & 0x01));
	tx_buffer[1] = (uint8_t)(0x00 | ((Reg.offset + Offset) << 2));

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 2));
	RF_CALL(SpiMaster::transfer(nullptr, out.data(), out.size()));
	Cs::setOutput(true);

	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::Register Reg, size_t Len, size_t Offset>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::writeRegister(const std::span<const uint8_t, Len> val)
{
	static_assert(Len + Offset <= Reg.length, "Size of write is too large for this register!");

	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] =
		(uint8_t)(0xC0 | ((Reg.bank.addr << 1) & 0x3E) | (((Reg.offset + Offset) >> 6) & 0x01));
	tx_buffer[1] = (uint8_t)(0x00 | ((Reg.offset + Offset) << 2));

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 2));
	RF_CALL(SpiMaster::transfer(val.data(), nullptr, Len));
	Cs::setOutput(true);

	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::Register Reg, size_t Len, size_t Offset>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::writeRegisterMasked(const std::span<const uint8_t, Len> or_mask,
													const std::span<const uint8_t, Len> and_mask)
{
	static_assert(Len + Offset <= Reg.length,
				  "Size of masked write is too large for this register!");
	static_assert(Len == 1 || Len == 2 || Len == 4,
				  "Masked writes only support sizes of 1,2 or 4 Bytes.");

	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] =
		(uint8_t)(0xC0 | ((Reg.bank.addr << 1) & 0x3E) | (((Reg.offset + Offset) >> 6) & 0x01));
	tx_buffer[1] = (uint8_t)(0x00 | ((Reg.offset + Offset) << 2));
	if constexpr (Len == 1)
		tx_buffer[1] |= 1;
	else if constexpr (Len == 2)
		tx_buffer[1] |= 2;
	else
		tx_buffer[1] |= 3;

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 2));
	RF_CALL(SpiMaster::transfer(and_mask.data(), nullptr, Len));
	RF_CALL(SpiMaster::transfer(or_mask.data(), nullptr, Len));
	Cs::setOutput(true);

	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::RegisterBank Reg, size_t Len>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::readRegisterBank(std::span<uint8_t, Len> out)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] = (uint8_t)((Reg.addr << 1) & 0x3E);

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 1));
	RF_CALL(SpiMaster::transfer(nullptr, out.data(), Len));
	Cs::setOutput(true);
	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::RegisterBank Reg, size_t Len>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::writeRegisterBank(const std::span<const uint8_t, Len> val)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] = (uint8_t)(0x80 | ((Reg.addr << 1) & 0x3E));

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 1));
	RF_CALL(SpiMaster::transfer(val.data(), nullptr, Len));
	Cs::setOutput(true);
	this->releaseMaster();
	RF_END();
}
