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

#include <cstdint>
#ifndef MODM_BMI088_HPP
#error "Don't include this file directly, use 'bmi088.hpp' instead!"
#endif

#include <modm/math/utils.hpp>

namespace modm
{

template<Bmi088Transport Transport>
template<typename... Args>
Bmi088<Transport>::Bmi088(Args... transportArgs)
	: Transport{transportArgs...}
{
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::initialize(bool runSelfTest)
{
	Transport::initialize();

	if (!checkChipId() or !reset() or !enableAccelerometer()) {
		return false;
	}

	if (runSelfTest and !selfTest()) {
		return false;
	}

	timerWait();
	const bool ok = this->writeRegister(GyroRegister::InterruptControl,
										uint8_t(GyroInterruptControl::DataReady));
	timer_.restart(WriteTimeout);
	return ok;
}

template<Bmi088Transport Transport>
std::optional<bmi088::AccData>
Bmi088<Transport>::readAccData()
{
	const auto data = this->readRegisters(AccRegister::DataXLow, 6);

	if (data.empty()) {
		return {};
	}

	return AccData {
		.raw = Vector3i(
			data[0] | data[1] << 8,
			data[2] | data[3] << 8,
			data[4] | data[5] << 8
		),
		.range = accRange_
	};
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::readAccDataReady()
{
	const auto value = readRegister(AccRegister::Status).value_or(0);
	return value & uint8_t(AccStatus::DataReady);
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::clearAccDataReadyInterrupt()
{
	const auto result = readRegister(AccRegister::InterruptStatus).value_or(0);
	return result & uint8_t(AccStatus::DataReady);
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setAccRate(AccRate config)
{
	timerWait();
	const bool ok = this->writeRegister(AccRegister::Config, uint8_t(config));
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setAccRange(AccRange range)
{
	timerWait();
	const bool ok = this->writeRegister(AccRegister::Range, uint8_t(range));
	timer_.restart(ResetTimeout);
	if (ok) {
		accRange_ = range;
		return true;
	}
	return false;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setAccInt1GpioConfig(AccGpioConfig_t config)
{
	timerWait();
	const bool ok = this->writeRegister(AccRegister::Int1Control, config.value);
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setAccInt2GpioConfig(AccGpioConfig_t config)
{
	timerWait();
	const bool ok = this->writeRegister(AccRegister::Int2Control, config.value);
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setAccGpioMap(AccGpioMap_t map)
{
	timerWait();
	const bool ok = this->writeRegister(AccRegister::IntMap, map.value);
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
std::optional<bmi088::GyroData>
Bmi088<Transport>::readGyroData()
{
	const auto data = this->readRegisters(GyroRegister::RateXLow, 6);

	if (data.empty()) {
		return {};
	}

	return GyroData {
		.raw = Vector3i(
			data[0] | data[1] << 8,
			data[2] | data[3] << 8,
			data[4] | data[5] << 8
		),
		.range = gyroRange_
	};
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::readGyroDataReady()
{
	const auto value = readRegister(GyroRegister::InterruptStatus).value_or(0);
	return bool(GyroStatus_t{value} & GyroStatus::DataReady);
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setGyroRate(GyroRate rate)
{
	timerWait();
	const bool ok = this->writeRegister(GyroRegister::Bandwidth, uint8_t(rate));
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setGyroRange(GyroRange range)
{
	timerWait();
	const bool ok = this->writeRegister(GyroRegister::Range, uint8_t(range));
	timer_.restart(ResetTimeout);
	if (ok) {
		gyroRange_ = range;
		return true;
	}
	return false;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setGyroGpioConfig(GyroGpioConfig_t config)
{
	timerWait();
	const bool ok = this->writeRegister(GyroRegister::Int3Int4Conf, config.value);
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::setGyroGpioMap(GyroGpioMap_t map)
{
	timerWait();
	const bool ok = this->writeRegister(GyroRegister::Int3Int4Map, map.value);
	timer_.restart(ResetTimeout);
	return ok;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::checkChipId()
{
	const std::optional gyroId = readRegister(GyroRegister::ChipId);
	const std::optional accId = readRegister(AccRegister::ChipId);
	const bool gyroIdValid = gyroId.value_or(0) == GyroChipId;
	const bool accIdValid = accId.value_or(0) == AccChipId;
	return gyroIdValid and accIdValid;
}

template<Bmi088Transport Transport>
void
Bmi088<Transport>::timerWait()
{
	while (timer_.isArmed()) {
		modm::fiber::yield();
	}
}

template<Bmi088Transport Transport>
std::optional<uint8_t>
Bmi088<Transport>::readRegister(auto reg)
{
	const auto data = this->readRegisters(reg, 1);
	if (data.empty()) {
		return std::nullopt;
	} else {
		return data[0];
	}
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::reset()
{
	const bool gyroOk = this->writeRegister(GyroRegister::SoftReset, ResetCommand);
	const bool accOk = this->writeRegister(AccRegister::SoftReset, ResetCommand);

	if (!gyroOk or !accOk) {
		return false;
	}
	accRange_ = AccRange::Range6g;
	gyroRange_ = GyroRange::Range2000dps;

	timer_.restart(ResetTimeout);
	timerWait();

	// required to switch the accelerometer to SPI mode if SPI is used
	Transport::initialize();

	return true;
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::selfTest()
{
	bool ok = setAccRange(AccRange::Range24g);
	ok &= setAccRate(AccRate::Rate1600Hz_Bw280Hz);
	if (!ok) {
		return false;
	}
	timer_.restart(2ms);
	timerWait();
	ok = this->writeRegister(AccRegister::SelfTest, uint8_t(AccSelfTest::Positive));
	if (!ok) {
		return false;
	}
	timer_.restart(50ms);
	timerWait();
	const auto positiveData = readAccData();
	if (!positiveData) {
		return false;
	}
	ok = this->writeRegister(AccRegister::SelfTest, uint8_t(AccSelfTest::Negative));
	if (!ok) {
		return false;
	}
	timer_.restart(50ms);
	timerWait();
	const auto negativeData = readAccData();
	if (!negativeData) {
		return false;
	}
	const Vector3f diff = positiveData->getFloat() - negativeData->getFloat();
	const bool accOk = (diff[0] >= 1000.f) and (diff[1] >= 1000.f) and (diff[2] >= 500.f);
	this->writeRegister(AccRegister::SelfTest, uint8_t(AccSelfTest::Off));
	if (!accOk) {
		return false;
	}
	ok = this->writeRegister(GyroRegister::SelfTest, uint8_t(GyroSelfTest::Trigger));
	if (!ok) {
		return false;
	}
	timer_.restart(30ms);
	timerWait();
	const auto gyroTest = GyroSelfTest_t(readRegister(GyroRegister::SelfTest).value_or(0));
	if (gyroTest != (GyroSelfTest::Ready | GyroSelfTest::RateOk)) {
		return false;
	}
	return reset() and enableAccelerometer();
}

template<Bmi088Transport Transport>
bool
Bmi088<Transport>::enableAccelerometer()
{
	timerWait();
	bool ok = this->writeRegister(AccRegister::PowerConfig, uint8_t(AccPowerConf::Active));
	timer_.restart(AccSuspendTimeout);
	if (!ok) {
		return false;
	}
	timerWait();

	ok = this->writeRegister(AccRegister::PowerControl, uint8_t(AccPowerControl::On));
	timer_.restart(WriteTimeout);
	return ok;
}

inline Vector3f
bmi088::AccData::getFloat() const
{
	const float factor = (1500 << (int(range) + 1)) * (1 / 32768.f);
	return Vector3f {
		raw[0] * factor,
		raw[1] * factor,
		raw[2] * factor
	};
}

inline Vector3f
bmi088::GyroData::getFloat() const
{
	const float factor = (2000 >> int(range)) * (1 / 32768.f);
	return Vector3f {
		raw[0] * factor,
		raw[1] * factor,
		raw[2] * factor
	};
}

} // namespace modm
