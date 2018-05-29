/*
 * Copyright (c) 2017, Raphael Lehmann
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/driver/temperature/ltc2984.hpp>
#include <modm/debug/logger/logger.hpp>

#include <modm/platform/spi/mock/spi_master.hpp>
#include <modm/platform/gpio/unused.hpp>

#include "ltc2984_test.hpp"
#include "math.h"

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

void
Ltc2984Test::testDataStatus()
{
	// Test valid bit
	TEST_ASSERT_TRUE(modm::ltc2984::Data(0x01 << 24).isValid());
	TEST_ASSERT_TRUE(modm::ltc2984::Data(0xff << 24).isValid());
	TEST_ASSERT_FALSE(modm::ltc2984::Data(0ul).isValid());

	// Test other status bits
	TEST_ASSERT_EQUALS(static_cast<uint8_t>(modm::ltc2984::Data(0xAAul << 24).getStatus()), 0xAA);
	TEST_ASSERT_EQUALS(static_cast<uint8_t>(modm::ltc2984::Data(0x55ul << 24).getStatus()), 0x55);
}

void
Ltc2984Test::testDataTemperature()
{
	// Example data output words from datasheet, Table 9A, page 22.
	// http://cds.linear.com/docs/en/datasheet/2984fb.pdf

	uint32_t dataTable[8];
	float temperatureTableFloat[8];
	int32_t temperatureTableFixed[8];
	int16_t temperatureTableInteger[8];

	// 8191.999°C
	dataTable[0] = 0b00000001011111111111111111111111;
	temperatureTableFloat[0] = 8191.999;
	temperatureTableFixed[0] = 1024 * 8191.999f;
	temperatureTableInteger[0] = 8192;

	// 1024°C
	dataTable[1] = 0b00000001000100000000000000000000;
	temperatureTableFloat[1] = 1024;
	temperatureTableFixed[1] = 1024 * 1024;
	temperatureTableInteger[1] = 1024;

	// 1°C
	dataTable[2] = 0b00000001000000000000010000000000;
	temperatureTableFloat[2] = 1;
	temperatureTableFixed[2] = 1024 * 1;
	temperatureTableInteger[2] = 1;

	// 1/1024°C
	dataTable[3] = 0b00000001000000000000000000000001;
	temperatureTableFloat[3] = 1 / 1024.f;
	temperatureTableFixed[3] = 1024 * 1 / 1024;
	temperatureTableInteger[3] = 0;

	// 0°C
	dataTable[4] = 0b00000001000000000000000000000000;
	temperatureTableFloat[4] = 0;
	temperatureTableFixed[4] = 0;
	temperatureTableInteger[4] = 0;

	// -1/1024°C
	dataTable[5] = 0b00000001111111111111111111111111;
	temperatureTableFloat[5] = -1 / 1024.f;
	temperatureTableFixed[5] = 1024 * (-1) / 1024;
	temperatureTableInteger[5] = 0;

	// -1°C
	dataTable[6] = 0b00000001111111111111110000000000;
	temperatureTableFloat[6] = -1;
	temperatureTableFixed[6] = 1024 * -1;
	temperatureTableInteger[6] = -1;

	// -273.15°C
	dataTable[7] = 0b00000001111110111011101101100111;
	temperatureTableFloat[7] = -273.15;
	temperatureTableFixed[7] = -1024 * 273.15f;
	temperatureTableInteger[7] = -273;


	for (size_t jj = 0; jj < MODM_ARRAY_SIZE(dataTable); ++jj)
	{
		modm::ltc2984::Data temperature(dataTable[jj]);
		TEST_ASSERT_TRUE(fabs(temperature.getTemperatureFloat() - temperatureTableFloat[jj]) <= 0.001f);
		TEST_ASSERT_EQUALS(temperature.getTemperatureFixed(), temperatureTableFixed[jj]);
		TEST_ASSERT_EQUALS(temperature.getTemperatureInteger(), temperatureTableInteger[jj]);
	}
}

void
Ltc2984Test::testSpi()
{
	// Test Spi with MockSpiMaster
	using SpiMaster = modm::platform::SpiMasterMock;
	modm::Ltc2984<SpiMaster, modm::platform::GpioUnused> tempSensor;

	uint32_t channelConfigurationTest = (0b11101ul << 27) | (2000*1024);
	uint32_t channelConfiguration = modm::ltc2984::Configuration::rsense(modm::ltc2984::Configuration::Rsense::Resistance_t(2000*1024));
	TEST_ASSERT_EQUALS(channelConfiguration, channelConfigurationTest);
	RF_CALL_BLOCKING(tempSensor.configureChannel(modm::ltc2984::Channel::Ch2, channelConfiguration));

	constexpr std::size_t txBufferLength = 7;
	uint8_t txBuffer[txBufferLength];

	uint8_t txBufferCompare[] = {0x02, 0x02, 0x04, 0b11101000, 0b00011111, 0b01000000, 0b00000000};
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 7u);
	SpiMaster::popTxBuffer(txBuffer);
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare, 7u);

	channelConfigurationTest = 0x00000000;
	channelConfiguration = modm::ltc2984::Configuration::disabled();
	TEST_ASSERT_EQUALS(channelConfiguration, channelConfigurationTest);
	RF_CALL_BLOCKING(tempSensor.configureChannel(modm::ltc2984::Channel::Ch2, channelConfiguration));

	uint8_t txBufferCompare2[] = {0x02, 0x02, 0x04, 0, 0, 0, 0};
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 7u);
	SpiMaster::popTxBuffer(txBuffer);
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare2, 7u);

	channelConfigurationTest = 0x60A9C000;
	channelConfiguration = modm::ltc2984::Configuration::rtd(
		modm::ltc2984::Configuration::SensorType::Pt100,
		modm::ltc2984::Configuration::Rtd::RsenseChannel::Ch2_Ch1,
		modm::ltc2984::Configuration::Rtd::Wires::Wire4,
		modm::ltc2984::Configuration::Rtd::ExcitationMode::Rotation_Sharing,
		modm::ltc2984::Configuration::Rtd::ExcitationCurrent::Current_500uA,
		modm::ltc2984::Configuration::Rtd::RtdCurve::European
	);
	TEST_ASSERT_EQUALS(channelConfiguration, channelConfigurationTest);
	RF_CALL_BLOCKING(tempSensor.configureChannel(modm::ltc2984::Channel::Ch4, channelConfiguration));

	uint8_t txBufferCompare3[] = {0x02, 0x02, 0x0C, 0x60, 0xA9, 0xC0, 0x00};
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 7u);
	SpiMaster::popTxBuffer(txBuffer);
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare3, 7u);

	tempSensor.enableChannel(modm::ltc2984::Configuration::MuxChannel::Ch4);
	RF_CALL_BLOCKING(tempSensor.setChannels());
	uint8_t txBufferCompare4[] = {0x02, 0x00, 0xF4, 0x00, 0x00, 0x00, 0x08};
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 7u);
	SpiMaster::popTxBuffer(txBuffer);
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare4, 7u);

	RF_CALL_BLOCKING(tempSensor.initiateSingleMeasurement(modm::ltc2984::Channel::Ch4));
	uint8_t txBufferCompare5[] = {0x02, 0x00, 0x00, 0x84};
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 4u);
	SpiMaster::popTxBuffer(txBuffer);
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare5, 4u);

	modm::ltc2984::Data temperature;
	uint8_t rxBuffer[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00}; // valid temperature: 2.000 deg C
	SpiMaster::appendRxBuffer(rxBuffer, 7u);
	RF_CALL_BLOCKING(tempSensor.readChannel(modm::ltc2984::Channel::Ch4, temperature));
	uint8_t txBufferCompare6[] = {0x03, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00};
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 7u);
	SpiMaster::popTxBuffer(txBuffer);
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare6, 7u);
	TEST_ASSERT_EQUALS(temperature.getTemperatureFixed(), 2 * 1024);
}

void
Ltc2984Test::tearDown() {
	modm::platform::SpiMasterMock::clearBuffers();
}
