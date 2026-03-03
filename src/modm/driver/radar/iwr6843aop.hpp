/*
 * Copyright (c) 2026, Joel Schulz-Andres, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IWR6843AOP_HPP
#define MODM_IWR6843AOP_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/uart.hpp>
#include <modm/processing/timer.hpp>
#include <optional>
#include <span>

namespace modm
{

/// @ingroup modm_driver_iwr6843aop
struct iwr6843aop
{
	enum class Error : uint8_t
	{
		None,
		UartError,
		Timeout,
		CommandError,
		ParseError,
		BufferOverflow,
		QueueFull
	};

	enum class RadarOutputType : uint32_t
	{
		DetectedPoints = 1,
		RangeProfile = 2,
		NoiseFloorProfile = 3,
		AzimuthStaticHeatmap = 4,
		RangeDopplerHeatmap = 5,
		PerformanceStatistics = 6,
		SideInfoDetectedPoints = 7,
		AzimuthElevationStaticHeatmap = 8,
		TemperatureStatistics = 9
	};

	struct FrameHeader
	{
		uint32_t version;
		uint32_t totalPacketLen;
		uint32_t platform;
		uint32_t frameNumber;
		uint32_t timeCpuCycles;
		uint32_t numDetectedObj;
		uint32_t numTlvs;
		uint32_t subframeNumber;
	};

	struct DetectedPoint
	{
		float x;
		float y;
		float z;
		float velocity;
	};

	struct PointSideInfo
	{
		float snr;
		float noise;
	};

	struct PointWithSideInfo
	{
		DetectedPoint point;
		std::optional<PointSideInfo> sideInfo;
	};

	struct PerformanceStatistics
	{
		uint32_t interframeProcessingTime;
		uint32_t transmitOutputTime;
		uint32_t interframeProcessingMargin;
		uint32_t interchirpProcessingMargin;
		uint32_t activeFrameCpuLoad;
		uint32_t interframeCpuLoad;
	};

	struct TemperatureStatistics
	{
		uint32_t reportValid;
		uint32_t timeStamp;
		float tempRx0;
		float tempRx1;
		float tempRx2;
		float tempRx3;
		float tempTx0;
		float tempTx1;
		float tempTx2;
		float tempPm;
		float tempDig0;
		float tempDig1;
	};

	template<std::size_t MaxPoints>
	struct Frame
	{
		Timestamp timestamp;
		FrameHeader frameHeader;
		std::array<PointWithSideInfo, MaxPoints> points;
		std::size_t pointCount;
		std::optional<PerformanceStatistics> performanceStatistics;
		std::optional<TemperatureStatistics> temperatureStatistics;
	};

	static constexpr std::array<uint8_t, 8> SyncPattern{0x02, 0x01, 0x04, 0x03,
														0x06, 0x05, 0x08, 0x07};
	static constexpr std::size_t FrameHeaderLength{32};
	static constexpr std::size_t TlvHeaderLength{8};
	static constexpr std::size_t PointStructSize{16};
	static constexpr std::size_t SideInfoStructSize{4};
	static constexpr std::size_t PerformanceStatsSize{24};
	static constexpr std::size_t TemperatureStatsSize{28};
};

/**
 * TI xWRxx4x family radar UART driver.
 *
 * The driver uses one UART for control commands and one UART for streaming
 * point cloud data (TLV packet format). It should work for IWR1443, IWR1642,
 * IWR1843, IWR6443 and IWR6843 and their automotive AWR* counterparts.
 * The driver was tested with the prebuild binary version xWR64xx MMW Demo 03.06.02.00
 * on the IWR6843AOP chip.
 * The binary is contained in the mmWaveSDK distributed at https://www.ti.com/tool/de-de/MMWAVE-SDK
 * After installation, the sensor binary can be found at
 * <INDUSTRIAL_TOOLBOX_INSTALL_DIR>\mmwave_industrial_toolbox_<VER>\out_of_box_demo\68xx_aop_mmwave_sdk_hwa\prebuilt_binaries\xwr64xxAOP_mmw_demo.bin
 * The binary is then flashed using Uniflash: https://www.ti.com/tool/en-us/UNIFLASH
 *
 * @tparam ControlUart UART type for the control channel (CLI)
 * @tparam DataUart UART type for the data channel
 * @tparam SyncPin GPIO pin used for optional sync pulse output
 * @tparam FrameQueueSize Number of parsed frames that can be queued
 * @tparam MaxPointsPerFrame Maximum number of detected points stored per frame
 * @tparam MaxParserBufferSize Size of the binary parser buffer
 * @ingroup modm_driver_iwr6843aop
 */
template<class ControlUart, class DataUart, class SyncPin = modm::platform::GpioUnused,
		 std::size_t FrameQueueSize = 8, std::size_t MaxPointsPerFrame = 256,
		 std::size_t MaxParserBufferSize = 4096>
class Iwr6843aop : public iwr6843aop
{
public:
	static_assert(FrameQueueSize > 0);
	static_assert(MaxPointsPerFrame > 0);
	static_assert(MaxParserBufferSize >= 64);

	static constexpr std::size_t MaxResponseLines{16};
	static constexpr std::size_t MaxResponseLineLength{128};
	static constexpr uint32_t MaxConsecutiveParseErrors{32};

	using FrameType = Frame<MaxPointsPerFrame>;

	struct CommandResponse
	{
		std::array<std::array<char, MaxResponseLineLength>, MaxResponseLines> lines{};
		std::size_t lineCount{0};

		void
		clear()
		{
			lineCount = 0;
		}
	};

	Iwr6843aop();

	void
	setSyncInterval(Duration interval);

	Duration
	getSyncInterval() const;

	void
	setControlResponseTimeout(Duration timeout);

	Duration
	getControlResponseTimeout() const;

	bool
	configure(std::span<const char> configuration);

	template<std::size_t N>
	bool
	configure(const char (&configuration)[N])
	{
		static_assert(N > 0);
		return configure(std::span<const char>{configuration, N - 1});
	}

	bool
	sendCommand(std::span<const char> command, CommandResponse *response = nullptr);

	template<std::size_t N>
	bool
	sendCommand(const char (&command)[N], CommandResponse *response = nullptr)
	{
		static_assert(N > 0);
		return sendCommand(std::span<const char>{command, N - 1}, response);
	}

	bool
	sendCommandNoVerify(std::span<const char> command);

	template<std::size_t N>
	bool
	sendCommandNoVerify(const char (&command)[N])
	{
		static_assert(N > 0);
		return sendCommandNoVerify(std::span<const char>{command, N - 1});
	}

	bool
	sendSensorStop();

	/// Poll the data UART and parse all complete frames currently available.
	bool
	processData();

	bool
	getFrame(FrameType &frame);

	bool
	hasFrame() const;

	Error
	getLastError() const;

	void
	clearError();

	bool
	hasError() const;

	uint32_t
	getParseErrorCount() const;

	uint32_t
	getConsecutiveParseErrorCount() const;

private:
	static constexpr std::size_t DataReadChunkSize{512};
	static constexpr std::size_t MaxConfigLineLength{320};
	static constexpr std::size_t MinPacketLength{SyncPattern.size() + FrameHeaderLength};

	bool
	sendCommandInternal(std::span<const char> command, bool verifyDone, CommandResponse *response);

	bool
	waitForCommandResponse(bool verifyDone, CommandResponse *response);

	void
	maybeTriggerSyncPulse();

	void
	triggerSyncPulse();

	bool
	appendData(const uint8_t *data, std::size_t length);

	bool
	processParserBuffer();

	bool
	parsePacket(const uint8_t *packet, std::size_t packetLength, FrameType &frame);

	bool
	parseDetectedPoints(const uint8_t *payload, std::size_t payloadLength, FrameType &frame);

	bool
	parseSideInfo(const uint8_t *payload, std::size_t payloadLength, FrameType &frame);

	bool
	parsePerformanceStatistics(const uint8_t *payload, std::size_t payloadLength, FrameType &frame);

	bool
	parseTemperatureStatistics(const uint8_t *payload, std::size_t payloadLength, FrameType &frame);

	bool
	enqueueFrame(const FrameType &frame);

	void
	dropBytes(std::size_t count);

	void
	setError(Error error);

	bool
	registerParseError();

	static bool
	isWhitespace(char c);

	static bool
	containsPattern(const char *line, std::size_t lineLength, const char *pattern);

	static uint16_t
	readU16(const uint8_t *data);

	static int16_t
	readI16(const uint8_t *data);

	static uint32_t
	readU32(const uint8_t *data);

	static float
	readF32(const uint8_t *data);

	Duration syncInterval_{Duration{100}};
	Duration controlResponseTimeout_{Duration{250}};
	Timeout syncTimer_{Duration{100}};

	Error lastError_{Error::None};

	std::array<char, MaxConfigLineLength> configCommandBuffer_{};
	std::array<char, MaxResponseLineLength> responseLineBuffer_{};
	std::array<uint8_t, DataReadChunkSize> dataReadChunk_{};
	FrameType parserFrame_{};

	std::array<uint8_t, MaxParserBufferSize> parserBuffer_{};
	std::size_t parserSize_{0};

	std::array<FrameType, FrameQueueSize> frameQueue_{};
	std::size_t frameReadIndex_{0};
	std::size_t frameWriteIndex_{0};
	std::size_t frameCount_{0};

	uint32_t parseErrorCount_{0};
	uint32_t consecutiveParseErrorCount_{0};
};

}  // namespace modm

#include "iwr6843aop_impl.hpp"

#endif  // MODM_IWR6843AOP_HPP
