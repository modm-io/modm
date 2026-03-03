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
#error "Don't include this file directly, use 'iwr6843aop.hpp' instead!"
#endif

#include <algorithm>
#include <cstring>
#include <modm/architecture/interface/delay.hpp>

namespace modm
{

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::Iwr6843aop()
{
	for (auto &line : frameQueue_) { line.pointCount = 0; }
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::setSyncInterval(Duration interval)
{
	syncInterval_ = interval;
	if (syncInterval_.count() == 0)
	{
		syncTimer_.stop();
	} else
	{
		syncTimer_.restart(syncInterval_);
	}
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
Duration
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::getSyncInterval() const
{
	return syncInterval_;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::setControlResponseTimeout(Duration timeout)
{
	controlResponseTimeout_ = timeout;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
Duration
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::getControlResponseTimeout() const
{
	return controlResponseTimeout_;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::configure(std::span<const char> configuration)
{
	auto &command = configCommandBuffer_;
	std::size_t commandLength{0};

	auto executeCommand = [&](std::size_t length) -> bool {
		std::size_t start{0};
		while (start < length and isWhitespace(command[start])) { start++; }
		while (length > start and isWhitespace(command[length - 1])) { length--; }
		if (length <= start) { return true; }
		if (command[start] == '%') { return true; }
		return sendCommand(std::span<const char>{command.data() + start, length - start});
	};

	for (char c : configuration)
	{
		if (c != '\n')
		{
			if (commandLength >= command.size())
			{
				setError(Error::BufferOverflow);
				return false;
			}
			command[commandLength++] = c;
		} else
		{
			if (not executeCommand(commandLength)) { return false; }
			commandLength = 0;
		}
	}

	if (commandLength > 0)
	{
		if (not executeCommand(commandLength)) { return false; }
	}

	return true;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::sendCommand(std::span<const char> command,
											 CommandResponse *response)
{
	return sendCommandInternal(command, true, response);
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::sendCommandNoVerify(std::span<const char> command)
{
	return sendCommandInternal(command, false, nullptr);
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::sendSensorStop()
{
	static constexpr char SensorStopCommand[] = "sensorStop";
	return sendCommand(std::span<const char>{SensorStopCommand, sizeof(SensorStopCommand) - 1});
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::sendCommandInternal(std::span<const char> command, bool verifyDone,
													 CommandResponse *response)
{
	if (command.empty()) { return true; }

	if (response) { response->clear(); }

	ControlUart::discardReceiveBuffer();
	ControlUart::clearError();

	ControlUart::writeBlocking(reinterpret_cast<const uint8_t *>(command.data()), command.size());
	ControlUart::writeBlocking(static_cast<uint8_t>('\n'));
	ControlUart::flushWriteBuffer();

	if (ControlUart::hasError())
	{
		ControlUart::clearError();
		setError(Error::UartError);
		return false;
	}

	return waitForCommandResponse(verifyDone, response);
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::waitForCommandResponse(bool verifyDone, CommandResponse *response)
{
	auto &currentLine = responseLineBuffer_;
	std::size_t currentLineLength{0};

	bool seenDone{false};
	bool seenPrompt{false};

	Timeout timeout{controlResponseTimeout_};
	while (not timeout.isExpired())
	{
		uint8_t byte{0};
		if (ControlUart::read(byte))
		{
			timeout.restart(controlResponseTimeout_);
			if (byte == '\n')
			{
				if (response and response->lineCount < response->lines.size())
				{
					auto &dst = response->lines[response->lineCount++];
					const auto copyLength = std::min(currentLineLength, dst.size() - 1);
					std::memcpy(dst.data(), currentLine.data(), copyLength);
					dst[copyLength] = '\0';
				}

				if (containsPattern(currentLine.data(), currentLineLength, "Done"))
				{
					seenDone = true;
				}
				if (containsPattern(currentLine.data(), currentLineLength, ":/>"))
				{
					seenPrompt = true;
				}

				currentLineLength = 0;

				if ((verifyDone and seenDone and seenPrompt) or
					(not verifyDone and (seenDone or seenPrompt)))
				{
					return true;
				}
			} else if (byte != '\r')
			{
				if (currentLineLength < currentLine.size() - 1)
				{
					currentLine[currentLineLength++] = static_cast<char>(byte);
					// CLI prompt may arrive without newline terminator.
					if (containsPattern(currentLine.data(), currentLineLength, ":/>"))
					{
						seenPrompt = true;
						if (verifyDone and seenDone) { return true; }
					}
				}
			}
		} else if (ControlUart::hasError())
		{
			ControlUart::discardReceiveBuffer();
			ControlUart::clearError();
			setError(Error::UartError);
			return false;
		} else
		{
			modm::this_fiber::sleep_for(100us);
		}
	}

	setError(verifyDone ? Error::CommandError : Error::Timeout);
	return false;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::processData()
{
	maybeTriggerSyncPulse();

	while (true)
	{
		const auto bytesRead = DataUart::read(dataReadChunk_.data(), dataReadChunk_.size());
		if (bytesRead == 0) { break; }

		if (not appendData(dataReadChunk_.data(), bytesRead)) { return false; }
		modm::this_fiber::yield();
	}

	if (DataUart::hasError())
	{
		DataUart::discardReceiveBuffer();
		DataUart::clearError();
		setError(Error::UartError);
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::getFrame(FrameType &frame)
{
	if (frameCount_ == 0) { return false; }

	frame = frameQueue_[frameReadIndex_];
	frameReadIndex_ = (frameReadIndex_ + 1) % FrameQueueSize;
	frameCount_--;
	return true;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::hasFrame() const
{
	return frameCount_ > 0;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
typename Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
					MaxParserBufferSize>::Error
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::getLastError() const
{
	return lastError_;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::clearError()
{
	lastError_ = Error::None;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::hasError() const
{
	return lastError_ != Error::None;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
uint32_t
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::getParseErrorCount() const
{
	return parseErrorCount_;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
uint32_t
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::getConsecutiveParseErrorCount() const
{
	return consecutiveParseErrorCount_;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::maybeTriggerSyncPulse()
{
	if (syncInterval_.count() == 0) { return; }

	if (syncTimer_.isExpired())
	{
		triggerSyncPulse();
		syncTimer_.restart(syncInterval_);
	}
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::triggerSyncPulse()
{
	SyncPin::set();
	modm::this_fiber::sleep_for(100us);
	SyncPin::reset();
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::appendData(const uint8_t *data, std::size_t length)
{
	if (length == 0) { return true; }

	if (length > MaxParserBufferSize)
	{
		data += (length - MaxParserBufferSize);
		length = MaxParserBufferSize;
		setError(Error::BufferOverflow);
	}

	if (parserSize_ + length > MaxParserBufferSize)
	{
		const auto overflow = parserSize_ + length - MaxParserBufferSize;
		dropBytes(overflow);
		setError(Error::BufferOverflow);
	}

	std::memcpy(parserBuffer_.data() + parserSize_, data, length);
	parserSize_ += length;

	return processParserBuffer();
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::processParserBuffer()
{
	while (true)
	{
		if (parserSize_ < SyncPattern.size()) { return true; }

		std::size_t syncPosition = parserSize_;
		for (std::size_t ii = 0; ii <= parserSize_ - SyncPattern.size(); ++ii)
		{
			if (std::memcmp(parserBuffer_.data() + ii, SyncPattern.data(), SyncPattern.size()) == 0)
			{
				syncPosition = ii;
				break;
			}
		}

		if (syncPosition == parserSize_)
		{
			const std::size_t keep = SyncPattern.size() - 1;
			if (parserSize_ > keep)
			{
				std::memmove(parserBuffer_.data(), parserBuffer_.data() + parserSize_ - keep, keep);
				parserSize_ = keep;
			}
			return true;
		}

		if (syncPosition > 0) { dropBytes(syncPosition); }

		if (parserSize_ < MinPacketLength) { return true; }

		const uint32_t totalPacketLength = readU32(parserBuffer_.data() + SyncPattern.size() + 4);
		if (totalPacketLength < MinPacketLength or totalPacketLength > MaxParserBufferSize)
		{
			setError(Error::ParseError);
			if (not registerParseError()) { return false; }
			dropBytes(1);
			continue;
		}

		if (parserSize_ < totalPacketLength) { return true; }

		if (not parsePacket(parserBuffer_.data(), totalPacketLength, parserFrame_))
		{
			setError(Error::ParseError);
			if (not registerParseError()) { return false; }
			dropBytes(1);
			continue;
		}

		consecutiveParseErrorCount_ = 0;
		enqueueFrame(parserFrame_);
		dropBytes(totalPacketLength);
	}
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::parsePacket(const uint8_t *packet, std::size_t packetLength,
											 FrameType &frame)
{
	if (packetLength < MinPacketLength) { return false; }
	if (std::memcmp(packet, SyncPattern.data(), SyncPattern.size()) != 0) { return false; }

	const uint8_t *header = packet + SyncPattern.size();
	frame.timestamp = Clock::now();
	frame.frameHeader.version = readU32(header + 0);
	frame.frameHeader.totalPacketLen = readU32(header + 4);
	frame.frameHeader.platform = readU32(header + 8);
	frame.frameHeader.frameNumber = readU32(header + 12);
	frame.frameHeader.timeCpuCycles = readU32(header + 16);
	frame.frameHeader.numDetectedObj = readU32(header + 20);
	frame.frameHeader.numTlvs = readU32(header + 24);
	frame.frameHeader.subframeNumber = readU32(header + 28);

	frame.pointCount = 0;
	frame.performanceStatistics.reset();
	frame.temperatureStatistics.reset();

	std::size_t tlvOffset = SyncPattern.size() + FrameHeaderLength;
	for (uint32_t tlvIndex = 0; tlvIndex < frame.frameHeader.numTlvs; ++tlvIndex)
	{
		if (tlvOffset + TlvHeaderLength > packetLength) { return false; }

		const auto tlvType = readU32(packet + tlvOffset);
		const auto tlvPayloadLength = readU32(packet + tlvOffset + 4);
		const std::size_t tlvTotalLength =
			TlvHeaderLength + static_cast<std::size_t>(tlvPayloadLength);
		if (tlvOffset + tlvTotalLength > packetLength) { return false; }

		const uint8_t *payload = packet + tlvOffset + TlvHeaderLength;
		const std::size_t payloadLength = tlvPayloadLength;

		switch (tlvType)
		{
			case static_cast<uint32_t>(RadarOutputType::DetectedPoints):
				if (not parseDetectedPoints(payload, payloadLength, frame)) { return false; }
				break;
			case static_cast<uint32_t>(RadarOutputType::SideInfoDetectedPoints):
				if (not parseSideInfo(payload, payloadLength, frame)) { return false; }
				break;
			case static_cast<uint32_t>(RadarOutputType::PerformanceStatistics):
				if (not parsePerformanceStatistics(payload, payloadLength, frame)) { return false; }
				break;
			case static_cast<uint32_t>(RadarOutputType::TemperatureStatistics):
				if (not parseTemperatureStatistics(payload, payloadLength, frame)) { return false; }
				break;
			default:
				break;
		}

		tlvOffset += tlvTotalLength;
	}

	return true;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::parseDetectedPoints(const uint8_t *payload,
													 std::size_t payloadLength, FrameType &frame)
{
	if ((payloadLength % PointStructSize) != 0) { return false; }

	const std::size_t pointsInPayload = payloadLength / PointStructSize;
	const std::size_t pointsToStore = std::min(pointsInPayload, MaxPointsPerFrame);

	for (std::size_t ii = 0; ii < pointsToStore; ++ii)
	{
		const std::size_t offset = ii * PointStructSize;
		DetectedPoint point{
			.x = readF32(payload + offset + 0),
			.y = readF32(payload + offset + 4),
			.z = readF32(payload + offset + 8),
			.velocity = readF32(payload + offset + 12),
		};
		frame.points[ii] = PointWithSideInfo{.point = point, .sideInfo = std::nullopt};
	}

	frame.pointCount = pointsToStore;
	if (pointsInPayload > pointsToStore) { setError(Error::BufferOverflow); }
	return true;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::parseSideInfo(const uint8_t *payload, std::size_t payloadLength,
											   FrameType &frame)
{
	if ((payloadLength % SideInfoStructSize) != 0) { return false; }

	const std::size_t sideInfoCount = payloadLength / SideInfoStructSize;
	const std::size_t pointCount = std::min(sideInfoCount, frame.pointCount);

	for (std::size_t ii = 0; ii < pointCount; ++ii)
	{
		const std::size_t offset = ii * SideInfoStructSize;
		const auto snrRaw = readI16(payload + offset + 0);
		const auto noiseRaw = readI16(payload + offset + 2);

		frame.points[ii].sideInfo = PointSideInfo{
			.snr = static_cast<float>(snrRaw) * 0.1f,
			.noise = static_cast<float>(noiseRaw) * 0.1f,
		};
	}

	return true;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::parsePerformanceStatistics(const uint8_t *payload,
															std::size_t payloadLength,
															FrameType &frame)
{
	if (payloadLength < PerformanceStatsSize) { return false; }

	frame.performanceStatistics = PerformanceStatistics{
		.interframeProcessingTime = readU32(payload + 0),
		.transmitOutputTime = readU32(payload + 4),
		.interframeProcessingMargin = readU32(payload + 8),
		.interchirpProcessingMargin = readU32(payload + 12),
		.activeFrameCpuLoad = readU32(payload + 16),
		.interframeCpuLoad = readU32(payload + 20),
	};
	return true;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::parseTemperatureStatistics(const uint8_t *payload,
															std::size_t payloadLength,
															FrameType &frame)
{
	if (payloadLength < TemperatureStatsSize) { return false; }

	frame.temperatureStatistics = TemperatureStatistics{
		.reportValid = readU32(payload + 0),
		.timeStamp = readU32(payload + 4),
		.tempRx0 = static_cast<float>(readU16(payload + 8)),
		.tempRx1 = static_cast<float>(readU16(payload + 10)),
		.tempRx2 = static_cast<float>(readU16(payload + 12)),
		.tempRx3 = static_cast<float>(readU16(payload + 14)),
		.tempTx0 = static_cast<float>(readU16(payload + 16)),
		.tempTx1 = static_cast<float>(readU16(payload + 18)),
		.tempTx2 = static_cast<float>(readU16(payload + 20)),
		.tempPm = static_cast<float>(readU16(payload + 22)),
		.tempDig0 = static_cast<float>(readU16(payload + 24)),
		.tempDig1 = static_cast<float>(readU16(payload + 26)),
	};
	return true;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::enqueueFrame(const FrameType &frame)
{
	if (frameCount_ >= FrameQueueSize)
	{
		setError(Error::QueueFull);
		return false;
	}

	frameQueue_[frameWriteIndex_] = frame;
	frameWriteIndex_ = (frameWriteIndex_ + 1) % FrameQueueSize;
	frameCount_++;
	return true;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::dropBytes(std::size_t count)
{
	count = std::min(count, parserSize_);
	if (count == 0) { return; }

	const auto remaining = parserSize_ - count;
	if (remaining > 0)
	{
		std::memmove(parserBuffer_.data(), parserBuffer_.data() + count, remaining);
	}
	parserSize_ = remaining;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
void
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::setError(Error error)
{
	if (error != Error::None) { lastError_ = error; }
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::registerParseError()
{
	parseErrorCount_++;
	consecutiveParseErrorCount_++;
	return consecutiveParseErrorCount_ < MaxConsecutiveParseErrors;
}

// ----------------------------------------------------------------------------

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::isWhitespace(char c)
{
	return c == ' ' or c == '\t' or c == '\r';
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
bool
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::containsPattern(const char *line, std::size_t lineLength,
												 const char *pattern)
{
	const auto patternLength = std::strlen(pattern);
	if (patternLength == 0 or lineLength < patternLength) { return false; }

	for (std::size_t ii = 0; ii <= lineLength - patternLength; ++ii)
	{
		if (std::memcmp(line + ii, pattern, patternLength) == 0) { return true; }
	}
	return false;
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
uint16_t
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::readU16(const uint8_t *data)
{
	return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
int16_t
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::readI16(const uint8_t *data)
{
	return static_cast<int16_t>(readU16(data));
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
uint32_t
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::readU32(const uint8_t *data)
{
	return static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) |
		   (static_cast<uint32_t>(data[2]) << 16) | (static_cast<uint32_t>(data[3]) << 24);
}

template<class ControlUart, class DataUart, class SyncPin, std::size_t FrameQueueSize,
		 std::size_t MaxPointsPerFrame, std::size_t MaxParserBufferSize>
float
Iwr6843aop<ControlUart, DataUart, SyncPin, FrameQueueSize, MaxPointsPerFrame,
		   MaxParserBufferSize>::readF32(const uint8_t *data)
{
	const uint32_t bits = readU32(data);
	float value{0.f};
	std::memcpy(&value, &bits, sizeof(value));
	return value;
}

}  // namespace modm
