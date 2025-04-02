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

#include <algorithm>
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

	RF_CALL(fetchChipState());
	if (chip_state == Dw3110::SystemState::TX || chip_state == Dw3110::SystemState::RX ||
		chip_state == Dw3110::SystemState::TX_WAIT || chip_state == Dw3110::SystemState::RX_WAIT)
	{
		RF_CALL(sendCommand<Dw3110::FastCommand::CMD_TXRXOFF>());
	}

	// Loop until valid state
	timeout.restart(1ms);
	while (chip_state != Dw3110::SystemState::IDLE_RC &&
		   chip_state != Dw3110::SystemState::IDLE_PLL)
	{
		if (timeout.execute())
		{
			MODM_LOG_ERROR << "Timeout waiting for IDLE State!" << (int)chip_state << modm::endl;
			RF_RETURN(false);
		}
		RF_YIELD();
		RF_CALL(fetchChipState());
	}

	if (!RF_CALL(checkDevID())) { RF_RETURN(false); }

	// Initialize factory programmed defaults
	RF_CALL(loadOTP());

	// Load magic constants mentioned in various places in the manual
	constexpr static uint8_t rf_tx_ctrl_1_magic[] = {0x0E};
	RF_CALL(writeRegister<Dw3110::RF_TX_CTRL_1, 1>(rf_tx_ctrl_1_magic));

	constexpr static uint8_t ldo_rload_magic[] = {0x14};
	RF_CALL(writeRegister<Dw3110::LDO_RLOAD, 1>(ldo_rload_magic));

	constexpr static uint8_t res_b0_magic[] = {0x9B};
	RF_CALL(writeRegister<Dw3110::STS_CONF_1, 1>(res_b0_magic));

	// Magic value from the user manual (8.2.7.4)
	// constexpr static uint8_t dtune3_magic[] = {0xCC, 0x35, 0x5F, 0xAF};
	// RF_CALL(writeRegister<Dw3110::DTUNE3, 4>(dtune3_magic));
	// TODO maybe only when sending packets with no payload,
	// investigate (github.com/egnor/DW3000_notes.md))

	// Change to IDLE_PLL by setting SEQ_CTRL:AINIT2IDLE
	constexpr static uint8_t and_mask[] = {0xFF};
	constexpr static uint8_t or_mask[] = {0x01};
	RF_CALL(writeRegisterMasked<Dw3110::SEQ_CTRL, 1, 1>(or_mask, and_mask));

	// Loop until we are in IDLE_PLL
	timeout.restart(1ms);
	RF_CALL(fetchChipState());
	while (chip_state != Dw3110::SystemState::IDLE_PLL)
	{
		RF_YIELD();
		if (timeout.execute())
		{
			MODM_LOG_ERROR << "Failed to reach IDLE_PLL State!" << modm::endl;
			RF_RETURN(false);
		}
		fetchChipState();
	}

	if (!RF_CALL(calibrate())) { RF_RETURN(false); }
	RF_CALL(setChannel(channel));
	RF_CALL(setPreambleCode(pcode, pcode));
	RF_CALL(setPreambleLength(plen));
	RF_CALL(setSFD(sfd));
	RF_CALL(setEnableLongFrames(false));
	RF_CALL(setSendHeaderFast(false));
	RF_CALL(setCCATimeout(256));
	RF_CALL(reloadSTSIV());
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::checkDevID()
{
	RF_BEGIN();

	// Check we actually read registers correctly by checking for the device type
	constexpr static uint8_t DEV_ID_MATCH[] = {0x03, 0xCA, 0xDE};
	RF_CALL(readRegister<Dw3110::DEV_ID, 4, 0>(std::span<uint8_t>(scratch).first<4>()));

	if (!std::ranges::equal(DEV_ID_MATCH, std::span<uint8_t>(scratch).subspan<1, 3>()))
	{
		MODM_LOG_ERROR << "Device did not return valid Dw3000 device ID!" << modm::endl;
		RF_RETURN(false);
	}
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setEnableLongFrames(bool value)
{
	RF_BEGIN()
	long_frames = value;
	if (value)
	{
		constexpr static uint8_t or_mask_true[] = {0x10};
		constexpr static uint8_t and_mask_true[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1>(or_mask_true, and_mask_true));
	} else
	{
		constexpr static uint8_t or_mask_false[] = {0x00};
		constexpr static uint8_t and_mask_false[] = {0xEF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1>(or_mask_false, and_mask_false));
	}
	RF_END();
}

// TODO Write timeout to RX_FWTO
template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setReceiveWaitTimeout(modm::chrono::micro_clock::duration duration)
{
	RF_BEGIN()
	if (duration.count() == 0)
	{
		constexpr static uint8_t or_mask[] = {0x00};
		constexpr static uint8_t and_mask[] = {0xFD};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(or_mask, and_mask));
	} else
	{
		scratch[0] = (duration.count() >> 0) & 0xFF;
		scratch[1] = (duration.count() >> 8) & 0xFF;
		scratch[2] = (duration.count() >> 16) & 0xFF;
		RF_CALL(writeRegister<Dw3110::RX_FWTO, 3>(std::span<uint8_t>(scratch).first<3>()));

		constexpr static uint8_t or_mask[] = {0x02};
		constexpr static uint8_t and_mask[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(or_mask, and_mask));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setSendHeaderFast(bool value)
{
	RF_BEGIN()
	if (value)
	{
		constexpr static uint8_t or_mask_true[] = {0x20};
		constexpr static uint8_t and_mask_true[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1>(or_mask_true, and_mask_true));
	} else
	{
		constexpr static uint8_t or_mask_false[] = {0x00};
		constexpr static uint8_t and_mask_false[] = {0xDF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1>(or_mask_false, and_mask_false));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setReenableOnRxFailure(bool value)
{
	RF_BEGIN()
	if (value)
	{
		constexpr static uint8_t or_mask_true[] = {0x04};
		constexpr static uint8_t and_mask_true[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(or_mask_true, and_mask_true));
	} else
	{
		constexpr static uint8_t or_mask_false[] = {0x00};
		constexpr static uint8_t and_mask_false[] = {0xFB};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(or_mask_false, and_mask_false));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setCCATimeout(uint16_t timeout)
{
	RF_BEGIN();
	scratch[0] = (timeout >> 0) & 0xFF;
	scratch[1] = (timeout >> 8) & 0xFF;
	RF_CALL(writeRegister<Dw3110::PRE_TOC, 2>(std::span<const uint8_t>(scratch).first<2>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setEnableFastTurnaround(bool value)
{
	RF_BEGIN()
	if (value)
	{
		constexpr static uint8_t or_mask_true[] = {0x04};
		constexpr static uint8_t and_mask_true[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 2>(or_mask_true, and_mask_true));
	} else
	{
		constexpr static uint8_t or_mask_false[] = {0x00};
		constexpr static uint8_t and_mask_false[] = {0xFB};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 2>(or_mask_false, and_mask_false));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint32_t>
modm::Dw3110Phy<SpiMaster, Cs>::readChipTime()
{
	RF_BEGIN();
	const static uint8_t zero[] = {0x0, 0x0, 0x0, 0x0};
	RF_CALL(writeRegister<Dw3110::SYS_TIME, 4>(zero));
	RF_CALL(readRegister<Dw3110::SYS_TIME, 4>(std::span<uint8_t>(scratch).first<4>()));
	RF_END_RETURN((uint32_t)scratch[0] | ((uint32_t)scratch[1] << 8) |
				  ((uint32_t)scratch[2] << 16) | ((uint32_t)scratch[3] << 24));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint64_t>
modm::Dw3110Phy<SpiMaster, Cs>::getReceiveTimestamp()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::RX_TIME, 5>(std::span<uint8_t>(scratch).first<5>()));
	RF_END_RETURN((uint64_t)scratch[0] | ((uint64_t)scratch[1] << 8) |
				  ((uint64_t)scratch[2] << 16) | ((uint64_t)scratch[3] << 24) |
				  ((uint64_t)scratch[4] << 32));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint64_t>
modm::Dw3110Phy<SpiMaster, Cs>::getTransmitTimestamp()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::TX_TIME, 5>(std::span<uint8_t>(scratch).first<5>()));
	RF_END_RETURN((uint64_t)scratch[0] | ((uint64_t)scratch[1] << 8) |
				  ((uint64_t)scratch[2] << 16) | ((uint64_t)scratch[3] << 24) |
				  ((uint64_t)scratch[4] << 32));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setAcknowledgeTurnaround(uint8_t time)
{
	RF_BEGIN();
	scratch[0] = time;
	RF_CALL(writeRegister<Dw3110::ACK_RESP_T, 1, 3>(std::span<const uint8_t>(scratch).first<1>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setWaitForResponseTime(modm::PreciseClock::duration time)
{
	RF_BEGIN();
	scratch[0] = time.count() & 0xFF;
	scratch[1] = (time.count() >> 8) & 0xFF;
	scratch[2] = (time.count() >> 16) & 0x0F;
	RF_CALL(writeRegister<Dw3110::ACK_RESP_T, 3>(std::span<const uint8_t>(scratch).first<3>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<modm::Dw3110::SystemStatus_t>
modm::Dw3110Phy<SpiMaster, Cs>::getStatus()
{
	RF_BEGIN();
	RF_CALL(fetchSystemStatus());
	RF_END_RETURN(system_status);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::clearStatusBits(Dw3110::SystemStatus_t mask)
{
	RF_BEGIN();
	scratch[0] = (mask.value >> 0) & 0xFF;
	scratch[1] = (mask.value >> 8) & 0xFF;
	scratch[2] = (mask.value >> 16) & 0xFF;
	scratch[3] = (mask.value >> 24) & 0xFF;
	scratch[4] = (mask.value >> 32) & 0xFF;
	scratch[5] = (mask.value >> 40) & 0xFF;
	RF_CALL(writeRegister<Dw3110::SYS_STATUS, 6>(std::span<const uint8_t>(scratch).first<6>()));
	RF_END();
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
	// Check LDO and kick if not 0
	RF_CALL(readOTPMemory<Dw3110::LDOTUNE_CAL_1>(otp_read));
	if (otp_read[0] == 0 && otp_read[1] == 0 && otp_read[2] == 0 && otp_read[3] == 0)
	{
		RF_CALL(readOTPMemory<Dw3110::LDOTUNE_CAL_2>(otp_read));
	}
	if (otp_read[0] != 0 || otp_read[1] != 0 || otp_read[2] != 0 ||
		otp_read[3] != 0)  // TODO find default if 0
	{
		// Set LDO_KICK
		constexpr static uint8_t or_mask[] = {0xC0};
		constexpr static uint8_t and_mask[] = {0xF0};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask, and_mask));
	}

	// Load from OTP according to (github.com/egnor/DW3000_notes.md))
	// XTAL_TRIM = 0x13 |  Len = 2, need bits 0-5 -> XTAL:0-5
	RF_CALL(readOTPMemory<Dw3110::XTAL_TRIM>(otp_read));
	RF_CALL(readRegister<Dw3110::XTAL, 1>(xtal));
	if (xtal[0] & 0x1F)  // TODO find default if 0
	{
		xtal[0] = (xtal[0] & 0xE0) | (otp_read[0] & 0x1F);
		RF_CALL(writeRegister<Dw3110::XTAL, 1>(xtal));
	}

	// BIASTUNE_CAL = 0x0A | Len = ?? need bits 16-20 -> BIAS_CTRL:0-5
	RF_CALL(readOTPMemory<Dw3110::BIASTUNE_CAL>(otp_read));
	if (otp_read[0] != 0 || otp_read[1] != 0 || otp_read[2] != 0 ||
		otp_read[3] != 0)  // TODO find default if 0
	{
		// Set BIAS_KICK
		constexpr static uint8_t or_mask[] = {0x01};
		constexpr static uint8_t and_mask[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1, 1>(or_mask, and_mask));
	}

	// Fix incomplete bias initialization
	RF_CALL(readRegister<Dw3110::BIAS_CTRL, 1>(bias_ctrl));
	if (otp_read[2] & 0x1F)
	{
		bias_ctrl[0] = (bias_ctrl[0] & 0xE0) | (otp_read[2] & 0x1F);
		RF_CALL(writeRegister<Dw3110::BIAS_CTRL, 1>(bias_ctrl));
	}  // TODO find default if 0

	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::OTPAddr Addr>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::readOTPMemory(std::span<uint8_t, 4> out)
{
	RF_BEGIN();
	constexpr static uint8_t or_mask_one[] = {0x01};
	constexpr static uint8_t and_mask_one[] = {0xF1};
	RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask_one, and_mask_one));
	RF_CALL(writeRegisterMasked<Dw3110::OTP_ADDR, 2>(Addr.or_mask, Addr.and_mask));
	constexpr static uint8_t or_mask_read_2[] = {0x02};
	constexpr static uint8_t and_mask_read_2[] = {0xF2};
	RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask_read_2, and_mask_read_2));
	RF_CALL(readRegister<Dw3110::OTP_RDATA>(out));
	constexpr static uint8_t or_mask_zero[] = {0x00};
	constexpr static uint8_t and_mask_zero[] = {0xF0};
	RF_CALL(writeRegisterMasked<Dw3110::OTP_CFG, 1>(or_mask_zero, and_mask_zero));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::calibrate()
{
	RF_BEGIN();

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
	constexpr static uint8_t rx_cal_1[] = {0x1, 0x0, 0x2, 0x0};
	RF_CALL(writeRegister<Dw3110::RX_CAL, 4>(rx_cal_1));

	// Clear result registers
	constexpr static uint8_t zeros[] = {0, 0, 0, 0};
	RF_CALL(writeRegister<Dw3110::RX_CAL_RESI, 4>(zeros));
	RF_CALL(writeRegister<Dw3110::RX_CAL_RESQ, 4>(zeros));

	// Enable calibration
	constexpr static uint8_t cal_enable_or[] = {0x10};
	constexpr static uint8_t cal_enable_and[] = {0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::RX_CAL, 1>(cal_enable_or, cal_enable_and));

	// Wait until calibration is done
	timeout.restart(10ms);
	RF_CALL(readRegister<Dw3110::RX_CAL_STS, 1>(rx_cal_sts));
	while (rx_cal_sts[0] == 0)
	{
		RF_YIELD();
		if (timeout.execute()) { RF_RETURN(false); }
		RF_CALL(readRegister<Dw3110::RX_CAL_STS, 1>(rx_cal_sts));
	}

	// Setting COMP_DLY Bit 0
	constexpr static uint8_t comp_dly_1_or[] = {0x1};
	constexpr static uint8_t comp_dly_1_and[] = {0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::RX_CAL, 1, 2>(comp_dly_1_or, comp_dly_1_and));

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

	// Reset RX_CAL
	constexpr static uint8_t reset_val[] = {0x0, 0x0, 0x2, 0x0};
	RF_CALL(writeRegister<Dw3110::RX_CAL, 4>(reset_val));

	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setChannel(Dw3110::Channel channel)
{
	RF_BEGIN();

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

		// TODO Set RF_RX_CTRL_HI to 0x08B5A833 for ch9

		// Actually set Channel
		constexpr static uint8_t chan_ctrl_or[] = {0x01};
		constexpr static uint8_t chan_ctrl_and[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::CHAN_CTRL, 1>(chan_ctrl_or, chan_ctrl_and));
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

		// TODO Find default value of RF_RX_CTRL_HI for setting to ch5

		// Actually set Channel
		constexpr static uint8_t chan_ctrl_or[] = {0x00};
		constexpr static uint8_t chan_ctrl_and[] = {0xFE};
		RF_CALL(writeRegisterMasked<Dw3110::CHAN_CTRL, 1>(chan_ctrl_or, chan_ctrl_and));
	}

	constexpr static uint8_t pll_cfg_ld_magic[] = {0x81};
	RF_CALL(writeRegister<Dw3110::PLL_CAL, 1>(pll_cfg_ld_magic));

	constexpr static uint8_t cal_enable_or[] = {0x01};
	constexpr static uint8_t cal_enable_and[] = {0xFF};
	RF_CALL(writeRegisterMasked<Dw3110::PLL_CAL, 1, 1>(cal_enable_or, cal_enable_and));
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setPreambleCode(Dw3110::PreambleCode rx, Dw3110::PreambleCode tx)
{
	RF_BEGIN();

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
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setPreambleLength(Dw3110::PreambleLength plen)
{
	RF_BEGIN();

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

	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setSFD(Dw3110::StartFrameDelimiter sfd)
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::CHAN_CTRL, 2>(chan_ctrl));
	chan_ctrl[0] = (chan_ctrl[0] & 0xF9) | (((uint8_t)sfd & 0x03) << 1);
	RF_CALL(writeRegister<Dw3110::CHAN_CTRL, 2>(chan_ctrl));
	if (sfd == Dw3110::StartFrameDelimiter::Decawave_16)
		sfd_len = 16;
	else
		sfd_len = 8;
	RF_CALL(setRX_SFD_TOC());
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setRX_SFD_TOC()
{
	RF_BEGIN();
	if (pac_len == 0 || preamble_len == 0 || sfd_len == 0) RF_RETURN();

	sfd_toc_val = preamble_len + 1 - pac_len + sfd_len;
	rx_sfd_toc[0] = (uint8_t)(sfd_toc_val & 0xFF);
	rx_sfd_toc[1] = (uint8_t)((sfd_toc_val >> 8) & 0xFF);
	RF_CALL(writeRegister<Dw3110::RX_SFD_TOC, 2>(rx_sfd_toc));
	RF_END_RETURN();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint64_t>
modm::Dw3110Phy<SpiMaster, Cs>::getDeviceUID()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::EUI_64, 8>(std::span<uint8_t>(scratch).first<8>()));
	RF_END_RETURN((uint64_t)scratch[0] | ((uint64_t)scratch[1] << 8) |
				  ((uint64_t)scratch[2] << 16) | ((uint64_t)scratch[3] << 24) |
				  ((uint64_t)scratch[4] << 32) | ((uint64_t)scratch[5] << 40) |
				  ((uint64_t)scratch[6] << 48) | ((uint64_t)scratch[7] << 56));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setDeviceUID(uint64_t uid)
{
	RF_BEGIN();
	scratch[0] = (uid >> 0) & 0xFF;
	scratch[1] = (uid >> 8) & 0xFF;
	scratch[2] = (uid >> 16) & 0xFF;
	scratch[3] = (uid >> 24) & 0xFF;
	scratch[4] = (uid >> 32) & 0xFF;
	scratch[5] = (uid >> 40) & 0xFF;
	scratch[6] = (uid >> 48) & 0xFF;
	scratch[7] = (uid >> 56) & 0xFF;
	RF_CALL(writeRegister<Dw3110::EUI_64, 8>(std::span<const uint8_t>(scratch).first<8>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint16_t>
modm::Dw3110Phy<SpiMaster, Cs>::getPanUID()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::PANADR, 2, 2>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN((uint64_t)scratch[0] | ((uint64_t)scratch[1] << 8));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint16_t>
modm::Dw3110Phy<SpiMaster, Cs>::getShortUID()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::PANADR, 2, 0>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN((uint64_t)scratch[0] | ((uint64_t)scratch[1] << 8));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setPanUID(uint16_t pid)
{
	RF_BEGIN();
	scratch[0] = (pid >> 0) & 0xFF;
	scratch[1] = (pid >> 8) & 0xFF;
	RF_CALL(writeRegister<Dw3110::PANADR, 2, 2>(std::span<const uint8_t>(scratch).first<2>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setShortUID(uint16_t sid)
{
	RF_BEGIN();
	scratch[0] = (sid >> 0) & 0xFF;
	scratch[1] = (sid >> 8) & 0xFF;
	RF_CALL(writeRegister<Dw3110::PANADR, 2, 0>(std::span<const uint8_t>(scratch).first<2>()));
	RF_END();
}

// Enable or disable the frame filtering
template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setFrameFilterEnabled(bool value)
{
	RF_BEGIN();
	if (value)
	{
		constexpr static uint8_t FFEN_or[] = {0x01};
		constexpr static uint8_t FFEN_and[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 0>(FFEN_or, FFEN_and));
	} else
	{
		constexpr static uint8_t FFEN_or[] = {0x00};
		constexpr static uint8_t FFEN_and[] = {0xFE};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 0>(FFEN_or, FFEN_and));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<modm::Dw3110::FilterConfig_t>
modm::Dw3110Phy<SpiMaster, Cs>::getFilterConfig()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::FF_CFG, 2, 0>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN(Dw3110::FilterConfig_t(scratch[0] | scratch[1] << 8));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setFilterConfig(Dw3110::FilterConfig_t fc)
{
	RF_BEGIN();
	scratch[0] = (fc.value & 0xFF) >> 0;
	scratch[1] = (fc.value & 0xFF) >> 8;
	RF_CALL(writeRegister<Dw3110::FF_CFG, 2, 0>(std::span<const uint8_t>(scratch).first<2>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint16_t>
modm::Dw3110Phy<SpiMaster, Cs>::getRXAntennaDelay()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::CIA_CONF, 2, 0>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN(uint16_t(scratch[0] << 0 | scratch[1] << 8));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint16_t>
modm::Dw3110Phy<SpiMaster, Cs>::getTXAntennaDelay()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::TX_ANTD, 2, 0>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN(uint16_t(scratch[0] << 0 | scratch[1] << 8));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<float>
modm::Dw3110Phy<SpiMaster, Cs>::getReceiverClockOffset()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::CIA_DIAG_0, 2>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN(float(scratch[0] << 0 | (scratch[1] & 0x1F) << 8) / 67108864.0f * 1000000.0f);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setAntennaDelay(uint16_t delay)
{
	RF_BEGIN();
	scratch[0] = (delay >> 0) & 0xFF;
	scratch[1] = (delay >> 8) & 0xFF;
	RF_CALL(writeRegister<Dw3110::CIA_CONF, 2, 0>(std::span<const uint8_t>(scratch).first<2>()));
	RF_CALL(writeRegister<Dw3110::TX_ANTD, 2, 0>(std::span<const uint8_t>(scratch).first<2>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setAutoAckEnabled(bool value)
{
	RF_BEGIN();
	if (value)
	{
		constexpr static uint8_t AUTO_ACK_or[] = {0x08};
		constexpr static uint8_t AUTO_ACK_and[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(AUTO_ACK_or, AUTO_ACK_and));
	} else
	{
		constexpr static uint8_t AUTO_ACK_or[] = {0x00};
		constexpr static uint8_t AUTO_ACK_and[] = {0xF7};
		RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(AUTO_ACK_or, AUTO_ACK_and));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110Phy<SpiMaster, Cs>::TXMode mode>
consteval modm::Dw3110::FastCommand
modm::Dw3110Phy<SpiMaster, Cs>::txModeToCmd()
{
	static_assert(mode == TXMode::Default || mode == TXMode::DefaultAndReceive ||
					  mode == TXMode::Force || mode == TXMode::ForceAndReceive,
				  "Unknown TXMode in txModeToCmd!");
	if constexpr (mode == TXMode::Default)
	{
		return modm::Dw3110::FastCommand::CMD_CCA_TX;
	} else if constexpr (mode == TXMode::DefaultAndReceive)
	{
		return modm::Dw3110::FastCommand::CMD_CCA_TX_W4R;
	} else if constexpr (mode == TXMode::Force)
	{
		return modm::Dw3110::FastCommand::CMD_TX;
	} else if constexpr (mode == TXMode::ForceAndReceive)
	{
		return modm::Dw3110::FastCommand::CMD_TX_W4R;
	}
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110Phy<SpiMaster, Cs>::DelayTXMode dmode>
consteval modm::Dw3110::FastCommand
modm::Dw3110Phy<SpiMaster, Cs>::txModeToCmdDelay()
{
	static_assert(dmode == DelayTXMode::AtTime || dmode == DelayTXMode::DelayWRTRX ||
					  dmode == DelayTXMode::DelayWRTTX || dmode == DelayTXMode::DelayWRTRef ||
					  dmode == DelayTXMode::AtTimeAndReceive ||
					  dmode == DelayTXMode::DelayWRTRXAndReceive ||
					  dmode == DelayTXMode::DelayWRTTXAndReceive ||
					  dmode == DelayTXMode::DelayWRTRefAndReceive,
				  "Unknown DelayTXMode in txModeToCmdDelay!");
	if constexpr (dmode == DelayTXMode::AtTime)
	{
		return modm::Dw3110::FastCommand::CMD_DTX;
	} else if constexpr (dmode == DelayTXMode::DelayWRTRX)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_RS;
	} else if constexpr (dmode == DelayTXMode::DelayWRTTX)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_TS;
	} else if constexpr (dmode == DelayTXMode::DelayWRTRef)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_REF;
	} else if constexpr (dmode == DelayTXMode::AtTimeAndReceive)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_W4R;
	} else if constexpr (dmode == DelayTXMode::DelayWRTRXAndReceive)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_RS_W4R;
	} else if constexpr (dmode == DelayTXMode::DelayWRTTXAndReceive)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_TS_W4R;
	} else if constexpr (dmode == DelayTXMode::DelayWRTRefAndReceive)
	{
		return modm::Dw3110::FastCommand::CMD_DTX_REF_W4R;
	}
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110Phy<SpiMaster, Cs>::DelayTXMode dmode>
modm::ResumableResult<typename modm::Dw3110Phy<SpiMaster, Cs>::Error>
modm::Dw3110Phy<SpiMaster, Cs>::transmitDelayed(uint32_t time, std::span<const uint8_t> payload,
												bool ranging, bool fast)
{
	RF_BEGIN();
	scratch[0] = (time >> 0) & 0xFF;
	scratch[1] = (time >> 8) & 0xFF;
	scratch[2] = (time >> 16) & 0xFF;
	scratch[3] = (time >> 24) & 0xFF;
	RF_CALL(writeRegister<Dw3110::DX_TIME, 4>(std::span<const uint8_t>(scratch).first<4>()));
	RF_END_RETURN_CALL(transmitGeneric<txModeToCmdDelay<dmode>()>(payload, ranging, fast));
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110Phy<SpiMaster, Cs>::TXMode tmode>
modm::ResumableResult<typename modm::Dw3110Phy<SpiMaster, Cs>::Error>
modm::Dw3110Phy<SpiMaster, Cs>::transmit(std::span<const uint8_t> payload, bool ranging, bool fast)
{
	return transmitGeneric<txModeToCmd<tmode>()>(payload, ranging, fast);
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::FastCommand Cmd>
modm::ResumableResult<typename modm::Dw3110Phy<SpiMaster, Cs>::Error>
modm::Dw3110Phy<SpiMaster, Cs>::transmitGeneric(std::span<const uint8_t> payload, bool ranging,
												bool fast)
{
	RF_BEGIN();
	if ((long_frames && payload.size() > 1021) || (!long_frames && payload.size() > 125))
	{
		MODM_LOG_ERROR << "Payload is too long to transmit!" << modm::endl;
		RF_RETURN(Error::PayloadTooLarge);
	}

	// Go idle
	RF_CALL(sendCommand<Dw3110::FastCommand::CMD_TXRXOFF>());

	// Write payload to buffer
	RF_CALL(writeRegisterBank<Dw3110::TX_BUFFER_BANK>(payload, payload.size()));

	RF_CALL(readRegister<Dw3110::TX_FCTRL, 6>(tx_info));
	tx_info[0] = (uint8_t)(0xFF & (payload.size() + fcs_len));  // Set Payload length
	tx_info[1] = (tx_info[1] & 0xF0) | ((uint8_t)((payload.size() + fcs_len) >> 8) & 0x03);

	if (fast) tx_info[1] |= 0x04;     // Set TXBR if fast is selected
	if (ranging) tx_info[1] |= 0x08;  // Set TXR if ranging is selected

	tx_info[2] = 0;  // Clear TXB_OFFSET
	tx_info[3] &= 0x3;

	RF_CALL(writeRegister<Dw3110::TX_FCTRL, 6>(tx_info));
	RF_CALL(sendCommand<Cmd>());

	RF_CALL(fetchSystemStatus());
	RF_CALL(fetchChipState());
	while (system_status.none(Dw3110::SystemStatus::TXFRS))
	{
		last_err = RF_CALL(checkTXFailed());
		if (last_err != Error::None)
		{
			RF_CALL(sendCommand<Dw3110::FastCommand::CMD_TXRXOFF>());
			RF_CALL(
				clearStatusBits(Dw3110::SystemStatus::CCA_FAIL | Dw3110::SystemStatus::HPDWARN));
			RF_RETURN(last_err);
		}

		RF_YIELD();
		RF_CALL(fetchSystemStatus());
		RF_CALL(fetchChipState());
	}

	// Clear TX related flags
	RF_CALL(clearStatusBits(Dw3110::SystemStatus::TXFRS | Dw3110::SystemStatus::TXFRB |
							Dw3110::SystemStatus::TXPHS | Dw3110::SystemStatus::TXPRS |
							Dw3110::SystemStatus::CCA_FAIL | Dw3110::SystemStatus::HPDWARN));
	RF_END_RETURN(Error::None);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<typename modm::Dw3110Phy<SpiMaster, Cs>::Error>
modm::Dw3110Phy<SpiMaster, Cs>::checkTXFailed()
{
	RF_BEGIN();
	if (system_status.any(Dw3110::SystemStatus::CCA_FAIL)) { RF_RETURN(Error::ChannelBusy); }

	if (system_status.any(Dw3110::SystemStatus::HPDWARN)) { RF_RETURN(Error::DelayTooShort); }

	if (sys_state[0] == 0x0 && sys_state[1] == 0x0 && sys_state[2] == 0x0D && sys_state[3] == 0x0)
	{
		RF_CALL(fetchChipState());
		if (sys_state[0] == 0x0 && sys_state[1] == 0x0 && sys_state[2] == 0x0D &&
			sys_state[3] == 0x0)
		{
			RF_RETURN(Error::DelayTooShort);
		}
	}
	RF_END_RETURN(Error::None);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setReferenceTime(uint32_t time)
{
	RF_BEGIN();
	scratch[0] = (time >> 0) & 0xFF;
	scratch[1] = (time >> 8) & 0xFF;
	scratch[2] = (time >> 16) & 0xFF;
	scratch[3] = (time >> 24) & 0xFF;
	RF_CALL(writeRegister<Dw3110::DREF_TIME, 4>(std::span<const uint8_t>(scratch).first<4>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::startReceive()
{
	RF_BEGIN();
	if (RF_CALL(this->packetReady())) RF_RETURN(true);

	RF_CALL(fetchChipState());
	if (chip_state == Dw3110::SystemState::RX || chip_state == Dw3110::SystemState::RX_WAIT)
	{
		RF_RETURN(true);
	}

	// Go idle
	RF_CALL(sendCommand<Dw3110::FastCommand::CMD_TXRXOFF>());

	// Clear RXFR RXPHE RXFCG and RXFCE flag
	RF_CALL(clearStatusBits(Dw3110::SystemStatus::RXFR | Dw3110::SystemStatus::RXPHE |
							Dw3110::SystemStatus::RXFCG | Dw3110::SystemStatus::RXFCE));

	RF_CALL(sendCommand<Dw3110::FastCommand::CMD_RX>());
	RF_CALL(fetchChipState());
	timeout.restart(10ms);
	while (chip_state != Dw3110::SystemState::RX && chip_state != Dw3110::SystemState::RX_WAIT)
	{
		RF_YIELD();
		RF_CALL(fetchChipState());
		if (timeout.execute()) { RF_RETURN(false); }
	}

	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::fetchPacket(std::span<uint8_t> payload, size_t &payload_len)
{
	RF_BEGIN();

	RF_CALL(readRegister<Dw3110::RX_FINFO, 4>(rx_finfo));
	payload_len = (rx_finfo[0] | ((rx_finfo[1] & 0x03) << 8));
	if (payload_len >= fcs_len) payload_len -= fcs_len;
	if (payload.size() < payload_len) { RF_RETURN(false); }
	RF_CALL(readRegisterBank<Dw3110::RX_BUFFER_0_BANK>(payload, payload_len));

	// Clear most rx flags
	RF_CALL(clearStatusBits(Dw3110::SystemStatus::RXFR | Dw3110::SystemStatus::RXPHE |
							Dw3110::SystemStatus::RXFCG | Dw3110::SystemStatus::RXFCE |
							Dw3110::SystemStatus::RXSFDD | Dw3110::SystemStatus::RXPRD |
							Dw3110::SystemStatus::RXPHD | Dw3110::SystemStatus::RXFSL));
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::packetReady()
{
	RF_BEGIN();
	RF_CALL(fetchSystemStatus());
	RF_END_RETURN(system_status.all(Dw3110::SystemStatus::RXFR | Dw3110::SystemStatus::RXFCG));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::isReceiving()
{
	RF_BEGIN();
	RF_CALL(fetchChipState());
	RF_END_RETURN(chip_state == Dw3110::SystemState::RX);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::fetchSystemStatus()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::SYS_STATUS, 6>(sys_status));
	system_status =
		Dw3110::SystemStatus_t((uint64_t)sys_status[0] | ((uint64_t)sys_status[1] << 8) |
							   ((uint64_t)sys_status[2] << 16) | ((uint64_t)sys_status[3] << 24) |
							   ((uint64_t)sys_status[4] << 32) | ((uint64_t)sys_status[5] << 40));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::fetchChipState()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::SYS_STATE, 4>(sys_state));
	if (sys_state[2] == 0x00)
	{
		chip_state = Dw3110::SystemState::WAKEUP;
	} else if (sys_state[2] <= 0x02)
	{
		chip_state = Dw3110::SystemState::IDLE_RC;
	} else if (sys_state[2] == 0x03)
	{
		chip_state = Dw3110::SystemState::IDLE_PLL;
	} else if (sys_state[2] >= 0x08 && sys_state[2] <= 0x0F)
	{
		chip_state = Dw3110::SystemState::TX;
	} else if (sys_state[2] >= 0x12 && sys_state[2] <= 0x19)
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
modm::Dw3110Phy<SpiMaster, Cs>::writeRegister(std::span<const uint8_t, Len> val)
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
modm::Dw3110Phy<SpiMaster, Cs>::readModifyWriteRegister(std::span<const uint8_t, Len> or_mask,
														std::span<const uint8_t, Len> and_mask)
{
	static_assert(Len + Offset <= Reg.length,
				  "Size of masked write is too large for this register!");
	static_assert(Len == 1 || Len == 2 || Len == 4,
				  "Masked writes only support sizes of 1,2 or 4 Bytes.");

	RF_BEGIN();
	RF_CALL(readRegister<Reg, Len, Offset>(std::span<uint8_t>(temp_rw).first<Len>()));
	for (size_t i = 0; i < Len; i++)
	{
		temp_rw[i] |= or_mask[i];
		temp_rw[i] &= and_mask[i];
	}
	RF_CALL(writeRegister<Reg, Len, Offset>(std::span<uint8_t>(temp_rw).first<Len>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::Register Reg, size_t Len, size_t Offset>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::writeRegisterMasked(std::span<const uint8_t, Len> or_mask,
													std::span<const uint8_t, Len> and_mask)
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
template<modm::Dw3110::RegisterBank Reg>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::readRegisterBank(std::span<uint8_t> out, size_t len)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] = (uint8_t)((Reg.addr << 1) & 0x3E);

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 1));
	RF_CALL(SpiMaster::transfer(nullptr, out.data(), len));
	Cs::setOutput(true);
	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
template<modm::Dw3110::RegisterBank Reg>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::writeRegisterBank(std::span<const uint8_t> val, size_t len)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	tx_buffer[0] = (uint8_t)(0x80 | ((Reg.addr << 1) & 0x3E));

	Cs::setOutput(false);
	RF_CALL(SpiMaster::transfer(tx_buffer.data(), nullptr, 1));
	RF_CALL(SpiMaster::transfer(val.data(), nullptr, len));
	Cs::setOutput(true);
	this->releaseMaster();
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setInterruptsEnabled(Dw3110::SystemStatus_t mask)
{
	RF_BEGIN();
	scratch[0] = (mask.value >> 0) & 0xFF;
	scratch[1] = (mask.value >> 8) & 0xFF;
	scratch[2] = (mask.value >> 16) & 0xFF;
	scratch[3] = (mask.value >> 24) & 0xFF;
	scratch[4] = (mask.value >> 32) & 0xFF;
	scratch[5] = (mask.value >> 40) & 0xFF;
	RF_CALL(writeRegister<Dw3110::SYS_ENABLE, 6>(std::span<const uint8_t>(scratch).first<6>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setIRQPolarity(bool high)
{
	RF_BEGIN();
	if (high)
	{
		constexpr static uint8_t or_mask[] = {0x20};
		constexpr static uint8_t and_mask[] = {0xFF};
		RF_CALL(writeRegisterMasked<Dw3110::DIAG_TMC, 1, 2>(or_mask, and_mask));
	} else
	{

		constexpr static uint8_t or_mask[] = {0x00};
		constexpr static uint8_t and_mask[] = {0xDF};
		RF_CALL(writeRegisterMasked<Dw3110::DIAG_TMC, 1, 2>(or_mask, and_mask));
	}
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::setSTSLength(uint8_t len)
{
	RF_BEGIN();
	if (len < 3) { RF_RETURN(false); }
	scratch[0] = len;
	RF_CALL(writeRegister<Dw3110::STS_CFG, 1>(std::span<const uint8_t>(scratch).first<1>()));
	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setSTSMode(Dw3110::STSMode mode, bool sdc)
{
	RF_BEGIN();
	scratch[0] = ((uint8_t)mode << 4) | (sdc ? 0x80 : 0x00);
	scratch[1] = 0x4F | scratch[0];
	RF_CALL(writeRegisterMasked<Dw3110::SYS_CFG, 1, 1>(
		std::span<const uint8_t>(scratch).first<1>(),
		std::span<const uint8_t>(scratch).subspan<1, 1>()));
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint16_t>
modm::Dw3110Phy<SpiMaster, Cs>::getSTSQuality()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::STS_STS, 2>(std::span<uint8_t>(scratch).first<2>()));
	RF_END_RETURN((uint16_t)(scratch[0] << 0 | (scratch[1] & 0x0F) << 8));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::getSTSGood()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::STS_CFG, 1>(std::span<uint8_t>(scratch).first<1>()));
	RF_CALL(readRegister<Dw3110::STS_STS, 2>(std::span<uint8_t>(scratch).subspan<1, 2>()));
	RF_END_RETURN((scratch[0] + 1) * 8.0f * 0.6f < (scratch[1] | (scratch[2] << 8)));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setSTSKey(std::span<const uint8_t, 16> key)
{
	return writeRegister<Dw3110::STS_KEY, 16, 0>(key);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::setSTSIV(std::span<const uint8_t, 16> iv)
{
	return writeRegister<Dw3110::STS_IV, 16, 0>(iv);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::getSTSKey(std::span<uint8_t, 16> key)
{
	return readRegister<Dw3110::STS_KEY, 16, 0>(key);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::getSTSIV(std::span<uint8_t, 16> iv)
{
	return readRegister<Dw3110::STS_IV, 16, 0>(iv);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint32_t>
modm::Dw3110Phy<SpiMaster, Cs>::getCurrentCounter()
{
	RF_BEGIN();
	RF_CALL(readRegister<Dw3110::CTR_DBG, 4, 0>(std::span<uint8_t>(scratch).first<4>()));
	RF_END_RETURN((uint32_t)scratch[0] | (uint32_t)scratch[1] << 8 | (uint32_t)scratch[2] << 16 |
				  (uint32_t)scratch[3] << 24);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::reloadSTSIV()
{
	constexpr static uint8_t sts_ctrl[] = {0x01};
	return writeRegister<Dw3110::STS_CTRL, 1>(sts_ctrl);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::reuseLastSTSIV()
{

	constexpr static uint8_t sts_ctrl[] = {0x02};
	return writeRegister<Dw3110::STS_CTRL, 1>(sts_ctrl);
}