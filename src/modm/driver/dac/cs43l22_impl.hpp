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

template<typename I2cMaster>
Cs43l22<I2cMaster>::Cs43l22(uint8_t i2cAddress)
	: I2cDevice<I2cMaster, 5>(i2cAddress)
{
	volume = -300;
}

template<typename I2cMaster>
ResumableResult<bool>
Cs43l22<I2cMaster>::initialize()
{
	RF_BEGIN();
	// Verify the chip ID
	success = RF_CALL(readRegister(Register::ChipIdRevision));
	if (!success || (ChipId_t::get(static_cast<ChipId_t>(rx_buffer)) != ChipId::CS43L22))
	{
		RF_RETURN(false);
	}
	RF_CALL(writeRegister(Register::PowerControl1, Power_t(Power::Down)));
	RF_CALL(writeRegister(Register::PowerControl2, ChannelPowerHeadphoneA_t(ChannelPower::OnAlways) |
															ChannelPowerHeadphoneB_t(ChannelPower::OnAlways) |
															ChannelPowerSpeakerA_t(ChannelPower::OffAlways)  |
															ChannelPowerSpeakerB_t(ChannelPower::OffAlways)));
	RF_CALL(writeRegister(Register::ClockingControl, ClockingControl::AUTO_DETECT));
	RF_CALL(writeRegister(Register::InterfaceControl1, DacInterfaceFormat_t(DacInterfaceFormat::I2sPhillipsStandard) |
																Role_t(Role::Slave)));
	RF_CALL(setMasterVolume(volume));
	RF_CALL(writeRegister(Register::PowerControl1, Power_t(Power::Up)));
	/* Additional configuration for the CODEC. These configurations are done to reduce
	the time needed for the Codec to power off. If these configurations are removed,
	then a long delay should be added between powering off the Codec and switching
	off the I2S peripheral MCLK clock (which is the operating clock for Codec).
	If this delay is not inserted, then the codec will not shut down properly and
	it results in high noise after shut down. */
	RF_CALL(writeRegister(Register::AnalogZcAndSrSettings, AnalogSoftRampA_t(SoftRamp::Disabled) |
	    															AnalogSoftRampB_t(SoftRamp::Disabled) |
																	AnalogZeroCrossingA_t(ZeroCrossing::Disabled) |
																	AnalogZeroCrossingB_t(ZeroCrossing::Disabled)));
	/* Disable the digital soft ramp */
	RF_CALL(writeRegister(Register::MiscellaneousControls, MiscellaneousControls_t(0x00)));
	/* Disable the limiter attack level */
	RF_CALL(writeRegister(Register::LimiterControl1MinMaxThresholds, LimiterControl1MinMaxThresholds_t(0x00)));
	RF_END_RETURN(success);
}

template<typename I2cMaster>
ResumableResult<bool>
Cs43l22<I2cMaster>::writeRegister(Register reg, RegisterValue_t value)
{
	RF_BEGIN();
	tx_buffer[0] = static_cast<uint8_t>(reg);
	tx_buffer[1] = value.value;
	this->transaction.configureWrite(tx_buffer, 2);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<bool>
Cs43l22<I2cMaster>::readRegister(Register reg)
{
	RF_BEGIN();
	rx_buffer = static_cast<uint8_t>(reg);
	// First, set the internal address pointer
	// of the DAC to the requested register
	this->transaction.configureWrite(&rx_buffer, 1);
	RF_CALL(this->runTransaction());
	this->transaction.configureRead(&rx_buffer, 1);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<bool>
Cs43l22<I2cMaster>::setMasterVolume(centiBel_t vol)
{
	RF_BEGIN();
	{
		volume = vol;
		if (volume > MaxVolume)
			volume = MaxVolume;
		else if (volume < MinVolume)
			volume = MinVolume;
		volume /= 5;
		if (volume < -128)
			volume += 256;
	}
	RF_CALL(writeRegister(Register::MasterVolumeControlA, static_cast<MasterVol_t>(volume)));
	RF_CALL(writeRegister(Register::MasterVolumeControlB, static_cast<MasterVol_t>(volume)));
	RF_END_RETURN(true);
}

template<typename I2cMaster>
void
Cs43l22<I2cMaster>::regToCentibel(uint8_t reg)
{
	volume = reg;
	if (volume <= 24 and volume >= 0)
		volume *= 5;
	else if (volume <= 52 and volume > 24)
		volume = MinVolume;
	else if (volume <= 127 and volume > 52)
		volume = (256-volume)*(-5);
	else
	{
		volume |= 0xFF00;
		volume *= 5;
	}
}

template<typename I2cMaster>
ResumableResult<bool>
Cs43l22<I2cMaster>::setMasterVolumeRelative(centiBel_t rel_vol)
{
	RF_BEGIN();
	if (RF_CALL(getMasterVolume()))
	{
		regToCentibel(rx_buffer);
		volume += rel_vol;
		RF_RETURN_CALL(setMasterVolume(volume));
	}
	RF_END_RETURN(false);
}

template<typename I2cMaster>
ResumableResult<bool>
Cs43l22<I2cMaster>::getMasterVolume()
{
	return readRegister(Register::MasterVolumeControlA);
}

}	// namespace modm
