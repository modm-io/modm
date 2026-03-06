/*
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMI270_TRANSPORT_HPP
#define MODM_BMI270_TRANSPORT_HPP

#include <array>
#include <cstdint>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <span>

namespace modm
{

/// @ingroup modm_driver_bmi270
struct Bmi270TransportBase
{
	enum class Register : uint8_t
	{
		ChipId = 0x00,
		Error = 0x02,
		Status = 0x03,
		AccDataXLow = 0x0C,
		GyroDataXLow = 0x12,
		SensorTime0 = 0x18,
		InterruptStatus0 = 0x1C,
		InterruptStatus1 = 0x1D,
		InternalStatus = 0x21,
		Temperature0 = 0x22,
		FifoLength0 = 0x24,
		FifoData = 0x26,
		FeatPage = 0x2F,
		Features = 0x30,
		AccConf = 0x40,
		AccRange = 0x41,
		GyroConf = 0x42,
		GyroRange = 0x43,
		FifoDowns = 0x45,
		FifoWtm0 = 0x46,
		FifoConfig0 = 0x48,
		ErrRegMask = 0x52,
		Int1IoCtrl = 0x53,
		Int2IoCtrl = 0x54,
		IntLatch = 0x55,
		IntMapData = 0x58,
		InitControl = 0x59,
		InitAddress0 = 0x5B,
		InitAddress1 = 0x5C,
		InitData = 0x5E,
		InternalError = 0x5F,
		AuxIfTrim = 0x68,
		GyroCrtConf = 0x69,
		NvmConf = 0x6A,
		IfConf = 0x6B,
		Drv = 0x6C,
		NvConf = 0x70,
		Offset0 = 0x71,
		Offset3 = 0x74,
		Offset6 = 0x77,
		GyroUserGain0 = 0x78,
		PowerConf = 0x7C,
		PowerCtrl = 0x7D,
		Command = 0x7E
	};

	static constexpr uint8_t MaxRegisterSequence{32};
};

/// @ingroup modm_driver_bmi270
template<typename T>
concept Bmi270Transport =
	requires(T& transport, Bmi270TransportBase::Register reg, uint8_t count, uint8_t data,
			 const std::array<uint8_t, 2>& values, std::array<uint8_t, 4>& fifoData) {
		{ transport.initialize() };
		{ transport.readRegisters(reg, count) } -> std::same_as<std::span<uint8_t>>;
		{ transport.writeRegister(reg, data) } -> std::same_as<bool>;
		{ transport.writeRegisters(reg, std::span{values}) } -> std::same_as<bool>;
		{ transport.readFifoData(std::span<uint8_t>{fifoData}) } -> std::same_as<bool>;
	};

/**
 * BMI270 SPI transport. Pass as template parameter to Bmi270 driver class.
 *
 * @tparam SpiMaster SPI master the device is connected to
 * @tparam Cs chip-select GPIO
 * @ingroup modm_driver_bmi270
 */
template<typename SpiMaster, typename Cs>
class Bmi270SpiTransport : public Bmi270TransportBase, public SpiDevice<SpiMaster>
{
public:
	Bmi270SpiTransport() = default;

	Bmi270SpiTransport(const Bmi270SpiTransport&) = delete;

	Bmi270SpiTransport&
	operator=(const Bmi270SpiTransport&) = delete;

	void
	initialize();

	std::span<uint8_t>
	readRegisters(Register startReg, uint8_t count);

	bool
	writeRegister(Register reg, uint8_t data);

	bool
	writeRegisters(Register startReg, std::span<const uint8_t> data);

	bool
	readFifoData(std::span<uint8_t> data);

private:
	static constexpr uint8_t ReadFlag{0x80};
	std::array<uint8_t, MaxRegisterSequence + 2> rxBuffer_{};
	std::array<uint8_t, MaxRegisterSequence + 2> txBuffer_{};
};

/**
 * BMI270 I2C transport. Pass as template parameter to Bmi270 driver class.
 *
 * @tparam I2cMaster I2C master the device is connected to
 * @ingroup modm_driver_bmi270
 */
template<typename I2cMaster>
class Bmi270I2cTransport : public Bmi270TransportBase, public I2cDevice<I2cMaster>
{
public:
	explicit Bmi270I2cTransport(uint8_t address = 0x68);

	Bmi270I2cTransport(const Bmi270I2cTransport&) = delete;

	Bmi270I2cTransport&
	operator=(const Bmi270I2cTransport&) = delete;

	void
	initialize();

	std::span<uint8_t>
	readRegisters(Register startReg, uint8_t count);

	bool
	writeRegister(Register reg, uint8_t data);

	bool
	writeRegisters(Register startReg, std::span<const uint8_t> data);

	bool
	readFifoData(std::span<uint8_t> data);

private:
	std::array<uint8_t, MaxRegisterSequence + 1> buffer_{};
};

}  // namespace modm

#include "bmi270_transport_impl.hpp"

#endif  // MODM_BMI270_TRANSPORT_HPP
