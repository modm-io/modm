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

#ifndef MODM_BMI088_TRANSPORT_HPP
#define MODM_BMI088_TRANSPORT_HPP

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/timer/timeout.hpp>

namespace modm
{

/// @ingroup modm_driver_bmi088
struct Bmi088TransportBase
{
	enum class
	AccRegister : uint8_t
	{
		ChipId = 0x00,
		// 0x01: reserved
		Error = 0x02,
		Status = 0x03,
		// 0x04-0x11: reserved
		DataXLow = 0x12,
		DataXHigh = 0x13,
		DataYLow = 0x14,
		DataYHigh = 0x15,
		DataZLow = 0x16,
		DataZHigh = 0x17,
		SensorTime0 = 0x18,
		SensorTime1 = 0x19,
		SensorTime2 = 0x1A,
		// 0x1B-0x1C: reserved
		InterruptStatus = 0x1D,
		// 0x1E-0x21: reserved
		TempHigh = 0x22,
		TempLow = 0x23,
		FifoLength0 = 0x24,
		FifoLength1 = 0x25,
		FifoData = 0x26,
		// 0x27-0x3F: reserved
		Config = 0x40,
		Range = 0x41,
		// 0x42-0x44: reserved
		FifoDownsampling = 0x45,
		FifoWatermark0 = 0x46,
		FifoWatermark1 = 0x47,
		FifoConfig0 = 0x48,
		FifoConfig1 = 0x49,
		// 0x4A-0x52: reserved
		Int1Control = 0x53,
		Int2Control = 0x54,
		// 0x55-0x57: reserved
		IntMap = 0x58,
		// 0x59-0x6C: reserved
		SelfTest = 0x6D,
		// 0x6E-0x7B: reserved
		PowerConfig = 0x7C,
		PowerControl = 0x7D,
		SoftReset = 0x7E
	};

	enum class
	GyroRegister : uint8_t
	{
		ChipId = 0x00,
		// 0x01: reserved
		RateXLow = 0x02,
		RateXHigh = 0x03,
		RateYLow = 0x04,
		RateYHigh = 0x05,
		RateZLow = 0x06,
		RateZHigh = 0x07,
		// 0x08-0x09: reserved
		InterruptStatus = 0x0A,
		// 0x0B-0x0D: reserved
		FifoStatus = 0x0E,
		Range = 0x0F,
		Bandwidth = 0x10,
		LowPowerMode1 = 0x11,
		// 0x12-0x13: reserved
		SoftReset = 0x14,
		InterruptControl = 0x15,
		Int3Int4Conf = 0x16,
		// 0x17: reserved
		Int3Int4Map = 0x18,
		// 0x19-0x1D: reserved
		FifoWatermark = 0x1E,
		// 0x1F-0x33: reserved
		FifoExtInt = 0x34,
		// 0x35-0x3B: reserved
		SelfTest = 0x3C,
		FifoConfig0 = 0x3D,
		FifoConfig1 = 0x3E,
		FifoData = 0x3F
	};
	static constexpr uint8_t MaxRegisterSequence{6};
};

/// @ingroup modm_driver_bmi088
template <typename T>
concept Bmi088Transport = requires(T& transport, Bmi088TransportBase::AccRegister reg1,
					     Bmi088TransportBase::GyroRegister reg2, uint8_t data)
{
	{ transport.initialize() };
	{ transport.readRegisters(reg1, /* count= */data) } -> std::same_as<std::span<uint8_t>>;
	{ transport.readRegisters(reg2, /* count= */data) } -> std::same_as<std::span<uint8_t>>;
	{ transport.writeRegister(reg1, data) } -> std::same_as<bool>;
	{ transport.writeRegister(reg2, data) } -> std::same_as<bool>;
};

/**
 * BMI088 SPI transport. Pass as template parameter to Bmi088 driver class to
 * use the driver with SPI.
 *
 * The transport class contains internal buffers which allow to read all three
 * gyroscope or accelerometer axes in a single DMA transaction.
 *
 * @tparam SpiMaster SPI master the device is connected to
 * @tparam AccCs accelerometer chip-select GPIO
 * @tparam GyroCs gyroscope chip-select GPIO
 * @ingroup modm_driver_bmi088
 */
template<typename SpiMaster, typename AccCs, typename GyroCs>
class Bmi088SpiTransport : public Bmi088TransportBase,
                           public SpiDevice<SpiMaster>
{
public:
	Bmi088SpiTransport() = default;

	Bmi088SpiTransport(const Bmi088SpiTransport&) = delete;

	Bmi088SpiTransport&
	operator=(const Bmi088SpiTransport&) = delete;

	void
	initialize();

	std::span<uint8_t>
	readRegisters(AccRegister startReg, uint8_t count);

	std::span<uint8_t>
	readRegisters(GyroRegister startReg, uint8_t count);

	bool
	writeRegister(AccRegister reg, uint8_t data);

	bool
	writeRegister(GyroRegister reg, uint8_t data);

private:
	template<typename Cs>
	std::span<uint8_t>
	readRegisters(uint8_t reg, uint8_t count, bool dummyByte);

	template<typename Cs>
	bool
	writeRegister(uint8_t reg, uint8_t data);

	static constexpr uint8_t ReadFlag{0b1000'0000};

	std::array<uint8_t, MaxRegisterSequence + 2> rxBuffer_{};
	std::array<uint8_t, MaxRegisterSequence + 2> txBuffer_{};
};


/**
 * BMI088 I2C transport. Pass as template parameter to Bmi088 driver class to
 * use the driver with I2C.
 *
 * @tparam I2cMaster I2c master the device is connected to
 * @ingroup modm_driver_bmi088
 */
template<typename I2cMaster>
class Bmi088I2cTransport : public Bmi088TransportBase, public I2cDevice<I2cMaster>
{
public:
	Bmi088I2cTransport(uint8_t accAddress, uint8_t gyroAddress);

	Bmi088I2cTransport(const Bmi088I2cTransport&) = delete;

	Bmi088I2cTransport&
	operator=(const Bmi088I2cTransport&) = delete;

	void
	initialize();

	std::span<uint8_t>
	readRegisters(AccRegister startReg, uint8_t count);

	std::span<uint8_t>
	readRegisters(GyroRegister startReg, uint8_t count);

	bool
	writeRegister(AccRegister reg, uint8_t data);

	bool
	writeRegister(GyroRegister reg, uint8_t data);

private:
	std::span<uint8_t>
	readRegisters(uint8_t reg, uint8_t count);

	bool
	writeRegister(uint8_t reg, uint8_t data);

	uint8_t accAddress_;
	uint8_t gyroAddress_;
	std::array<uint8_t, MaxRegisterSequence> buffer_{};
};

} // modm namespace

#include "bmi088_transport_impl.hpp"

#endif // MODM_BMI088_TRANSPORT_HPP
