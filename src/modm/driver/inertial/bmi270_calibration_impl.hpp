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

#include <algorithm>
#ifndef MODM_BMI270_HPP
#error "Don't include this file directly, use 'bmi270.hpp' instead!"
#endif

namespace modm
{

template<Bmi270Transport Transport>
std::optional<bmi270::GyroCrtConfig>
Bmi270<Transport>::getGyroCrtConfig()
{
	const auto value = readRegister(Register::GyroCrtConf);
	if (!value) { return {}; }

	return GyroCrtConfig{
		.running = bool(*value & 0x04),
		.readyForDownload = bool(*value & 0x08),
	};
}

template<Bmi270Transport Transport>
bool
Bmi270<Transport>::setGyroCrtConfig(GyroCrtConfig configuration)
{
	static constexpr uint8_t RunningMask{0x04};

	const auto current = readRegister(Register::GyroCrtConf);
	if (!current) { return false; }

	uint8_t value = *current;
	if (configuration.running)
	{
		value |= RunningMask;
	} else
	{
		value &= ~RunningMask;
	}

	const bool ok = this->writeRegister(Register::GyroCrtConf, value);
	modm::this_fiber::sleep_for(WriteTimeout);
	return ok;
}

template<Bmi270Transport Transport>
std::optional<bool>
Bmi270<Transport>::getNvmCrtEnabled()
{
	const auto value = readRegister(Register::NvmConf);
	if (!value) { return {}; }

	return bool(*value & 0x02);
}

template<Bmi270Transport Transport>
bool
Bmi270<Transport>::setNvmCrtEnabled(bool enable)
{
	const auto current = readRegister(Register::NvmConf);
	if (!current) { return false; }

	uint8_t value = *current;
	if (enable)
	{
		value |= 0x02;
	} else
	{
		value &= ~0x02u;
	}

	const bool ok = this->writeRegister(Register::NvmConf, value);
	modm::this_fiber::sleep_for(WriteTimeout);
	return ok;
}

template<Bmi270Transport Transport>
bool
Bmi270<Transport>::triggerGyroCrt()
{
	return sendCommand(Command::TriggerGyro);
}

template<Bmi270Transport Transport>
bool
Bmi270<Transport>::doCrt(std::span<const uint8_t> configFile)
{
	static constexpr uint8_t FeaturePage0{0};
	static constexpr uint8_t FeaturePage1{1};
	static constexpr uint8_t FeaturePageSize{16};
	static constexpr uint8_t MaxBurstLengthOffset{2};
	static constexpr uint8_t GyroSelfTestCrtOffset{3};
	static constexpr uint8_t GyroSelfTestCrtMask{0x01};
	static constexpr uint8_t AbortFeatureMask{0x02};
	static constexpr uint8_t GyroUserGainStatusOffset{8};
	static constexpr uint8_t GTriggerStatusMask{0x38};
	static constexpr uint8_t GTriggerStatusShift{3};
	static constexpr uint8_t GTriggerNoError{0};
	static constexpr uint8_t GTriggerPreconditionError{1};
	static constexpr uint8_t GTriggerDownloadError{2};
	static constexpr uint8_t GTriggerAbortError{3};
	static constexpr uint8_t CrtReadyRetryCount{100};
	static constexpr uint8_t CrtRunningRetryCount{200};
	static constexpr std::chrono::microseconds CrtReadyDelay{2000};
	static constexpr std::chrono::microseconds CrtRunningDelay{10000};
	static constexpr std::size_t CrtConfigStartIndex{0x1800};
	static constexpr std::size_t CrtConfigLength{2048};
	static constexpr uint16_t CrtMinBurstBytes{2};
	static constexpr uint16_t CrtMaxBurstBytes{uint16_t(255u * 2u)};
	static constexpr uint8_t GyroGainEnableMask{0x80};

	auto getMaxBurstLengthWords = [&]() -> std::optional<uint8_t> {
		std::array<uint8_t, FeaturePageSize> page{};
		if (!readFeaturePage(FeaturePage1, page)) { return {}; }
		return page[MaxBurstLengthOffset];
	};

	auto setMaxBurstLengthBytes = [&](uint16_t writeLengthBytes) -> bool {
		std::array<uint8_t, FeaturePageSize> page{};
		if (!readFeaturePage(FeaturePage1, page)) { return false; }

		uint16_t burstWords = writeLengthBytes / 2u;
		if (burstWords > 255u) { burstWords = 255u; }
		page[MaxBurstLengthOffset] = uint8_t(burstWords);
		return writeFeaturePage(FeaturePage1, page);
	};

	auto setSelfTestSelection = [&](bool selectCrt) -> bool {
		std::array<uint8_t, FeaturePageSize> page{};
		if (!readFeaturePage(FeaturePage1, page)) { return false; }

		if (selectCrt)
		{
			page[GyroSelfTestCrtOffset] |= GyroSelfTestCrtMask;
		} else
		{
			page[GyroSelfTestCrtOffset] &= ~GyroSelfTestCrtMask;
		}
		return writeFeaturePage(FeaturePage1, page);
	};

	auto setAbortFeature = [&](bool enable) -> bool {
		std::array<uint8_t, FeaturePageSize> page{};
		if (!readFeaturePage(FeaturePage1, page)) { return false; }

		if (enable)
		{
			page[GyroSelfTestCrtOffset] |= AbortFeatureMask;
		} else
		{
			page[GyroSelfTestCrtOffset] &= ~AbortFeatureMask;
		}
		return writeFeaturePage(FeaturePage1, page);
	};

	auto getGTriggerStatus = [&]() -> std::optional<uint8_t> {
		std::array<uint8_t, FeaturePageSize> page{};
		if (!readFeaturePage(FeaturePage0, page)) { return {}; }
		return uint8_t((page[GyroUserGainStatusOffset] & GTriggerStatusMask) >>
					   GTriggerStatusShift);
	};

	auto getCrtRunning = [&]() -> std::optional<bool> {
		const auto value = readRegister(Register::GyroCrtConf);
		if (!value) { return {}; }
		return bool(*value & 0x04);
	};

	auto getReadyForDownload = [&]() -> std::optional<bool> {
		const auto value = readRegister(Register::GyroCrtConf);
		if (!value) { return {}; }
		return bool(*value & 0x08);
	};

	auto setCrtRunning = [&](bool enable) -> bool {
		GyroCrtConfig config{};
		config.running = enable;
		config.readyForDownload = false;
		return setGyroCrtConfig(config);
	};

	auto waitStRunningComplete = [&]() -> bool {
		for (uint8_t retry = 0; retry < CrtRunningRetryCount; ++retry)
		{
			const auto running = getCrtRunning();
			if (!running) { return false; }
			if (!*running) { return true; }
			modm::this_fiber::sleep_for(CrtRunningDelay);
		}
		return false;
	};

	auto waitReadyForDownloadToggle = [&](bool previous) -> bool {
		bool toggled = false;
		for (uint8_t retry = 0; retry < CrtReadyRetryCount; ++retry)
		{
			const auto ready = getReadyForDownload();
			if (!ready) { return false; }
			if (*ready != previous)
			{
				toggled = true;
				break;
			}
			modm::this_fiber::sleep_for(CrtReadyDelay);
		}

		if (!toggled) { return false; }

		const auto running = getCrtRunning();
		return running.has_value() and *running;
	};

	auto processCrtDownload = [&](bool lastChunk) -> bool {
		const auto readyForDownload = getReadyForDownload();
		if (!readyForDownload) { return false; }

		if (!sendCommand(Command::TriggerGyro)) { return false; }

		if (!lastChunk and !waitReadyForDownloadToggle(*readyForDownload)) { return false; }

		return true;
	};

	auto writeInitBytes = [&](uint16_t index, std::span<const uint8_t> bytes) -> bool {
		const uint16_t wordAddress = index / 2;
		const std::array<uint8_t, 2> initAddress{
			uint8_t(wordAddress & 0x0F),
			uint8_t((wordAddress >> 4) & 0xFF),
		};

		if (!this->writeRegisters(Register::InitAddress0, std::span{initAddress})) { return false; }
		if (!this->writeRegisters(Register::InitData, bytes)) { return false; }
		modm::this_fiber::sleep_for(WriteTimeout);
		return true;
	};

	auto writeCrtConfigFile = [&](uint16_t writeLengthBytes) -> bool {
		if (writeLengthBytes < CrtMinBurstBytes) { writeLengthBytes = CrtMinBurstBytes; }
		writeLengthBytes = std::min<uint16_t>(writeLengthBytes, CrtMaxBurstBytes);
		writeLengthBytes &= ~uint16_t{1};
		if (writeLengthBytes == 0) { return false; }

		const std::size_t configEnd = CrtConfigStartIndex + CrtConfigLength;
		if (!configFile.empty() and configFile.size() < configEnd) { return false; }

		auto fillChunk = [&](std::size_t index, std::span<uint8_t> chunk) {
			if (!configFile.empty())
			{
				std::copy_n(configFile.begin() + index, chunk.size(), chunk.begin());
			} else
			{
				for (std::size_t ii = 0; ii < chunk.size(); ++ii)
				{
					chunk[ii] = bmi270_private::configuration[index + ii];
				}
			}
		};

		const uint16_t remainder = uint16_t(CrtConfigLength % writeLengthBytes);
		const std::size_t balanceEnd = configEnd - remainder;
		std::array<uint8_t, Transport::MaxRegisterSequence> chunkBuffer{};

		if (remainder == 0)
		{
			for (std::size_t index = CrtConfigStartIndex; index < configEnd;
				 index += writeLengthBytes)
			{
				const bool lastChunk = (index >= (configEnd - writeLengthBytes));
				auto chunk = std::span{chunkBuffer}.subspan(0, writeLengthBytes);
				fillChunk(index, chunk);
				if (!writeInitBytes(uint16_t(index), chunk) or !processCrtDownload(lastChunk))
				{
					return false;
				}
			}
			return true;
		}

		for (std::size_t index = CrtConfigStartIndex; index < balanceEnd; index += writeLengthBytes)
		{
			auto chunk = std::span{chunkBuffer}.subspan(0, writeLengthBytes);
			fillChunk(index, chunk);
			if (!writeInitBytes(uint16_t(index), chunk) or !processCrtDownload(false))
			{
				return false;
			}
		}

		if (!setMaxBurstLengthBytes(CrtMinBurstBytes)) { return false; }

		for (std::size_t index = balanceEnd; index < configEnd; index += CrtMinBurstBytes)
		{
			const bool lastChunk = (index >= (configEnd - CrtMinBurstBytes));
			auto chunk = std::span{chunkBuffer}.subspan(0, CrtMinBurstBytes);
			fillChunk(index, chunk);
			if (!writeInitBytes(uint16_t(index), chunk) or !processCrtDownload(lastChunk))
			{
				return false;
			}
		}

		return true;
	};

	auto evaluateCrtResult = [&](uint16_t configuredBurstBytes) -> bool {
		const auto status = getGTriggerStatus();
		if (!status) { return false; }

		switch (*status)
		{
			case GTriggerNoError:
				return setMaxBurstLengthBytes(0);

			case GTriggerDownloadError:
			case GTriggerAbortError:
				(void)setMaxBurstLengthBytes(configuredBurstBytes);
				return false;

			case GTriggerPreconditionError:
			default:
				return false;
		}
	};

	auto disableFifoSensors = [&]() -> bool {
		FifoConfiguration config{
			.stopOnFull = false,
			.timeEnable = false,
			.tagInt1 = FifoTagInterrupt::Edge,
			.tagInt2 = FifoTagInterrupt::Edge,
			.headerEnable = false,
			.auxEnable = false,
			.accEnable = false,
			.gyroEnable = false,
		};
		return setFifoConfiguration(config);
	};

	auto crtPrepareSetup = [&](PowerControl_t savedPower) -> bool {
		PowerControl_t prepPower{savedPower.value};
		prepPower.value &= ~uint8_t(PowerControl::Gyroscope);
		if (!setPowerControl(prepPower)) { return false; }

		if (!disableFifoSensors()) { return false; }

		prepPower.value |= uint8_t(PowerControl::Accelerometer);
		if (!setPowerControl(prepPower)) { return false; }

		modm::this_fiber::sleep_for(1ms);
		return setAbortFeature(false);
	};

	auto getGyroGainEnable = [&]() -> std::optional<bool> {
		const auto offset6 = readRegister(Register::Offset6);
		if (!offset6) { return {}; }
		return bool(*offset6 & GyroGainEnableMask);
	};

	auto setGyroGainEnable = [&](bool enable) -> bool {
		const auto offset6 = readRegister(Register::Offset6);
		if (!offset6) { return false; }

		uint8_t value = *offset6;
		if (enable)
		{
			value |= GyroGainEnableMask;
		} else
		{
			value &= ~GyroGainEnableMask;
		}

		const bool ok = this->writeRegister(Register::Offset6, value);
		modm::this_fiber::sleep_for(WriteTimeout);
		return ok;
	};

	const auto savedPowerControl = getPowerControl();
	const auto savedPowerConfiguration = getPowerConfiguration();
	const auto savedGyroGainEnable = getGyroGainEnable();
	if (!savedPowerControl or !savedPowerConfiguration or !savedGyroGainEnable) { return false; }

	const bool apsWasEnabled =
		bool(*savedPowerConfiguration & PowerConfiguration::AdvancedPowerSave);
	bool ok = true;
	bool restoreOk = true;

	if (apsWasEnabled) { ok &= setAdvancedPowerSave(false); }
	if (ok) { ok &= setGyroGainEnable(true); }

	if (ok)
	{
		const auto running = getCrtRunning();
		ok &= running.has_value() and !*running;
	}

	auto maxBurstLengthWords = uint8_t{0};
	if (ok)
	{
		const auto maxBurst = getMaxBurstLengthWords();
		if (!maxBurst)
		{
			ok = false;
		} else
		{
			maxBurstLengthWords = *maxBurst;
		}
	}

	if (ok) { ok &= setCrtRunning(true); }
	if (ok) { ok &= crtPrepareSetup(*savedPowerControl); }
	if (ok) { ok &= setSelfTestSelection(true); }

	uint16_t configuredBurstBytes = 0;
	if (ok and (maxBurstLengthWords == 0))
	{
		ok &= sendCommand(Command::TriggerGyro);
		ok &= waitStRunningComplete();
		if (ok) { ok &= evaluateCrtResult(0); }
	} else if (ok)
	{
		configuredBurstBytes = Transport::MaxRegisterSequence;
		if (configuredBurstBytes < CrtMinBurstBytes) { configuredBurstBytes = CrtMinBurstBytes; }
		configuredBurstBytes = std::min<uint16_t>(configuredBurstBytes, CrtMaxBurstBytes);
		if (configuredBurstBytes & 1u) { --configuredBurstBytes; }
		if (configuredBurstBytes == 0) { ok = false; }

		if (ok) { ok &= setMaxBurstLengthBytes(configuredBurstBytes); }

		bool downloadReady = false;
		if (ok)
		{
			const auto ready = getReadyForDownload();
			if (!ready)
			{
				ok = false;
			} else
			{
				downloadReady = *ready;
			}
		}
		if (ok) { ok &= sendCommand(Command::TriggerGyro); }
		if (ok) { ok &= waitReadyForDownloadToggle(downloadReady); }
		if (ok) { ok &= writeCrtConfigFile(configuredBurstBytes); }
		if (ok) { ok &= waitStRunningComplete(); }
		if (ok) { ok &= evaluateCrtResult(configuredBurstBytes); }
	}

	restoreOk &= setPowerControl(*savedPowerControl);
	restoreOk &= setGyroGainEnable(*savedGyroGainEnable);
	if (apsWasEnabled) { restoreOk &= setAdvancedPowerSave(true); }

	return ok and restoreOk;
}

template<Bmi270Transport Transport>
std::optional<bmi270::GyroUserGain>
Bmi270<Transport>::getGyroUserGain()
{
	static constexpr uint8_t UserGainMask{0x7F};

	const auto data = this->readRegisters(Register::GyroUserGain0, 3);
	if (data.empty()) { return {}; }

	return GyroUserGain{
		.x = int8_t(data[0] & UserGainMask),
		.y = int8_t(data[1] & UserGainMask),
		.z = int8_t(data[2] & UserGainMask),
	};
}

template<Bmi270Transport Transport>
bool
Bmi270<Transport>::setGyroUserGain(GyroUserGain gain)
{
	static constexpr uint8_t UserGainMask{0x7F};

	const std::array<uint8_t, 3> data{
		static_cast<uint8_t>(static_cast<uint8_t>(gain.x) & UserGainMask),
		static_cast<uint8_t>(static_cast<uint8_t>(gain.y) & UserGainMask),
		static_cast<uint8_t>(static_cast<uint8_t>(gain.z) & UserGainMask),
	};

	const bool ok = this->writeRegisters(Register::GyroUserGain0, std::span{data});
	modm::this_fiber::sleep_for(WriteTimeout);
	return ok;
}

}  // namespace modm
