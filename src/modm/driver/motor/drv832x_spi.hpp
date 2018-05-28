// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DRV832X_SPI_HPP
#define MODM_DRV832X_SPI_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

struct drv832xSpi
{
	enum class
	Register : uint16_t
	{
		FaultStatus1		= 0x0,
		VgsStatus2			= 0x1,
		DriverControl		= 0x2,
		GateDriveHS			= 0x3,
		GateDriveLS			= 0x4,
		OcpControl			= 0x5,
		CsaControl			= 0x6,
	};

	enum class FaultStatus1 : uint16_t
	{
		VdsOvercurrentLC	= Bit0,
		VdsOvercurrentHC	= Bit1,
		VdsOvercurrentLB	= Bit2,
		VdsOvercurrentHB	= Bit3,
		VdsOvercurrentLA	= Bit4,
		VdsOvercurrentHA	= Bit5,
		OvertemperatureShutdown	= Bit6,
		UndervoltageLockout		= Bit7,
		GateDriveFault		= Bit8,
		VdsOvercurrentFault	= Bit9,
		Fault				= Bit10,
	};
	MODM_FLAGS16(FaultStatus1);

	enum class VgsStatus2 : uint16_t
	{
		VgsGateDriveFaultLC	= Bit0,
		VgsGateDriveFaultHC	= Bit1,
		VgsGateDriveFaultLB	= Bit2,
		VgsGateDriveFaultHB	= Bit3,
		VgsGateDriveFaultLA	= Bit4,
		VgsGateDriveFaultHA	= Bit5,
		ChargePumpUndervoltageFault	= Bit6,
		OvertemperatureWarning		= Bit7,
		SenseCOvercurrent	= Bit8,
		SenseBOvercurrent	= Bit9,
		SenseAOvercurrent	= Bit10,
	};
	MODM_FLAGS16(VgsStatus2);

	enum class DriverControl : uint16_t
	{
		ClearFaultBits		= Bit0,
		Brake				= Bit1,
		Coast				= Bit2,
		Pwm1Dir				= Bit3,
		Pwm1Com				= Bit4,
		PwmMode1			= Bit5,
		PwmMode2			= Bit6,
		OvertemperatureReport	= Bit7,
		DisableGateDriveFault	= Bit8,
		DisableChargePumpUVLO	= Bit9,
	};
	MODM_FLAGS16(DriverControl);

	enum class PwmMode : uint16_t
	{
		PwmMode6x			= 0b00,
		PwmMode3x			= 0b01,
		PwmMode1x			= 0b10,
		PwmModeIndependent	= 0b11,
	};
	using PwmMode_t = Configuration< DriverControl_t, PwmMode, 0b11, 5 >; // Bit 5..6

	enum class GateDriveHS : uint16_t
	{
		IDriveN0	= Bit0,
		IDriveN1	= Bit1,
		IDriveN2	= Bit2,
		IDriveN3	= Bit3,

		IDriveP0	= Bit4,
		IDriveP1	= Bit5,
		IDriveP2	= Bit6,
		IDriveP3	= Bit7,

		Lock0		= Bit8,
		Lock1		= Bit9,
		Lock2		= Bit10,
	};
	MODM_FLAGS16(GateDriveHS);

	enum class Lock : uint16_t
	{
		Lock		= 0b110,
		Unlock		= 0b011,
	};
	typedef Configuration< GateDriveHS_t, Lock, 0b111, 8 >  Lock_t; // Bit 8..10

	enum class GateDriveLS : uint16_t
	{
		IDriveN0	= Bit0,
		IDriveN1	= Bit1,
		IDriveN2	= Bit2,
		IDriveN3	= Bit3,

		IDriveP0	= Bit4,
		IDriveP1	= Bit5,
		IDriveP2	= Bit6,
		IDriveP3	= Bit7,

		TDrive0		= Bit8,
		TDrive1		= Bit9,

		Cbc			= Bit10,	/// Cbc: In retry OCP_MODE,for both VDS_OCP and SEN_OCP,
								/// the fault is automatically cleared when a PWM input is given
	};
	MODM_FLAGS16(GateDriveLS);

	enum class TDrive : uint16_t
	{
		ns500		= 0b00,
		ns1000		= 0b01,
		ns2000		= 0b10,
		ns4000		= 0b11,
	};
	typedef Configuration< GateDriveLS_t, TDrive, 0b11, 8 >  TDrive_t; // Bit 8..9

	enum class IDriveN : uint16_t
	{
		mA20		= 0b0000,
		mA60		= 0b0001,
		mA120		= 0b0010,
		mA160		= 0b0011,
		mA240		= 0b0100,
		mA280		= 0b0101,
		mA340		= 0b0110,
		mA380		= 0b0111,
		mA520		= 0b1000,
		mA660		= 0b1001,
		mA740		= 0b1010,
		mA880		= 0b1011,
		mA1140		= 0b1100,
		mA1360		= 0b1101,
		mA1640		= 0b1110,
		mA2000		= 0b1111,
	};
	typedef Configuration< GateDriveLS_t, IDriveN, 0b1111, 0 > LS_IDriveN_t; // Bit 0..3
	typedef Configuration< GateDriveHS_t, IDriveN, 0b1111, 0 > HS_IDriveN_t; // Bit 0..3

	enum class IDriveP : uint16_t
	{
		mA10		= 0b0000,
		mA30		= 0b0001,
		mA60		= 0b0010,
		mA80		= 0b0011,
		mA120		= 0b0100,
		mA140		= 0b0101,
		mA170		= 0b0110,
		mA190		= 0b0111,
		mA260		= 0b1000,
		mA330		= 0b1001,
		mA370		= 0b1010,
		mA440		= 0b1011,
		mA570		= 0b1100,
		mA680		= 0b1101,
		mA820		= 0b1110,
		mA1000		= 0b1111,
	};
	typedef Configuration< GateDriveLS_t, IDriveP, 0b1111, 4 > LS_IDriveP_t; // Bit 4..7
	typedef Configuration< GateDriveHS_t, IDriveP, 0b1111, 4 > HS_IDriveP_t; // Bit 4..7

	enum class OcpControl : uint16_t
	{
		VdsLevel0		= Bit0,
		VdsLevel1		= Bit1,
		VdsLevel2		= Bit2,
		VdsLevel3		= Bit3,

		OcpDeglitch0	= Bit4,
		OcpDeglitch1	= Bit5,

		OcpMode0		= Bit6,
		OcpMode1		= Bit7,

		Deadtime0		= Bit8,
		Deadtime1		= Bit9,

		TimeRetry		= Bit10,
	};
	MODM_FLAGS16(OcpControl);

	enum class VdsLevel : uint16_t
	{
		mV60		= 0b0000,
		mV130		= 0b0001,
		mV200		= 0b0010,
		mV260		= 0b0011,
		mV310		= 0b0100,
		mV450		= 0b0101,
		mV530		= 0b0110,
		mV600		= 0b0111,
		mV680		= 0b1000,
		mV750		= 0b1001, /// default
		mV940		= 0b1010,
		mV1130		= 0b1011,
		mV1300		= 0b1100,
		mV1500		= 0b1101,
		mV1700		= 0b1110,
		mV1880		= 0b1111,
	};
	typedef Configuration< OcpControl_t, VdsLevel, 0b1111, 0 >  VdsLevel_t; // Bit 0..3

	enum class OcpDeglitch : uint16_t
	{
		us2		= 0b00,
		us4		= 0b01,
		us6		= 0b10,
		us8		= 0b11,
	};
	typedef Configuration< OcpControl_t, OcpDeglitch, 0b11, 4 >  OcpDeglitch_t; // Bit 4..5

	enum class OcpMode : uint16_t
	{
		LatchedFault	= 0b00,
		AutoRetryFault	= 0b01,
		OnlyReport		= 0b10,
		NoOcp			= 0b11,
	};
	typedef Configuration< OcpControl_t, OcpMode, 0b11, 6 >  OcpMode_t; // Bit 6..7

	enum class CsaControl : uint16_t
	{
		SenseOcpLevel0		= Bit0,
		SenseOcpLevel1		= Bit1,

		CsaCalibrationC		= Bit2,

		CsaCalibrationB		= Bit3,

		CsaCalibrationA		= Bit4,

		DisableOvercurrentSense	= Bit5,

		CsaGain0			= Bit6,
		CsaGain1			= Bit7,

		LowSideReference	= Bit8,

		VrefDiv2			= Bit9,

		CsaFet				= Bit10,
	};
	MODM_FLAGS16(CsaControl);

	enum class SenseOcpLevel : uint16_t
	{
		SenseOcp0V25	= 0b00,
		SenseOcp0V5		= 0b01,
		SenseOcp0V75	= 0b10,
		SenseOcp1V		= 0b11,
	};
	typedef Configuration< CsaControl_t, SenseOcpLevel, 0b11, 0 >  SenseOcpLevel_t; // Bit 0..1

	enum class CsaGain : uint16_t
	{
		Gain5		= 0b00,
		Gain10		= 0b01,
		Gain20		= 0b10,
		Gain40		= 0b11,
	};
	typedef Configuration< CsaControl_t, CsaGain, 0b11, 6 >  CsaGain_t; // Bit 6..7
};

/**
 * \brief	DRV832xS: 6 to 60-V Three-Phase Smart Gate Driver
 *
 * This driver only covers the gate driver configuration accessible via SPI interface.
 *
 * \see		<a href="http://www.ti.com/lit/ds/symlink/drv8320.pdf">DRV832x Datasheet</a>
 *
 * \ingroup	driver_motor
 * \author	Raphael Lehmann
 */
template < class SpiMaster, class Cs >
class Drv832xSpi : public drv832xSpi, public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<3>
{
public:
	/**
	 * \brief	Initialize
	 *
	 * Sets used pins as output. SPI must be initialized by the user!
	 */
	Drv832xSpi();

	modm::ResumableResult<void>
	readFaultStatus1();

	modm::ResumableResult<void>
	readVgsStatus2();

	modm::ResumableResult<void>
	readDriverControl();

	modm::ResumableResult<void>
	readGateDriveHS();

	modm::ResumableResult<void>
	readGateDriveLS();

	modm::ResumableResult<void>
	readOcpControl();

	modm::ResumableResult<void>
	readCsaControl();

	modm::ResumableResult<void>
	readAll();

	modm::ResumableResult<void>
	initialize();

private:
	FaultStatus1_t _faultStatus1;
	VgsStatus2_t _vgsStatus2;
	DriverControl_t _driverControl;
	GateDriveHS_t _gateDriveHS;
	GateDriveLS_t _gateDriveLS;
	OcpControl_t _ocpControl;
	CsaControl_t _csaControl;

	// access bitmap for variables above
	uint8_t accessBitmap = 0;
public:
	const FaultStatus1_t& faultStatus1() {
		return _faultStatus1;
	}
	const VgsStatus2_t& vgsStatus2() {
		return _vgsStatus2;
	}
	DriverControl_t& driverControl() {
		accessBitmap |= 0b0000100;
		return _driverControl;
	}
	GateDriveHS_t& gateDriveHS() {
		accessBitmap |= 0b0001000;
		return _gateDriveHS;
	}
	GateDriveLS_t& gateDriveLS() {
		accessBitmap |= 0b0010000;
		return _gateDriveLS;
	}
	OcpControl_t& ocpControl() {
		accessBitmap |= 0b0100000;
		return _ocpControl;
	}
	CsaControl_t& csaControl() {
		accessBitmap |= 0b1000000;
		return _csaControl;
	}

	modm::ResumableResult<void>
	commit();

protected:
	modm::ResumableResult<void>
	writeData(Register address, uint16_t data);

	modm::ResumableResult<uint16_t>
	readData(Register address);

	modm::ResumableResult<void>
	readData(Register address, uint16_t& data);

private:
	uint8_t inBuffer[2];
	uint8_t outBuffer[2];
};

// ------------------------------------------------------------------------
// Output operators
IOStream&
operator << (IOStream& os, const drv832xSpi::FaultStatus1_t& c);

IOStream&
operator << (IOStream& os, const drv832xSpi::VgsStatus2_t& c);

IOStream&
operator << (IOStream& os, const drv832xSpi::DriverControl_t& c);

IOStream&
operator << (IOStream& os, const drv832xSpi::GateDriveHS_t& c);

IOStream&
operator << (IOStream& os, const drv832xSpi::GateDriveLS_t& c);

IOStream&
operator << (IOStream& os, const drv832xSpi::OcpControl_t& c);

IOStream&
operator << (IOStream& os, const drv832xSpi::CsaControl_t& c);

}

#include "drv832x_spi_impl.hpp"

#endif // MODM_DRV832X_SPI_HPP
