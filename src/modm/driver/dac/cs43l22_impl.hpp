/*
 * Copyright (c) 2021, Marton Ledneczki
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CS43L22_HPP
#	error "Don't include this file directly, use 'cs43l22.hpp' instead!"
#endif

namespace modm
{

template<typename I2cMaster, typename I2sMaster>
CS43L22<I2cMaster, I2sMaster>::CS43L22(uint8_t i2cAddress)
	: I2cDevice<I2cMaster, 3>(i2cAddress) {}

template<typename I2cMaster, typename I2sMaster>
ResumableResult<bool>
CS43L22<I2cMaster, I2sMaster>::initialize()
{
	RF_BEGIN();
	// Verify the chip ID
	success = RF_CALL_BLOCKING(readRegister(Register::ChipIdRevision));
	if (!success || (ChipId_t::get(static_cast<ChipId_t>(rx_buffer)) != ChipId::CS43L22))
	{
		RF_RETURN(false);
	}
	RF_CALL_BLOCKING(writeRegister(Register::PowerControl1, Power_t(Power::Down)));
	RF_CALL_BLOCKING(writeRegister(Register::PowerControl2, ChannelPowerHeadphoneA_t(ChannelPower::OnAlways) | 
															ChannelPowerHeadphoneB_t(ChannelPower::OnAlways) | 
															ChannelPowerSpeakerA_t(ChannelPower::OffAlways)  |
															ChannelPowerSpeakerB_t(ChannelPower::OffAlways)));
	RF_CALL_BLOCKING(writeRegister(Register::ClockingControl, ClockingControl::AUTO_DETECT));
	RF_CALL_BLOCKING(writeRegister(Register::InterfaceControl1, DacInterfaceFormat_t(DacInterfaceFormat::I2sPhillipsStandard) |
																Role_t(Role::Slave)));
	RF_CALL_BLOCKING(setMasterVolume(-30));
	RF_CALL_BLOCKING(writeRegister(Register::PowerControl1, Power_t(Power::Up)));
	/* Additional configuration for the CODEC. These configurations are done to reduce
	the time needed for the Codec to power off. If these configurations are removed, 
	then a long delay should be added between powering off the Codec and switching 
	off the I2S peripheral MCLK clock (which is the operating clock for Codec).
	If this delay is not inserted, then the codec will not shut down properly and
	it results in high noise after shut down. */
	RF_CALL_BLOCKING(writeRegister(Register::AnalogZcAndSrSettings, AnalogSoftRampA_t(SoftRamp::Disabled) |
	    															AnalogSoftRampB_t(SoftRamp::Disabled) |
																	AnalogZeroCrossingA_t(ZeroCrossing::Disabled) |
																	AnalogZeroCrossingB_t(ZeroCrossing::Disabled)));
	/* Disable the digital soft ramp */
	RF_CALL_BLOCKING(writeRegister(Register::MiscellaneousControls, MiscellaneousControls_t(0x00)));
	/* Disable the limiter attack level */
	RF_CALL_BLOCKING(writeRegister(Register::LimiterControl1MinMaxThresholds, LimiterControl1MinMaxThresholds_t(0x00)));
	RF_END_RETURN(success);
}

template<typename I2cMaster, typename I2sMaster>
ResumableResult<bool>
CS43L22<I2cMaster, I2sMaster>::writeRegister(Register reg, RegisterValue_t value)
{
	RF_BEGIN();
	tx_buffer[0] = static_cast<uint8_t>(reg);
	tx_buffer[1] = value.value;
	this->transaction.configureWrite(tx_buffer, 2);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster, typename I2sMaster>
ResumableResult<bool>
CS43L22<I2cMaster, I2sMaster>::readRegister(Register reg)
{
	RF_BEGIN();
	rx_buffer = static_cast<uint8_t>(reg);
	this->transaction.configureRead(&rx_buffer, 1);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster, typename I2sMaster>
ResumableResult<bool>
CS43L22<I2cMaster, I2sMaster>::setMasterVolume(centiBels_t vol)
{
	RF_BEGIN();
	if (vol > MaxVolume)
		vol = MaxVolume;
	else if (vol < MinVolume)
		vol = MinVolume;
	vol /= 5;
	RF_CALL_BLOCKING(writeRegister(Register::MasterVolumeControlA, static_cast<MasterVol_t>(vol)));
	RF_CALL_BLOCKING(writeRegister(Register::MasterVolumeControlB, static_cast<MasterVol_t>(vol)));
	RF_END_RETURN(true);
}

}	// namespace modm