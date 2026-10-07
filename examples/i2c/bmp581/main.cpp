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

#include <atomic>
#include <modm/board.hpp>
#include <modm/driver/pressure/bmp581.hpp>

using namespace Board;

// I2C2 Bus Configuration
using I2c = I2cMaster2;
using Scl = GpioB10;
using Sda = GpioB11;

// Interrupt Pin
using BaroInt = GpioA4;

// BMP581 Driver Setup
using Transport = modm::Bmp581I2cTransport<I2c>;
using Baro = modm::Bmp581<Transport>;

// BMP581 I2C address (0x46 with SDO to GND, 0x47 with SDO to VDDIO)
constexpr uint8_t BaroAddress = 0x47;
Baro baro{BaroAddress};
std::atomic<bool> dataReady{false};

bool
initializeBaro()
{
	// Initialize the sensor
	if (!baro.initialize())
	{
		MODM_LOG_ERROR << "BMP581 initialization failed!\n";
		return false;
	}

	// Configure oversampling: 4x for both pressure and temperature
	if (!baro.setOversampling(Baro::Osr::X4, Baro::Osr::X4, true))
	{
		MODM_LOG_ERROR << "Failed to set oversampling!\n";
		return false;
	}

	// Configure output data rate: 50 Hz
	if (!baro.setOdr(Baro::Odr::Hz50))
	{
		MODM_LOG_ERROR << "Failed to set ODR!\n";
		return false;
	}

	// Configure IIR filter: coefficient 3 for both
	if (!baro.setIirFilter(Baro::IirFilter::Coef3, Baro::IirFilter::Coef3))
	{
		MODM_LOG_ERROR << "Failed to set IIR filter!\n";
		return false;
	}

	// Configure interrupt: active high, push-pull, pulsed mode
	const auto intConfig = Baro::IntConfig::Enable | Baro::IntConfig::Polarity;  // Active high
	if (!baro.setIntConfig(intConfig))
	{
		MODM_LOG_ERROR << "Failed to set interrupt config!\n";
		return false;
	}

	// Enable data ready interrupt
	if (!baro.setIntSource(Baro::IntSource::DataReadyEnable))
	{
		MODM_LOG_ERROR << "Failed to set interrupt source!\n";
		return false;
	}

	/*
	 * FIFO usage example:
	 *
	 * baro.setFifoSelect(Baro::FifoFrameSelect::PressureAndTemperature,
	 *                    Baro::FifoDecimation::None);
	 * baro.setFifoThreshold(8);
	 * baro.setIntSource(Baro::IntSource::FifoThresholdEnable);
	 * baro.setFifoEnabled(true);
	 *
	 * // On interrupt/main loop:
	 * // const auto count = baro.getFifoCount();
	 * // read up to `count` entries with readFifoFrames(...)
	 */

	// Set power mode to normal (continuous measurement)
	if (!baro.setPowerMode(Baro::PowerMode::Normal))
	{
		MODM_LOG_ERROR << "Failed to set power mode!\n";
		return false;
	}

	return true;
}

int
main()
{
	Board::initialize();
	Leds::setOutput();

	// Initialize I2C2
	I2c::connect<Scl::Scl, Sda::Sda>(I2c::PullUps::Internal);
	I2c::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "BMP581 Barometric Pressure Sensor Example\n";
	MODM_LOG_INFO << "=========================================\n\n";
	// Configure interrupt pin
	BaroInt::setInput(BaroInt::InputType::PullDown);

	// Initialize sensor with retries
	while (!initializeBaro())
	{
		LedRed::toggle();
		MODM_LOG_ERROR << "Retrying initialization...\n";
		modm::delay(250ms);
	}

	MODM_LOG_INFO << "BMP581 initialized successfully!\n\n";

	// Read chip ID for verification
	if (const auto chipId = baro.readChipId(); chipId)
	{
		MODM_LOG_INFO.printf("Chip ID: 0x%02X (expected 0x50)\n\n", *chipId);
	}

	MODM_LOG_INFO << "Starting continuous measurement (DRDY EXTI)...\n\n";

	Exti::connect<BaroInt>(Exti::Trigger::RisingEdge, [](auto) {
		dataReady = true;
		LedYellow::toggle();
	});

	modm::bmp581::Data data;
	uint32_t sampleCount = 0;
	uint32_t readErrorCount = 0;
	while (true)
	{
		if (!dataReady) { continue; }
		dataReady = false;

		if (baro.readData(data))
		{
			const float temperature = data.getTemperature();
			const float pressure = data.getPressure();
			const float pressureHpa = data.getPressureHpa();

			// Calculate approximate altitude (simplified barometric formula)
			// Using standard sea level pressure of 1013.25 hPa
			constexpr float seaLevelPressure = 1013.25f;
			const float altitude =
				44330.0f * (1.0f - powf(pressureHpa / seaLevelPressure, 0.1903f));

			sampleCount++;

			// Print every 25th sample (~2 Hz at 50Hz sampling)
			if (sampleCount % 25 == 0)
			{
				MODM_LOG_INFO.printf("Sample #%lu\n", sampleCount);
				MODM_LOG_INFO.printf("  Temperature: %7.3f C\n", temperature);
				MODM_LOG_INFO.printf("  Pressure:    %9.2f Pa (%7.2f hPa)\n", pressure,
									 pressureHpa);
				MODM_LOG_INFO.printf("  Altitude:    %7.1f m (approx)\n\n", altitude);
			}

			LedGreen::toggle();
			readErrorCount = 0;
		} else
		{
			readErrorCount++;
			if (readErrorCount % 50 == 0) { MODM_LOG_ERROR << "Failed to read sensor data!\n"; }
			LedRed::set();
		}
	}

	return 0;
}
