// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2020, Vivien Henry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADS868x_HPP
#define MODM_ADS868x_HPP


#include <modm/architecture/interface/spi_master.hpp>
#include <modm/architecture/interface/accessor.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/// @ingroup modm_driver_ads868x
struct ads868x
{
public:
	enum class
	Register: uint8_t{
		DEV_ID_REG = 0x00,
		RST_PWRCTRL_REG = 0x04,
		SDI_CTL_REG = 0x08,
		SDO_CTL_REG = 0x0C,
		DATAOUT_CTL_REG = 0x10,
		RANGE_CTL_REG = 0x14,
		ALARM_REG = 0x20,
		ALARM_H_TH_REG = 0x24,
		ALARM_L_TH_REG = 0x28,
	};


	/// Device ID register
	enum class
	DeviceIDRegister : uint32_t
	{
		Addr0 = Bit16,
		Addr1 = Bit17,
		Addr2 = Bit18,
		Addr3 = Bit19
	};
	MODM_FLAGS32(DeviceIDRegister);

	typedef modm::Value<DeviceIDRegister_t, 4, 16> DeviceID_t;


	// Reset and Power Control register
	enum class
	ResetPowerControlRegister: uint32_t
	{
		WKey0 = Bit8,
		WKey1 = Bit9,
		WKey2 = Bit10,
		WKey3 = Bit11,
		WKey4 = Bit12,
		WKey5 = Bit13,
		WKey6 = Bit14,
		WKey7 = Bit15,

		VDDAlarmDisable = Bit5,

		InputAlarmDisable = Bit4,
		RSTn_ApplicationReset = Bit2,
		NapModeEnable = Bit1,
		PowerDownEnable = Bit0
	};
	MODM_FLAGS32(ResetPowerControlRegister);

	typedef modm::Value<ResetPowerControlRegister_t, 8, 8> WriteKey_t;

	// SDI Data Input Control register
	enum class
	SDIControlRegister: uint32_t
	{
		SDIMode0 = Bit0,
		SDIMode1 = Bit1
	};
	MODM_FLAGS32(SDIControlRegister);

	enum class
	SDIMode : uint8_t
	{
		Std_Pol0_Phase0 = 0,
		Std_Pol0_Phase1 = 1,
		Std_Pol1_Phase0 = 2,
		Std_Pol1_Phase1 = 3
	};
	typedef modm::Configuration<SDIControlRegister_t, SDIMode, 0b11, 0> SDIMode_t;

	// SDO Data Output Control Register
	enum class
	SDOControlRegister: uint32_t
	{
		SDOMode0 = Bit0,
		SDOMode1 = Bit1,

		SSyncClock = Bit6,

		SDO1Config0 = Bit8,
		SDO1Config1 = Bit9,

		GPOValue = Bit12
	};
	MODM_FLAGS32(SDOControlRegister);

	enum class
	SDOMode: uint8_t
	{
		SameAsSDI0 = 0b00,
		SameAsSDI1 = 0b01,
		Invalid = 0b10,
		ADCMasterClk_SourcSync = 0b11
	};
	typedef modm::Configuration<SDOControlRegister_t, SDOMode, 0b11, 0> SDOMode_t;


	enum class
	SourceSyncClock: uint8_t
	{
		External = 0,
		Internal = 1
	};
	typedef modm::Configuration<SDOControlRegister_t, SourceSyncClock, 0b1, 6> SourceSyncClock_t;

	enum class
	SDO1Config: uint8_t
	{
		SDO1_Tristated = 0b00,
		SDO1_Alarm = 0b01,
		SDO1_GPO = 0b10,
		SDO1_2BitsSDO = 0b11
	};
	typedef modm::Configuration<SDOControlRegister_t, SDO1Config, 0b1, 8> SDO1Config_t;


	enum class
	DataOutControlRegister: uint32_t
	{
		DataVal0 = Bit0,
		DataVal1 = Bit1,
		DataVal2 = Bit2,

		ParityEnable = Bit3,

		Inc_Range = Bit8,

		Inc_InActiveAlarm_High = Bit10,
		Inc_InActiveAlarm1_Low = Bit11,

		Inc_VDDActiveAlarm0_High = Bit12,
		Inc_VDDActiveAlarm1_Low = Bit13,

		Inc_DeviceAddr = Bit14
	};
	MODM_FLAGS32(DataOutControlRegister);

	enum class
	DataValue: uint8_t
	{
		ConversionData = 0b000,
		All0 = 0b100,
		All1 = 0b101,
		Seq01 = 0b110,
		Seq0011 = 0b111
	};
	typedef modm::Configuration<DataOutControlRegister_t, DataValue, 0b111, 0> DataValue_t;


	enum class
	RangeSelectionRegister: uint32_t
	{
		RangeSel0 = Bit0,
		RangeSel1 = Bit1,
		RangeSel2 = Bit2,
		RangeSel3 = Bit3,

		InternalRefDisabled = Bit6
	};
	MODM_FLAGS32(RangeSelectionRegister);

	enum class
	RangeSel: uint8_t
	{
		Range_Bipolar_3_000_VRef = 0b0000,
		Range_Bipolar_2_500_VRef = 0b0001,
		Range_Bipolar_1_500_VRef = 0b0010,
		Range_Bipolar_1_250_VRef = 0b0011,
		Range_Bipolar_0_625_VRef = 0b0100,
		Range_Unipolar_3_00_VRef = 0b1000,
		Range_Unipolar_2_50_VRef = 0b1001,
		Range_Unipolar_1_50_VRef = 0b1010,
		Range_Unipolar_1_25_VRef = 0b1011
	};
	typedef modm::Configuration<RangeSelectionRegister_t, RangeSel, 0b1111, 0> RangeSel_t;


}; // struct ads868x

/**
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Vivien Henry
 * @ingroup modm_driver_ads868x
 */
template <typename SpiMaster, typename Cs, typename nReset>
class Ads868x : public ads868x
{
public:
	Ads868x();

	/// Call this function once before using the device
	static void initialize();

	static uint16_t singleConversion();

	static void setDeviceAddress(uint8_t devID){
		DeviceIDRegister_t dev_id_reg = DeviceID_t(devID);
		uint32_t raw = dev_id_reg.value;
		writeRegister(Register::DEV_ID_REG, raw);
	};

	static uint8_t getDeviceAddress(){
		uint32_t data = readRegister(Register::DEV_ID_REG);
		DeviceIDRegister_t dev_id_reg(data);
		return DeviceID_t::get(dev_id_reg);
	}

	static void setOutputProtocol(SDOMode mode, SourceSyncClock syncClock, SDO1Config sdo1Config){
		SDOControlRegister_t sdo_reg = SDOMode_t(mode) | SourceSyncClock_t(syncClock) | SDO1Config_t(sdo1Config);
		uint32_t raw = sdo_reg.value;
		writeRegister(Register::SDO_CTL_REG, raw);
	}

	static void setRange(RangeSel range){
		RangeSelectionRegister_t ran_reg = RangeSel_t(range);
		uint32_t raw = ran_reg.value;
		writeRegister(Register::RANGE_CTL_REG, raw);
	}

	static RangeSel getRange(){
		uint32_t data = readRegister(Register::RANGE_CTL_REG);
		RangeSelectionRegister_t rang_reg(data);
		return RangeSel_t::get(rang_reg);
	}

private:
	static void writeRegister(Register reg, uint32_t data);
	static uint32_t readRegister(Register reg);

};

// IOStream&
// operator<<(IOStream& out, const ad7928::Data& data);

} // namespace modm

#include "ads868x_impl.hpp"

#endif // MODM_ADS868x_HPP
