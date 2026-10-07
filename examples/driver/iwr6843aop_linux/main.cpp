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

#include <cstdlib>
#include <fstream>
#include <modm/debug/logger.hpp>
#include <modm/platform.hpp>
#include <modm/platform/uart/static_serial_interface.hpp>
#include <modm/driver/radar/iwr6843aop.hpp>
#include <string>

using namespace modm::platform;

template<int N>
class HostedSerialUart : public modm::platform::StaticSerialInterface<N>
{
public:
	using modm::platform::StaticSerialInterface<N>::read;

	// blocking read seems to work nicer with the hardware
	static std::size_t
	read(uint8_t *data, std::size_t length)
	{
		std::size_t count{0};
		uint8_t byte{0};
		while (count < length)
		{
			if (not modm::platform::StaticSerialInterface<N>::read(byte)) { break; }
			data[count++] = byte;
		}
		return count;
	}

	static bool
	hasError()
	{
		return false;
	}

	static void
	clearError()
	{}
};

using ControlUart = HostedSerialUart<0>;
using DataUart = HostedSerialUart<1>;
using Radar = modm::Iwr6843aop<ControlUart, DataUart>;

int
main(int argc, char **argv)
{
	const char *cfgPath = "../../../src/modm/driver/radar/iwr6843aop.cfg";
	if (argc > 1) { cfgPath = argv[1]; }

	std::ifstream configFile(cfgPath, std::ios::binary);
	if (not configFile.good())
	{
		MODM_LOG_ERROR << "Could not open config file: " << cfgPath << modm::endl;
		return EXIT_FAILURE;
	}

	const std::string configuration((std::istreambuf_iterator<char>(configFile)),
									std::istreambuf_iterator<char>());

	SerialInterface controlPort("/dev/ttyUSB0", 115200);
	SerialInterface dataPort("/dev/ttyUSB1", 921600);

	if (not ControlUart::initialize<115200>(controlPort))
	{
		MODM_LOG_ERROR << "Could not open control port: " << controlPort.getDeviceName().c_str()
					   << modm::endl;
		return EXIT_FAILURE;
	}

	if (not DataUart::initialize<921600>(dataPort))
	{
		MODM_LOG_ERROR << "Could not open data port: " << dataPort.getDeviceName().c_str()
					   << modm::endl;
		controlPort.close();
		return EXIT_FAILURE;
	}

	Radar radar;
	radar.setSyncInterval(modm::Duration{100});

	MODM_LOG_INFO << "Uploading configuration to radar..." << modm::endl;
	if (not radar.configure(std::span<const char>{configuration.data(), configuration.size()}))
	{
		MODM_LOG_ERROR << "Radar configuration failed, error="
					   << static_cast<int>(radar.getLastError()) << modm::endl;
		controlPort.close();
		dataPort.close();
		return EXIT_FAILURE;
	}

	MODM_LOG_INFO << "Configuration successful" << modm::endl;

	Radar::FrameType frame{};
	modm::Timestamp lastFrameTimestamp{};
	bool hasLastFrameTimestamp{false};
	uint32_t processedFrames{0};

	while (true)
	{
		if (not radar.processData())
		{
			MODM_LOG_ERROR << "Data processing error=" << static_cast<int>(radar.getLastError())
						   << ", parse_errors=" << radar.getParseErrorCount()
						   << ", consecutive_parse_errors=" << radar.getConsecutiveParseErrorCount()
						   << modm::endl;
			radar.clearError();
			modm::delay_ms(5);
		}

		while (radar.getFrame(frame))
		{
			processedFrames++;

			const auto now = modm::Clock::now();
			const auto age = now - frame.timestamp;

			uint32_t intervalMs{0};
			if (hasLastFrameTimestamp)
			{
				intervalMs = (frame.timestamp - lastFrameTimestamp).count();
			}
			lastFrameTimestamp = frame.timestamp;
			hasLastFrameTimestamp = true;

			float maxVelocity{0.f};
			for (std::size_t ii = 0; ii < frame.pointCount; ++ii)
			{
				float velocity = frame.points[ii].point.velocity;
				if (velocity < 0.f) { velocity = -velocity; }
				if (velocity > maxVelocity) { maxVelocity = velocity; }
			}

			MODM_LOG_INFO.printf(
				"Frame #%lu: %lu points detected, age: %lu ms, interval: %lu ms, max velocity: "
				"%.2f m/s\n",
				static_cast<unsigned long>(frame.frameHeader.frameNumber),
				static_cast<unsigned long>(frame.pointCount),
				static_cast<unsigned long>(age.count()), static_cast<unsigned long>(intervalMs),
				static_cast<double>(maxVelocity));

			if ((processedFrames % 10u) == 0u)
			{
				MODM_LOG_INFO.printf("Processed %lu frames\n",
									 static_cast<unsigned long>(processedFrames));
			}
		}

		modm::delay_ms(1);
	}

	controlPort.close();
	dataPort.close();
	return EXIT_SUCCESS;
}
