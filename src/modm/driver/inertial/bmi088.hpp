/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMI088_HPP
#define MODM_BMI088_HPP

#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/timer/timeout.hpp>
#include <modm/math/geometry/vector3.hpp>
#include "bmi088_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_bmi088
struct bmi088
{
	enum class AccRange : uint8_t
	{
		Range3g  = 0x00, //< +- 3g
		Range6g  = 0x01, //< +- 6g (default)
		Range12g = 0x02, //< +- 12g
		Range24g = 0x03  //< +- 24g
	};

	enum class GyroRange : uint8_t
	{
		Range2000dps = 0x00, //< +- 2000 deg/s (default)
		Range1000dps = 0x01, //< +- 1000 deg/s
		Range500dps  = 0x02, //< +- 500 deg/s
		Range250dps  = 0x03, //< +- 250 deg/s
		Range125dps  = 0x04  //< +- 125 deg/s
	};

	struct AccData
	{
		/// acceleration in milli-g
		Vector3f getFloat() const;

		Vector3i raw;
		AccRange range;
	};

	struct GyroData
	{
		/// angular rate in deg/s
		Vector3f getFloat() const;

		Vector3i raw;
		GyroRange range;
	};

	/// Accelerometer data rate and filter bandwidth config
	enum class AccRate : uint8_t
	{
		Rate12Hz_Bw5Hz     = 0x5 | 0xA0,
		Rate12Hz_Bw2Hz     = 0x5 | 0x90,
		Rate12Hz_Bw1Hz     = 0x5 | 0x80,
		Rate25Hz_Bw10Hz    = 0x6 | 0xA0,
		Rate25Hz_Bw5Hz     = 0x6 | 0x90,
		Rate25Hz_Bw3Hz     = 0x6 | 0x80,
		Rate50Hz_Bw20Hz    = 0x7 | 0xA0,
		Rate50Hz_Bw9Hz     = 0x7 | 0x90,
		Rate50Hz_Bw5Hz     = 0x7 | 0x80,
		Rate100Hz_Bw40Hz   = 0x8 | 0xA0,
		Rate100Hz_Bw19Hz   = 0x8 | 0x90,
		Rate100Hz_Bw10Hz   = 0x8 | 0x80,
		Rate200Hz_Bw80Hz   = 0x9 | 0xA0,
		Rate200Hz_Bw38Hz   = 0x9 | 0x90,
		Rate200Hz_Bw20Hz   = 0x9 | 0x80,
		Rate400Hz_Bw145Hz  = 0xA | 0xA0,
		Rate400Hz_Bw75Hz   = 0xA | 0x90,
		Rate400Hz_Bw40Hz   = 0xA | 0x80,
		Rate800Hz_Bw230Hz  = 0xB | 0xA0,
		Rate800Hz_Bw140Hz  = 0xB | 0x90,
		Rate800Hz_Bw80Hz   = 0xB | 0x80,
		Rate1600Hz_Bw280Hz = 0xC | 0xA0,
		Rate1600Hz_Bw234Hz = 0xC | 0x90,
		Rate1600Hz_Bw145Hz = 0xC | 0x80
	};

	enum class AccGpioConfig : uint8_t
	{
		ActiveHigh = Bit1,		//< Set GPIO polarity
		OpenDrain  = Bit2,		//< Configure open-drain mode, push-pull if not set
		EnableOutput = Bit3,	//< Activate GPIO output function
		EnableInput = Bit4		//< Activate GPIO intput function
	};
	MODM_FLAGS8(AccGpioConfig);

	/// Configuration flags to map accelerometer interrupt signal to GPIO
	enum class AccGpioMap : uint8_t
	{
		Int1FifoFull = Bit0,
		Int1FifoWatermark = Bit1,
		Int1DataReady = Bit2,
		Int2FifoFull = Bit4,
		Int2FifoWatermark = Bit5,
		Int2DataReady = Bit6
	};
	MODM_FLAGS8(AccGpioMap);

	enum class AccStatus : uint8_t
	{
		DataReady = Bit7
	};
	MODM_FLAGS8(AccStatus);

	enum class AccPowerConf : uint8_t
	{
		Active = 0x00,
		Suspend = 0x03
	};

	enum class AccPowerControl : uint8_t
	{
		Off = 0x00,
		On = 0x04
	};

	enum class AccSelfTest : uint8_t
	{
		Off = 0x00,
		Positive = 0x0D,
		Negative = 0x09
	};

	/// Gyroscope data rate and filter bandwidth config
	enum class GyroRate : uint8_t
	{
		Rate2000Hz_Bw532Hz = 0x00,
		Rate2000Hz_Bw230Hz = 0x01,
		Rate1000Hz_Bw116Hz = 0x02,
		Rate400Hz_Bw47Hz   = 0x03,
		Rate200Hz_Bw23Hz   = 0x04,
		Rate100Hz_Bw12Hz   = 0x05,
		Rate200Hz_Bw64Hz   = 0x06,
		Rate100Hz_Bw32Hz   = 0x07
	};

	enum class GyroGpioConfig : uint8_t
	{
		Int3ActiveHigh = Bit0,
		Int3OpenDrain  = Bit1,
		Int4ActiveHigh = Bit2,
		Int4OpenDrain  = Bit3
	};
	MODM_FLAGS8(GyroGpioConfig);

	enum class GyroGpioMap : uint8_t
	{
		Int3DataReady = Bit0,
		Int3FifoInterrupt = Bit2,
		Int4FifoInterrupt = Bit5,
		Int4DataReady = Bit7
	};
	MODM_FLAGS8(GyroGpioMap);

	enum class GyroSelfTest : uint8_t
	{
		// read-only bits
		RateOk = Bit4,
		Fail = Bit2,
		Ready = Bit1,
		// write-only bit
		Trigger = Bit0
	};
	MODM_FLAGS8(GyroSelfTest);

	enum class GyroStatus : uint8_t
	{
		DataReady = Bit7,
		FifoInterrupt = Bit4
	};
	MODM_FLAGS8(GyroStatus);

	enum class GyroInterruptControl : uint8_t
	{
		Fifo = Bit6,
		DataReady = Bit7
	};
	MODM_FLAGS8(GyroInterruptControl);
};

/**
 * Bosch BMI088 IMU
 *
 * The device contains an accelerometer and a gyroscope which operate
 * independently.
 *
 * The device supports both I2C and SPI. For applications with high data
 * rates or strict timing requirements SPI is recommended.
 *
 * The "MM Feature Set" accelerometer functions are not supported yet.
 *
 * @tparam Transport Transport layer (use @ref Bmi088SpiTransport or @ref Bmi088I2cTransport)
 * @ingroup modm_driver_bmi088
 */
template<Bmi088Transport Transport>
class Bmi088 : public bmi088, public Transport
{
public:
	/// @arg transportArgs	Arguments to transport layer.
	///						Pass addresses for I2C, none for SPI.
	template<typename... Args>
	Bmi088(Args... transportArgs);

	/// Initialize device. Call before any other member function.
	/// @return true on success, false on error
	bool
	initialize(bool runSelfTest);

	// Accelerometer functions

	std::optional<AccData>
	readAccData();

	/// Read data ready flag from ACC_STATUS register. Cleared on data read.
	bool
	readAccDataReady();

	/// Read and clear data ready flag in ACC_INT_STAT_1 register. Not cleared on data read.
	bool
	clearAccDataReadyInterrupt();

	/// @return true on success, false on error
	bool
	setAccRate(AccRate config);

	/// @return true on success, false on error
	bool
	setAccRange(AccRange range);

	/// @return true on success, false on error
	bool
	setAccInt1GpioConfig(AccGpioConfig_t config);

	/// @return true on success, false on error
	bool
	setAccInt2GpioConfig(AccGpioConfig_t config);

	/// @return true on success, false on error
	bool
	setAccGpioMap(AccGpioMap_t map);

	// Gyroscope functions

	std::optional<GyroData>
	readGyroData();

	/**
	 * Read Data ready interrupt status.
	 * @warning The interrupt is cleared automatically after 280-400 μs.
	 * @return true on success, false on error
	 */
	bool
	readGyroDataReady();

	/// @return true on success, false on error
	bool
	setGyroRate(GyroRate config);

	/// @return true on success, false on error
	bool
	setGyroRange(GyroRange range);

	/// @return true on success, false on error
	bool
	setGyroGpioConfig(GyroGpioConfig_t config);

	/// @return true on success, false on error
	bool
	setGyroGpioMap(GyroGpioMap_t map);

private:
	using AccRegister = Transport::AccRegister;
	using GyroRegister = Transport::GyroRegister;

	static constexpr std::chrono::milliseconds ResetTimeout{30};
	static constexpr std::chrono::microseconds WriteTimeout{2};
	static constexpr std::chrono::microseconds AccSuspendTimeout{450};

	static constexpr uint8_t ResetCommand{0xB6};
	static constexpr uint8_t AccChipId{0x1E};
	static constexpr uint8_t GyroChipId{0x0F};

	bool
	checkChipId();

	bool
	reset();

	bool
	selfTest();

	bool
	enableAccelerometer();

	void
	timerWait();

	std::optional<uint8_t>
	readRegister(auto reg);

	modm::PreciseTimeout timer_;
	AccRange accRange_{AccRange::Range6g};
	GyroRange gyroRange_{GyroRange::Range2000dps};
};


} // modm namespace

#include "bmi088_impl.hpp"

#endif // MODM_ADIS16470_HPP
