/*
 * Copyright (c) 2026, Joel Schulz-Andres
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMP581_HPP
#define MODM_BMP581_HPP

#include <array>
#include <cstdint>
#include <modm/architecture/interface/register.hpp>
#include <modm/processing/fiber.hpp>
#include <optional>

#include "bmp581_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_bmp581
struct bmp581
{
	/// Chip ID value for BMP581
	static constexpr uint8_t ChipId = 0x50;

	/// Soft reset command value
	static constexpr uint8_t ResetCommand = 0xB6;

	/// I2C addresses
	static constexpr uint8_t AddressLow = 0x46;   //< SDO to GND
	static constexpr uint8_t AddressHigh = 0x47;  //< SDO to VDDIO

	/// Register addresses
	enum class Register : uint8_t
	{
		ChipId = 0x01,
		RevId = 0x02,
		ChipStatus = 0x11,
		DriveConfig = 0x13,
		IntConfig = 0x14,
		IntSource = 0x15,
		FifoConfig = 0x16,
		FifoCount = 0x17,
		FifoSel = 0x18,
		TempDataXlsb = 0x1D,
		TempDataLsb = 0x1E,
		TempDataMsb = 0x1F,
		PressDataXlsb = 0x20,
		PressDataLsb = 0x21,
		PressDataMsb = 0x22,
		IntStatus = 0x27,
		Status = 0x28,
		FifoData = 0x29,
		NvmAddr = 0x2B,
		NvmDataLsb = 0x2C,
		NvmDataMsb = 0x2D,
		DspConfig = 0x30,
		DspIir = 0x31,
		OorThrPLsb = 0x32,
		OorThrPMsb = 0x33,
		OorRange = 0x34,
		OorConfig = 0x35,
		OsrConfig = 0x36,
		OdrConfig = 0x37,
		OsrEff = 0x38,
		Cmd = 0x7E,
	};

	/// Power modes
	enum class PowerMode : uint8_t
	{
		Standby = 0b00,     //< Standby mode (default after reset)
		Normal = 0b01,      //< Normal mode (continuous measurement)
		Forced = 0b10,      //< Forced mode (single measurement)
		Continuous = 0b11,  //< Continuous mode
	};

	/// Output data rate configuration
	enum class Odr : uint8_t
	{
		Hz240 = 0x00,
		Hz218 = 0x01,
		Hz199 = 0x02,
		Hz179 = 0x03,
		Hz160 = 0x04,
		Hz149 = 0x05,
		Hz140 = 0x06,
		Hz129 = 0x07,
		Hz120 = 0x08,
		Hz110 = 0x09,
		Hz100 = 0x0A,
		Hz89 = 0x0B,
		Hz80 = 0x0C,
		Hz70 = 0x0D,
		Hz60 = 0x0E,
		Hz50 = 0x0F,
		Hz45 = 0x10,
		Hz40 = 0x11,
		Hz35 = 0x12,
		Hz30 = 0x13,
		Hz25 = 0x14,
		Hz20 = 0x15,
		Hz15 = 0x16,
		Hz10 = 0x17,
		Hz5 = 0x18,
		Hz4 = 0x19,
		Hz3 = 0x1A,
		Hz2 = 0x1B,
		Hz1 = 0x1C,
		Hz0_5 = 0x1D,
		Hz0_25 = 0x1E,
		Hz0_125 = 0x1F,
	};

	/// Oversampling rate configuration
	enum class Osr : uint8_t
	{
		X1 = 0b000,    //< No oversampling
		X2 = 0b001,    //< 2x oversampling
		X4 = 0b010,    //< 4x oversampling
		X8 = 0b011,    //< 8x oversampling
		X16 = 0b100,   //< 16x oversampling
		X32 = 0b101,   //< 32x oversampling
		X64 = 0b110,   //< 64x oversampling
		X128 = 0b111,  //< 128x oversampling
	};

	/// IIR filter coefficient
	enum class IirFilter : uint8_t
	{
		Bypass = 0b000,   //< No filtering
		Coef1 = 0b001,    //< Coefficient 1
		Coef3 = 0b010,    //< Coefficient 3
		Coef7 = 0b011,    //< Coefficient 7
		Coef15 = 0b100,   //< Coefficient 15
		Coef31 = 0b101,   //< Coefficient 31
		Coef63 = 0b110,   //< Coefficient 63
		Coef127 = 0b111,  //< Coefficient 127
	};

	/// Status register (0x28) bit definitions
	enum class Status : uint8_t
	{
		CoreReady = Bit0,         //< Digital core domain accessible
		NvmReady = Bit1,          //< Ready for NVM operations
		NvmError = Bit2,          //< NVM error detected
		NvmCmdError = Bit3,       //< Boot command error
		BootErrCorrected = Bit4,  //< ECC error corrected during boot
		CrackPass = Bit7,         //< Crack check passed
	};
	MODM_FLAGS8(Status);

	/// Interrupt status register (0x27) bit definitions
	enum class IntStatus : uint8_t
	{
		DataReady = Bit0,      //< Data ready interrupt
		FifoFull = Bit1,       //< FIFO full interrupt
		FifoThreshold = Bit2,  //< FIFO threshold interrupt
		OorPressure = Bit3,    //< Out-of-range pressure interrupt
		PowerOnReset = Bit4,   //< Power-on reset detected
	};
	MODM_FLAGS8(IntStatus);

	/// Interrupt source enable register (0x15) bit definitions
	enum class IntSource : uint8_t
	{
		DataReadyEnable = Bit0,      //< Enable data ready interrupt
		FifoFullEnable = Bit1,       //< Enable FIFO full interrupt
		FifoThresholdEnable = Bit2,  //< Enable FIFO threshold interrupt
		OorPressureEnable = Bit3,    //< Enable OOR pressure interrupt
	};
	MODM_FLAGS8(IntSource);

	/// Interrupt configuration register (0x14) bit definitions
	enum class IntConfig : uint8_t
	{
		Mode = Bit0,       //< 0: Pulsed, 1: Latched
		Polarity = Bit1,   //< 0: Active low, 1: Active high
		OpenDrain = Bit2,  //< 0: Push-pull, 1: Open-drain
		Enable = Bit3,     //< Enable interrupt output
	};
	MODM_FLAGS8(IntConfig);

	/// OSR configuration register (0x36) bit definitions
	enum class OsrConfig : uint8_t
	{
		OsrT0 = Bit0,    //< Temperature OSR bit 0
		OsrT1 = Bit1,    //< Temperature OSR bit 1
		OsrT2 = Bit2,    //< Temperature OSR bit 2
		OsrP0 = Bit3,    //< Pressure OSR bit 0
		OsrP1 = Bit4,    //< Pressure OSR bit 1
		OsrP2 = Bit5,    //< Pressure OSR bit 2
		PressEn = Bit6,  //< Pressure measurement enable
	};
	MODM_FLAGS8(OsrConfig);

	/// Configuration types for OSR settings
	typedef Configuration<OsrConfig_t, Osr, (Bit2 | Bit1 | Bit0), 0> TemperatureOsr;
	typedef Configuration<OsrConfig_t, Osr, (Bit2 | Bit1 | Bit0), 3> PressureOsr;

	/// ODR configuration register (0x37) bit definitions
	enum class OdrConfig : uint8_t
	{
		Mode0 = Bit0,    //< Power mode bit 0
		Mode1 = Bit1,    //< Power mode bit 1
		Odr0 = Bit2,     //< ODR bit 0
		Odr1 = Bit3,     //< ODR bit 1
		Odr2 = Bit4,     //< ODR bit 2
		Odr3 = Bit5,     //< ODR bit 3
		Odr4 = Bit6,     //< ODR bit 4
		DeepDis = Bit7,  //< Disable deep standby
	};
	MODM_FLAGS8(OdrConfig);

	/// Configuration types for ODR settings
	typedef Configuration<OdrConfig_t, PowerMode, (Bit1 | Bit0), 0> PowerMode_t;
	typedef Configuration<OdrConfig_t, Odr, (Bit6 | Bit5 | Bit4 | Bit3 | Bit2), 2> Odr_t;

	/// DSP configuration register (0x30) bit definitions
	enum class DspConfig : uint8_t
	{
		CompPtEn = Bit0,        //< Enable pressure/temperature compensation
		CompPtSel = Bit1,       //< Compensation source selection
		IirFlushForced = Bit2,  //< IIR flush forced enable
		ShdwSelIir_T = Bit3,    //< IIR temperature shadow selection
		FifoSelIir_T = Bit4,    //< IIR temperature FIFO selection
		ShdwSelIir_P = Bit5,    //< IIR pressure shadow selection
		FifoSelIir_P = Bit6,    //< IIR pressure FIFO selection
		OorSelIir_P = Bit7,     //< IIR pressure OOR selection
	};
	MODM_FLAGS8(DspConfig);

	/// DSP IIR register (0x31) bit definitions
	enum class DspIir : uint8_t
	{
		SetIir_T0 = Bit0,  //< Temperature IIR coefficient bit 0
		SetIir_T1 = Bit1,  //< Temperature IIR coefficient bit 1
		SetIir_T2 = Bit2,  //< Temperature IIR coefficient bit 2
		SetIir_P0 = Bit3,  //< Pressure IIR coefficient bit 0
		SetIir_P1 = Bit4,  //< Pressure IIR coefficient bit 1
		SetIir_P2 = Bit5,  //< Pressure IIR coefficient bit 2
	};
	MODM_FLAGS8(DspIir);

	/// Configuration types for IIR filter settings
	typedef Configuration<DspIir_t, IirFilter, (Bit2 | Bit1 | Bit0), 0> TemperatureIir;
	typedef Configuration<DspIir_t, IirFilter, (Bit2 | Bit1 | Bit0), 3> PressureIir;

	/// Measurement data container
	struct Data
	{
		/// Get temperature in degrees Celsius
		float
		getTemperature() const
		{
			int32_t raw = (static_cast<int32_t>(rawTemp[2]) << 16) |
						  (static_cast<int32_t>(rawTemp[1]) << 8) |
						  (static_cast<int32_t>(rawTemp[0]));
			// Sign-extend 24-bit signed value to 32-bit
			if (raw & 0x00800000) { raw |= 0xFF000000; }
			return static_cast<float>(raw) / 65536.0f;
		}

		/// Get pressure in Pascals
		float
		getPressure() const
		{
			uint32_t raw = (static_cast<uint32_t>(rawPress[2]) << 16) |
						   (static_cast<uint32_t>(rawPress[1]) << 8) |
						   (static_cast<uint32_t>(rawPress[0]));
			return static_cast<float>(raw) / 64.0f;
		}

		/// Get pressure in hectopascals (hPa) / millibars (mbar)
		float
		getPressureHpa() const
		{
			return getPressure() / 100.0f;
		}

		/// Raw temperature data (XLSB, LSB, MSB)
		std::array<uint8_t, 3> rawTemp{};

		/// Raw pressure data (XLSB, LSB, MSB)
		std::array<uint8_t, 3> rawPress{};
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Register reg)
	{
		return static_cast<uint8_t>(reg);
	}
	/// @endcond
};

/**
 * Bosch BMP581 Barometric Pressure Sensor Driver
 *
 * The BMP581 is a high-performance barometric pressure sensor with:
 * - Absolute pressure range: 30 to 125 kPa
 * - Pressure resolution: 1/64 Pa
 * - Temperature resolution: 1/65536 C
 * - Low power consumption
 * - FIFO buffer (32 frames)
 * - Interrupt support
 *
 * Unlike older BMP sensors like the BMP085, the BMP581 outputs calibrated data directly,
 * which means no calibration coefficient compensation needs to be done in software.
 *
 * @tparam Transport Transport layer (use @ref Bmp581I2cTransport or @ref Bmp581SpiTransport)
 * @ingroup modm_driver_bmp581
 */
template<Bmp581Transport Transport>
class Bmp581 : public bmp581, public Transport
{
public:
	/// @arg transportArgs Arguments to transport layer.
	///                    Pass address for I2C, none for SPI.
	template<typename... Args>
	Bmp581(Args... transportArgs);

	/// Initialize device. Call before any other member function.
	/// @return true on success, false on error
	bool
	initialize();

	/// Perform soft reset and wait for device ready
	/// @return true on success, false on error
	bool
	reset();

	/// Read chip ID register
	/// @return chip ID value, or std::nullopt on error
	std::optional<uint8_t>
	readChipId();

	/// Read status register
	/// @return status flags, or std::nullopt on error
	std::optional<Status_t>
	readStatus();

	/// Read interrupt status register (clears latched interrupts)
	/// @return interrupt status flags, or std::nullopt on error
	std::optional<IntStatus_t>
	readIntStatus();

	/// Configure power mode
	/// @return true on success, false on error
	bool
	setPowerMode(PowerMode mode);

	/// Configure output data rate
	/// @return true on success, false on error
	bool
	setOdr(Odr odr);

	/// Configure oversampling for pressure and temperature
	/// @param pressOsr Pressure oversampling rate
	/// @param tempOsr Temperature oversampling rate
	/// @param enablePressure Enable pressure measurement (default true)
	/// @return true on success, false on error
	bool
	setOversampling(Osr pressOsr, Osr tempOsr, bool enablePressure = true);

	/// Configure IIR filter coefficients
	/// @param pressIir Pressure IIR filter coefficient
	/// @param tempIir Temperature IIR filter coefficient
	/// @return true on success, false on error
	bool
	setIirFilter(IirFilter pressIir, IirFilter tempIir);

	/// Configure interrupt output
	/// @param config Interrupt configuration flags
	/// @return true on success, false on error
	bool
	setIntConfig(IntConfig_t config);

	/// Configure interrupt sources
	/// @param sources Interrupt source enable flags
	/// @return true on success, false on error
	bool
	setIntSource(IntSource_t sources);

	/// Read temperature and pressure data
	/// @param data Reference to Data struct to fill
	/// @return true on success, false on error
	bool
	readData(Data& data);

	/// Read temperature only
	/// @return Temperature in Celsius, or std::nullopt on error
	std::optional<float>
	readTemperature();

	/// Read pressure only
	/// @return Pressure in Pascals, or std::nullopt on error
	std::optional<float>
	readPressure();

	/// Check if data is ready
	/// @return true if data ready, false otherwise
	bool
	isDataReady();

private:
	std::optional<uint8_t>
	readRegister(Register reg);

	bool
	writeRegister(Register reg, uint8_t value);

	bool
	updateRegister(Register reg, uint8_t mask, uint8_t value);
};

}  // namespace modm

#include "bmp581_impl.hpp"

#endif  // MODM_BMP581_HPP
