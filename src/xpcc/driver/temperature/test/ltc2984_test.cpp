#include <xpcc/driver/temperature/ltc2984.hpp>
#include <xpcc/debug/logger/logger.hpp>

#include "ltc2984_test.hpp"

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DISABLED

void
Ltc2984Test::testDataStatus()
{
	// Test valid bit
	TEST_ASSERT_TRUE(xpcc::ltc2984::Data(0x01ul << 24).isValid());
	TEST_ASSERT_TRUE(xpcc::ltc2984::Data(0xfful << 24).isValid());
	TEST_ASSERT_FALSE(xpcc::ltc2984::Data(0ul).isValid());

	// Test other status bits
	TEST_ASSERT_EQUALS(static_cast<uint8_t>(xpcc::ltc2984::Data(0xAAul << 24).getStatus()), 0xAA);
	TEST_ASSERT_EQUALS(static_cast<uint8_t>(xpcc::ltc2984::Data(0x55ul << 24).getStatus()), 0x55);
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
	temperatureTableFixed[1] = 1048576ul;
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


	for (size_t jj = 0; jj < XPCC_ARRAY_SIZE(dataTable); ++jj)
	{
		xpcc::ltc2984::Data temperature(dataTable[jj]);
		TEST_ASSERT_TRUE(fabs(temperature.getTemperatureFloat() - temperatureTableFloat[jj]) <= 0.001f);
		TEST_ASSERT_EQUALS(temperature.getTemperatureFixed(), temperatureTableFixed[jj]);
		TEST_ASSERT_EQUALS(temperature.getTemperatureInteger(), temperatureTableInteger[jj]);
	}
}
