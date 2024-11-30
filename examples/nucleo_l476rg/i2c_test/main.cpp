/*
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

/*
 * Test of I2C transaction with STM32L4
 *
 * A simple I2C device with write, read, and write_read transaction is implemented.
 * A device responding to address 0x3c must be connected, e.g. SSD1306 display.
 * The display must be powercycled after writing garbage to it. It may not be initialized
 * after this test programm.
 *
 * Observe the I2C transactions either with pulseview or with sigrok-cli, e.g.
 * sigrok-cli --driver fx2lafw --config samplerate=1m --samples 64k --channels D4=sda,D5=scl --triggers sda=f --wait-trigger -P i2c:sda=sda:scl=scl
 */

using MyI2cMaster = modm::platform::I2cMaster1;

using namespace Board;

template < class I2cMaster >
class I2cTestDevice : public modm::I2cDevice<I2cMaster, 2>
{
public:
	I2cTestDevice(uint8_t address = 0x3C);

	bool
	write(size_t len);

	bool
	read(size_t len);

	bool
	writeRead(size_t write_len, size_t read_len);

private:
	uint8_t buffer[1024];
};

template < typename I2cMaster >
I2cTestDevice<I2cMaster>::I2cTestDevice(uint8_t address) :
	modm::I2cDevice<I2cMaster, 2>(address)
{
}

template < typename I2cMaster >
bool
I2cTestDevice<I2cMaster>::write(size_t len)
{
	for (size_t ii = 0; ii < 1024; ++ii) {
		buffer[ii] = ii + 1;
	}
	buffer[255] = 0x82;
	return modm::I2cDevice<I2cMaster, 2>::write(buffer, len);
}

template < typename I2cMaster >
bool
I2cTestDevice<I2cMaster>::read(size_t len)
{
	if (not modm::I2cDevice<I2cMaster, 2>::read(buffer, len)) return false;

	for (size_t ii = 0; ii < len; ++ii) {
		MODM_LOG_DEBUG.printf("%02x ", buffer[ii]);
	}
	MODM_LOG_DEBUG << modm::endl;
	return true;
}

template < typename I2cMaster >
bool
I2cTestDevice<I2cMaster>::writeRead(size_t write_len, size_t read_len)
{
	for (size_t ii = 0; ii < 1024; ++ii) {
		buffer[ii] = ii + 1;
	}
	buffer[255] = 0x82;
	if (not modm::I2cDevice<I2cMaster, 2>::writeRead(buffer, write_len, buffer, read_len))
		return false;

	for (size_t ii = 0; ii < read_len; ++ii) {
		MODM_LOG_DEBUG.printf("%02x ", buffer[ii]);
	}
	MODM_LOG_DEBUG << modm::endl;
	return true;
}

I2cTestDevice< MyI2cMaster > i2c;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MyI2cMaster::connect<Board::D14::Sda, Board::D15::Scl>();
	MyI2cMaster::initialize<Board::SystemClock, 100_kHz>();

	LedGreen::set();

	i2c.ping();
	modm::delay(25us);

	i2c.write(0);
	modm::delay(25us);

	i2c.write(1);
	modm::delay(25us);

	i2c.write(2);
	modm::delay(25us);

	i2c.writeRead(0, 5);
	modm::delay(25us);

	i2c.writeRead(1, 5);
	modm::delay(25us);

	i2c.writeRead(2, 5);
	modm::delay(25us);

	// Blink if run without hanging.
	while(true)
	{
		LedGreen::toggle();
		modm::delay(500ms);
	}

	return 0;
}
