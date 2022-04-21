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
#define MODM_CS43L22_HPP

#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/// @ingroup modm_driver_cs43l22
struct cs43l22
{
	enum class
	Register : uint8_t
	{
		ChipIdRevision = 0x01,
		PowerControl1 = 0x02,
		PowerControl2 = 0x04,
		ClockingControl = 0x05,
		InterfaceControl1 = 0x06,
		InterfaceControl2 = 0x07,
		PassthroughASelect = 0x08,
		PassthroughBSelect = 0x09,
		AnalogZcAndSrSettings = 0x0A,
		PassthroughGansControl = 0x0C,
		PlaybackControl1 = 0x0D,
		MiscellaneousControls = 0x0E,
		PlaybackControl2 = 0x0F,
		PassthroughAVolume = 0x14,
		PassthroughBVolume = 0x15,
		PcmAVolume = 0x1A,
		PcmBVolume = 0x1B,
		BeepFrequencyAndOnTime = 0x1C,
		BeepVolumeAndOffTime = 0x1D,
		BeepAndToneConfiguration = 0x1E,
		ToneControl = 0x1F,
		MasterVolumeControlA = 0x20,
		MasterVolumeControlB = 0x21,
		HeadphoneVolumeControlA = 0x22,
		HeadphoneVolumeControlB = 0x23,
		SpeakerVolumeControlA = 0x24,
		SpeakerVolumeControlB = 0x25,
		PcmChannelSwap = 0x26,
		LimiterControl1MinMaxThresholds = 0x27,
		LimiterControl2ReleaseRate = 0x28,
		LimiterAttackRate = 0x29,
		Status = 0x2E,
		VpBatteryLevel = 0x30,
		SpeakerStatus = 0x31,
		ChargePumpFrequency = 0x34
	};

	enum class
	ChipIdRevision : uint8_t
	{
		REVID0  = Bit0,
		REVID1  = Bit1,
		REVID2  = Bit2,

		CHIPID0 = Bit3,
		CHIPID1 = Bit4,
		CHIPID2 = Bit5,
		CHIPID3 = Bit6,
		CHIPID4 = Bit7
	};
	MODM_FLAGS8(ChipIdRevision);

	enum class
	ChipId : uint8_t
	{
		CS43L22 = int(ChipIdRevision::CHIPID4) | int(ChipIdRevision::CHIPID3) | int(ChipIdRevision::CHIPID2)
	};
	using ChipId_t = Configuration< ChipIdRevision_t, ChipId, (Bit7 | Bit6 | Bit5 | Bit4 | Bit3) >;

	enum class
	RevisionId : uint8_t
	{
		A0 = 0,
		A1 = int(ChipIdRevision::REVID0),
		B0 = int(ChipIdRevision::REVID1),
		B1 = int(ChipIdRevision::REVID1) | int(ChipIdRevision::REVID0)
	};
	using RevisionId_t = Configuration< ChipIdRevision_t, RevisionId, (Bit2 | Bit1 | Bit0) >;

	enum class
	PowerControl1 : uint8_t
	{
		PDN0 = Bit0,
		PDN1 = Bit1,
		PDN2 = Bit2,
		PDN3 = Bit3,
		PDN4 = Bit4,
		PDN5 = Bit5,
		PDN6 = Bit6,
		PDN7 = Bit7,
	};
	MODM_FLAGS8(PowerControl1);

	enum class
	Power : uint8_t
	{
		Down = int(PowerControl1::PDN0),
		Up   = int(PowerControl1::PDN7) |
		       int(PowerControl1::PDN4) |
			   int(PowerControl1::PDN3) |
			   int(PowerControl1::PDN2) |
			   int(PowerControl1::PDN1)
	};
	using Power_t = Configuration< PowerControl1_t, Power, 0xFF >;

	enum class
	PowerControl2 : uint8_t
	{
		PDN_SPKA0 = Bit0,
		PDN_SPKA1 = Bit1,

		PDN_SPKB0 = Bit2,
		PDN_SPKB1 = Bit3,

		PDN_HPA0  = Bit4,
		PDN_HPA1  = Bit5,

		PDN_HPB0  = Bit6,
		PDN_HPB1  = Bit7,
	};
	MODM_FLAGS8(PowerControl2);

	enum class
	ChannelPower : uint8_t
	{
		OnWhenSelectorPinLo = 0,
		OnWhenSelectorPinHi = 1,
		OnAlways = 2,
		OffAlways = 3
	};
	using ChannelPowerSpeakerA_t = Configuration< PowerControl2_t, ChannelPower, 0b11, 0>;
	using ChannelPowerSpeakerB_t = Configuration< PowerControl2_t, ChannelPower, 0b11, 2>;
	using ChannelPowerHeadphoneA_t = Configuration< PowerControl2_t, ChannelPower, 0b11, 4>;
	using ChannelPowerHeadphoneB_t = Configuration< PowerControl2_t, ChannelPower, 0b11, 6>;

	enum class
	ClockingControl : uint8_t
	{
		MCLKDIV2    = Bit0,

		RATIO0      = Bit1,
		RATIO1      = Bit2,

		VIDEOCLK    = Bit3,
		GROUP_32K   = Bit4,

		SPEED0      = Bit5,
		SPEED1      = Bit6,

		AUTO_DETECT = Bit7
	};
	MODM_FLAGS8(ClockingControl);

	enum class
	InterfaceControl1 : uint8_t
	{
		AWL0     = Bit0,
		AWL1     = Bit1,

		DACDIF0  = Bit2,
		DACDIF1  = Bit3,

		DSP      = Bit4,
		INV_SCLK = Bit6,
		MASTER   = Bit7  // Master (output only), otherwise Slave (input only)
	};
	MODM_FLAGS8(InterfaceControl1);

	enum class
	Role
	{
		Slave = 0,
		Master = int(InterfaceControl1::MASTER)
	};
	using Role_t = Configuration< InterfaceControl1_t, Role, Bit7 >;

	enum class
	DacInterfaceFormat : uint8_t
	{
		LeftJustified       = 0,
		I2sPhillipsStandard = int(InterfaceControl1::DACDIF0),
		RightJustified      = int(InterfaceControl1::DACDIF1)
	};
	using DacInterfaceFormat_t = Configuration< InterfaceControl1_t, DacInterfaceFormat, (Bit2 | Bit3) >;

	enum class
	MasterVolumeControl : uint8_t
	{
		MSTVOL0 = Bit0,
		MSTVOL1 = Bit1,
		MSTVOL2 = Bit2,
		MSTVOL3 = Bit3,
		MSTVOL4 = Bit4,
		MSTVOL5 = Bit5,
		MSTVOL6 = Bit6,
		MSTVOL7 = Bit7,
	};
	MODM_FLAGS8(MasterVolumeControl);
	using MasterVol_t = Value< MasterVolumeControl_t, 8 >;

	enum class
	AnalogZcAndSrSettings : uint8_t
	{
		ANLGZCA  = Bit0,
		ANLGSFTA = Bit1,
		ANLGZCB  = Bit2,
		ANLGSFTB = Bit2
	};
	MODM_FLAGS8(AnalogZcAndSrSettings);

	enum class
	SoftRamp : uint8_t
	{
		Disabled = 0,
		Enabled  = 1
	};
	using AnalogSoftRampA_t = Configuration< AnalogZcAndSrSettings_t, SoftRamp, 0b1, 1>;
	using AnalogSoftRampB_t = Configuration< AnalogZcAndSrSettings_t, SoftRamp, 0b1, 3>;

	enum class
	ZeroCrossing : uint8_t
	{
		Disabled = 0,
		Enabled  = 1
	};
	using AnalogZeroCrossingA_t = Configuration< AnalogZcAndSrSettings_t, ZeroCrossing, 0b1, 0>;
	using AnalogZeroCrossingB_t = Configuration< AnalogZcAndSrSettings_t, ZeroCrossing, 0b1, 2>;

	enum class
	MiscellaneousControls
	{
		DIGZC     = Bit0,
		DIGSFT    = Bit1,
		DEEMPH    = Bit2,
		FREEZE    = Bit3,
		PASSAMUTE = Bit4,
		PASSBMUTE = Bit5,
		PASSTHRUA = Bit6,
		PASSTHRUB = Bit7
	};
	MODM_FLAGS8(MiscellaneousControls);

	enum class
	LimiterControl1MinMaxThresholds
	{
		LIMZCDIS = Bit0,
		LIMSRDIS = Bit1,

		CUSH0    = Bit2,
		CUSH1    = Bit3,
		CUSH2    = Bit4,

		LMAX0    = Bit5,
		LMAX1    = Bit6,
		LMAX2    = Bit7
	};
	MODM_FLAGS8(LimiterControl1MinMaxThresholds);

	using RegisterValue_t = FlagsGroup< ChipIdRevision_t, PowerControl1_t, PowerControl2_t,
										ClockingControl_t, InterfaceControl1_t,
										MasterVolumeControl_t, AnalogZcAndSrSettings_t,
										MiscellaneousControls_t, LimiterControl1MinMaxThresholds_t >;

	using centiBel_t = int16_t;
	static constexpr centiBel_t MaxVolume = 120;
	static constexpr centiBel_t MinVolume = -1020;
};  // struct cs43l22


/**
 * Cirrus Logic CS43L22, Portable Audio DAC with Integrated Class D Speaker Driver
 *
 * @tparam I2cMaster I²C interface
 *
 * @author Marton Ledneczki
 * @ingroup modm_driver_cs43l22
 */
template<typename I2cMaster>
class Cs43l22 : public cs43l22, public modm::I2cDevice<I2cMaster, 5>
{
public:
	Cs43l22(uint8_t i2cAddress);

	/// Initialize device, call before using any other function
	ResumableResult<bool>
	initialize();

	/**
	 * @note max volume is specified by cs43l22::MaxVolume
	 *  and min volume by cs43l22::MinVolume
	 */
	ResumableResult<bool>
	setMasterVolume(centiBel_t vol);

	ResumableResult<bool>
	setMasterVolumeRelative(centiBel_t rel_vol);

private:
	uint8_t tx_buffer[2];
	uint8_t rx_buffer;
	centiBel_t volume;
	bool success;

	ResumableResult<bool>
	writeRegister(Register reg, RegisterValue_t value);

	ResumableResult<bool>
	readRegister(Register reg);

	ResumableResult<bool>
	getMasterVolume();

	void
	regToCentibel(uint8_t reg);

};

}  // namespace modm

#include "cs43l22_impl.hpp"

#endif // MODM_CS43L22_HPP
