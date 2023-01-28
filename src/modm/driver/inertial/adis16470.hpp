// coding: utf-8
/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADIS16470_HPP
#define MODM_ADIS16470_HPP

#include <array>
#include <optional>
#include <span>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/processing/timer/timeout.hpp>

namespace modm
{

/// @ingroup modm_driver_adis16470
struct adis16470
{
	/// Available registers
	enum class
	Register : uint8_t
	{
		DIAG_STAT		= 0x02,
		X_GYRO_LOW		= 0x04,
		X_GYRO_OUT		= 0x06,
		Y_GYRO_LOW		= 0x08,
		Y_GYRO_OUT		= 0x0A,
		Z_GYRO_LOW		= 0x0C,
		Z_GYRO_OUT		= 0x0E,
		X_ACCL_LOW		= 0x10,
		X_ACCL_OUT		= 0x12,
		Y_ACCL_LOW		= 0x14,
		Y_ACCL_OUT		= 0x16,
		Z_ACCL_LOW		= 0x18,
		Z_ACCL_OUT		= 0x1A,
		TEMP_OUT		= 0x1C,
		TIME_STAMP		= 0x1E,
		DATA_CNTR		= 0x22,
		X_DELTANG_LOW	= 0x24,
		X_DELTANG_OUT	= 0x26,
		Y_DELTANG_LOW	= 0x28,
		Y_DELTANG_OUT	= 0x2A,
		Z_DELTANG_LOW	= 0x2C,
		Z_DELTANG_OUT	= 0x2E,
		X_DELTVEL_LOW	= 0x30,
		X_DELTVEL_OUT	= 0x32,
		Y_DELTVEL_LOW	= 0x34,
		Y_DELTVEL_OUT	= 0x36,
		Z_DELTVEL_LOW	= 0x38,
		Z_DELTVEL_OUT	= 0x3A,
		XG_BIAS_LOW		= 0x40,
		XG_BIAS_HIGH	= 0x42,
		YG_BIAS_LOW		= 0x44,
		YG_BIAS_HIGH	= 0x46,
		ZG_BIAS_LOW		= 0x48,
		ZG_BIAS_HIGH	= 0x4A,
		XA_BIAS_LOW		= 0x4C,
		XA_BIAS_HIGH	= 0x4E,
		YA_BIAS_LOW		= 0x50,
		YA_BIAS_HIGH	= 0x52,
		ZA_BIAS_LOW		= 0x54,
		ZA_BIAS_HIGH	= 0x56,
		FILT_CTRL		= 0x5C,
		MSC_CTRL		= 0x60,
		UP_SCALE		= 0x62,
		DEC_RATE		= 0x64,
		NULL_CNFG		= 0x66,
		GLOB_CMD		= 0x68,
		FIRM_REV		= 0x6C,
		FIRM_DM			= 0x6E,
		FIRM_Y			= 0x70,
		PROD_ID			= 0x72,
		SERIAL_NUM		= 0x74,
		USER_SCR1		= 0x76,
		USER_SCR2		= 0x78,
		USER_SCR3		= 0x7A,
		FLSHCNT_LOW		= 0x7C,
		FLSHCNT_HIGH	= 0x7E,
	};

	enum class
	AccessMethod : uint8_t
	{
		Read	= 0b01,
		Write	= 0b10,
		ReadWrite	= 0b11,
	};

	/// Get the allowed register access modes for a given Register
	constexpr AccessMethod
	getRegisterAccess(Register r)
	{
		if (r == Register::GLOB_CMD) {
			return AccessMethod::Write;
		}
		if (r <= Register::Z_DELTVEL_OUT) {
			return AccessMethod::Read;
		}
		if ((r >= Register::FIRM_REV) && (r != Register::USER_SCR1) && (r != Register::USER_SCR2) && (r != Register::USER_SCR3)) {
			return AccessMethod::Read;
		}
		return AccessMethod::ReadWrite;
	}

	/// Status/Error Flag Indicators
	/// DIAG_STAT register (address 0x02, 0x03) reset value is 0x0000
	enum class
	DiagStat : uint16_t
	{
		// Reserved				= Bit8 to Bit15,
		ClockError				= Bit7, ///< Indicates that the internal data sampling clock does not synchronize with the external clock, which only applies when using scale sync mode.
		MemoryFailure			= Bit6, ///< Indicates a failure in the flash memory test.
		SensorFailure			= Bit5, ///< Indicates failure of at least one sensor, at the conclusion of the self test.
		StandbyMode				= Bit4, ///< Indicates that the voltage across VDD and GND is <2.8 V, which causes data processing to stop.
		SpiCommunicationError	= Bit3, ///< Indicates that the total number of SCLK cycles is not equal to an integer multiple of 16.
		FlashUpdateFailure		= Bit2, ///< Indicates that the most recent flash memory update failed.
		DataPathOverrun			= Bit1, ///< Indicates that one of the data paths have experienced an overrun condition.
		// Reserved				= Bit0,
	};
	MODM_FLAGS16(DiagStat);

	/// Miscellaneous Control Register
	/// MSC_CTRL register (address 0x60, 0x61) reset value is 0x00C1
	enum class
	MscCtrl : uint16_t
	{
		// NotUsed				= Bit8 to Bit15,
		LinearGCompensationGyro	= Bit7, ///< Linear g compensation for gyroscopes (enabled if bit is set)
		PointOfPercussionAlign	= Bit6, ///< Point of percussion alignment (enabled if bit is set)
		// NotUsed				= Bit5,
		SyncFunction2			= Bit4, ///< See SyncFunction_t
		SyncFunction1			= Bit3, ///< See SyncFunction_t
		SyncFunction0			= Bit2, ///< See SyncFunction_t
		SyncPolarity			= Bit1, ///< Rising edge triggers sampling if this bit is set, falling edge otherwise
		DrPolarity				= Bit0, ///< DR pins is active high when data is valid if this bit is set, DR is actice low otherwise
	};
	MODM_FLAGS16(MscCtrl);

	enum class SyncFunction : uint16_t
	{
		//Reserved		= 0b111,
		//Reserved		= 0b110,
		PulseSync		= 0b101,
		//Reserved		= 0b100,
		OutputSync		= 0b011,
		ScaledSync		= 0b010,
		DirectSync		= 0b001,
		InternalClock	= 0b000, ///< InternalClock is default mode
	};
	using SyncFunction_t = Configuration< MscCtrl_t, SyncFunction, 0b111, 2 >; // Bit 2..4

	/// Global Commands
	/// GLOB_CMD register (address 0x68, 0x69) is write only
	enum class
	GlobCmd : uint16_t
	{
		// NotUsed					= Bit8 to Bit15,
		SoftwareReset				= Bit7, ///< This reset clears all data, and then restarts data sampling and processing. Software alternative to toggling the RST pin.
		// NotUsed					= Bit6,
		// NotUsed					= Bit5,
		FlashMemoryTest				= Bit4, ///< Triggers flash memory test; result can be read from DIAG_STAT register
		FlashMemoryUpdate			= Bit3, ///< Backups all applicable registers in the flash memory
		SensorSelfTest				= Bit2, ///< Triggers a self test. Motion during the execution of this test can indicate a false failure.
		FactoryCalibrationRestore	= Bit1, ///< Triggers a factory calibration restore
		BiasCorrectionUpdate		= Bit0, ///< Triggers a bias correction update
	};
	MODM_FLAGS16(GlobCmd);
};

/**
 * \ingroup	modm_driver_adis16470
 * \author	Raphael Lehmann
 */
template<class SpiMaster, class Cs>
class Adis16470 : public adis16470,
				  public modm::SpiDevice<SpiMaster>,
				  protected modm::NestedResumable<2>
{
public:
	/**
	 * @brief Initialize
	 *
	 * Sets used pins as output.
	 *
	 * SPI must be initialized by the user!
	 * @warning The SPI frequency must not exceed 2 MHz for this chip,
	 * or 1 MHz with burst mode.
	 */
	modm::ResumableResult<void>
	initialize();

	/**
	 * @brief Read a single register.
	 *
	 * @param reg The register to be read
	 * @return The register value in case of a read access, or std::nullopt if
	 * an error occured, e.g. if some register access is not permitted.
	 */
	modm::ResumableResult<std::optional<uint16_t>>
	readRegister(Register reg);

	/**
	 * @brief Read the DIAG_STAT register
	 *
	 * @return The register value
	 */
	modm::ResumableResult<modm::adis16470::DiagStat_t>
	readDiagStat();

	/**
	 * @brief Read the MSC_CTRL register
	 *
	 * @return The register value
	 */
	modm::ResumableResult<modm::adis16470::MscCtrl_t>
	readMscCtrl();

	/**
	 * @brief Write a single register.
	 *
	 * @param reg The register to be written
	 * @param value The value to be written to the register.
	 * @return False in case of any error, e.g. if some register acces is not
	 * permitted.
	 */
	modm::ResumableResult<bool>
	writeRegister(Register reg, uint16_t value);

	/**
	 * @brief Write the MSC_CTRL register
	 *
	 * @param value The value to be written to the MSC_CTRL register.
	 */
	modm::ResumableResult<void>
	writeMscCtrl(modm::adis16470::MscCtrl_t value);

	/**
	 * @brief Write the MSC_CTRL register
	 *
	 * @param value The value to be written to the MSC_CTRL register.
	 */
	modm::ResumableResult<void>
	writeGlobCmd(modm::adis16470::GlobCmd_t value);

	/**
	 * @brief Set the desired data output frequency from 1Hz to 2kHz
	 *
	 * @tparam frequency: desired frequency
	 * @tparam tolerance: acceptable tolerance, default 1%
	 */
	template<frequency_t frequency, percent_t tolerance=pct(1)>
	modm::ResumableResult<void>
	setDataOutputFrequency();

	/**
	 * @brief Read a sequence of registers.
	 * Improves SPI usage efficiency by 100% compared to `accessRegister(...)`.
	 *
	 * @param sequence All registers in the std::span will be read and the
	 * results will be stored in values.
	 * @param values Results of the read operations
	 * @return False in case of any error, e.g. if some register access is not
	 * permitted.
	 */
	modm::ResumableResult<bool>
	readRegisterSequence(std::span<const Register> sequence, std::span<uint16_t> values);

	/**
	 * @brief Read all output data registers using burst mode.
	 *
	 * In all clock modes, except when operating in scaled sync mode (Register
	 * MSC_CTRL, Bits[4:2] = 010), the burst read response includes the
	 * following registers and checksum value:
	 * DIAG_STAT, X_GYRO_OUT, Y_GYRO_OUT, Z_GYRO_OUT, X_ACCL_OUT, Y_ACCL_OUT,
	 *  Z_ACCL_OUT, TEMP_OUT, DATA_CNTR, and checksum.
	 *
	 * When operating in scaled sync mode (Register MSC_CTRL, Bits[4:2] = 010),
	 * the burst read response includes the following registers and value:
	 * DIAG_STAT, X_GYRO_OUT, Y_GYRO_OUT, Z_GYRO_OUT, X_ACCL_OUT, Y_ACCL_OUT,
	 * Z_ACCL_OUT, TEMP_OUT, TIME_STMP, and checksum.
	 *
	 * This is useful with DMA accelerated SPI to retrieve the measurement
	 * data, because it only triggers a single SPI transfer.
	 *
	 * @warning The SPI frequency must not exceed 1 MHz for this mode.
	 *
	 * @param data Reference to a array of 11 uint16_t entries, the read
	 * register values are written here.
	 * data[0] is empty and the last entry (data[10]) contains a checksum which
	 * has already been checked by this driver.
	 * @return False in case of a checksum mismatch.
	 */
	modm::ResumableResult<bool>
	readRegisterBurst(std::array<uint16_t, 11>& data);

private:
	static constexpr std::size_t bufferSize = 22;
	std::array<uint8_t, bufferSize> buffer;
	std::size_t i;
	uint16_t checksum;
	std::optional<uint16_t> tmp;

	const modm::ShortPreciseDuration tStall{std::chrono::microseconds(16)};
	modm::ShortPreciseTimeout timeout{tStall};
};


} // modm namespace

#include "adis16470_io.hpp"
#include "adis16470_impl.hpp"

#endif // MODM_ADIS16470_HPP
