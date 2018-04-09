/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * Based on work of Alexander Entinger, MSc / LXRobotics
 * Based on https://github.com/mrjohnk/ADNS-9800
 */

#ifndef MODM_ADNS9800_HPP
#error  "Don't include this file directly, use 'adns9800.hpp' instead!"
#endif

#include <modm/debug/logger.hpp>
#include <modm/architecture/interface/delay.hpp>

#include "adns9800.hpp"
#include "adns9800_srom.hpp"

// Set the log level
#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

template < typename Spi, typename Cs >
bool
modm::Adns9800< Spi, Cs>::isNewMotionDataAvailable()
{
	uint8_t const motion_reg = readReg(Register::Motion);
	bool const new_data_available = (motion_reg & 0x80) > 0;
	return new_data_available;
}

template < typename Spi, typename Cs >
void
modm::Adns9800< Spi, Cs>::getDeltaXY(int16_t &delta_x, int16_t &delta_y)
{
	Cs::reset();
	modm::delayMicroseconds(100); // tSRAD

	Spi::transferBlocking(static_cast<uint8_t>(Register::Motion_Burst));

	// Delay tSRAD
	modm::delayMicroseconds(100);

	static constexpr uint8_t buf_size = 6;
	uint8_t tx_buf[buf_size];
	uint8_t rx_buf[buf_size];

	Spi::transferBlocking(tx_buf, rx_buf, buf_size);

	modm::delayNanoseconds(120); // tSCLK-NCS for read operation is 120ns
	Cs::set();

	uint8_t delta_x_l = rx_buf[2];
	uint8_t delta_x_h = rx_buf[3];
	uint8_t delta_y_l = rx_buf[4];
	uint8_t delta_y_h = rx_buf[5];

	delta_x = delta_x_h << 8 | delta_x_l;
	delta_y = delta_y_h << 8 | delta_y_l;
}

template < typename Spi, typename Cs >
uint8_t
modm::Adns9800< Spi, Cs>::readReg(Register const reg)
{
	Cs::reset();

	uint8_t address = static_cast<uint8_t>(reg);

	// send adress of the register, with MSBit = 0 to indicate it's a read
	Spi::transferBlocking(address & 0x7f);
	modm::delayMicroseconds(100); // tSRAD
	
	// read data
	uint8_t data = Spi::transferBlocking(0);

	modm::delayNanoseconds(120); // tSCLK-NCS for read operation is 120ns
	Cs::set();
	modm::delayMicroseconds(19); // tSRW/tSRR (=20us) minus tSCLK-NCS

	return data; 
}

template < typename Spi, typename Cs >
void
modm::Adns9800< Spi, Cs>::writeReg(Register const reg, uint8_t const data)
{
	Cs::reset();

	uint8_t address = static_cast<uint8_t>(reg);

	//send adress of the register, with MSBit = 1 to indicate it's a write
	Spi::transferBlocking(address | 0x80);

	//send data
	Spi::transferBlocking(data);

	modm::delayMicroseconds(20); // tSCLK-NCS for write operation
	Cs::set();
	modm::delayMicroseconds(100); // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be shortened, but is looks like a safe lower bound 
}

template < typename Spi, typename Cs >
void
modm::Adns9800< Spi, Cs >::uploadFirmware()
{
	// set the configuration_IV register in 3k firmware mode
	writeReg(Register::Configuration_IV, 0x02); // bit 1 = 1 for 3k mode, other bits are reserved 

	// write 0x1d in SROM_enable reg for initializing
	writeReg(Register::SROM_Enable, 0x1d); 

	// wait for more than one frame period
	modm::delayMilliseconds(10); // assume that the frame rate is as low as 100fps... even if it should never be that low

	// write 0x18 to SROM_enable to start SROM download
	writeReg(Register::SROM_Enable, 0x18); 

	// write the SROM file (=firmware data) 
	Cs::reset();

	// write burst destination address
	uint8_t address = static_cast<uint8_t>(Register::SROM_Load_Burst) | 0x80;
	Spi::transferBlocking(address);
	modm::delayMicroseconds(15);

	// send all bytes of the firmware
	for(int ii = 0; ii < firmware_length; ++ii)
	{ 
		Spi::transferBlocking(firmware_data[ii]);
		modm::delayMicroseconds(15);
	}

	Cs::set(); 
}

template < typename Spi, typename Cs >
bool
modm::Adns9800< Spi, Cs >::initialise()
{
	bool success = true;

	// ensure that the serial port is reset
	Cs::set(); 
	Cs::reset();
	Cs::set();

	writeReg(Register::Power_Up_Reset, 0x5a); // force reset
	modm::delayMilliseconds(50); // wait for it to reboot

	// Read Product ID
	uint8_t id = readReg(Register::Product_ID);
	static constexpr uint8_t id_expected = 0x33;
	if (id != id_expected)
	{
		MODM_LOG_ERROR.printf("Product Id = %02x. Expected %02x\n", id, id_expected);
		success = false;
	}

	static constexpr uint8_t rev_expected = 0x03;
	id = readReg(Register::Revision_ID);
	if (id != rev_expected)
	{
		MODM_LOG_DEBUG.printf("Revision Id = %02x. Expected %02x\n", id, rev_expected);
		success = false;
	}

	id = readReg(Register::Inverse_Product_ID);
	if (id != ~id_expected)
	{
		MODM_LOG_DEBUG.printf("Inverse Product Id = %02x. Expected %02x\n", id, ~id_expected);
		success = false;
	}

	// read registers 0x02 to 0x06 (and discard the data)
	readReg(Register::Motion);
	readReg(Register::Delta_X_L);
	readReg(Register::Delta_X_H);
	readReg(Register::Delta_Y_L);
	readReg(Register::Delta_Y_H);

	uploadFirmware();
	modm::delayMilliseconds(10);

	// enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
	// reading the actual value of the register is important because the real
	// default value is different from what is said in the datasheet, and if you
	// change the reserved bytes (like by writing 0x00...) it would not work.

	// Laser always on
	// Do not read Motion

	uint8_t laser_ctrl0 = readReg(Register::LASER_CTRL0);
	writeReg(Register::LASER_CTRL0, (laser_ctrl0 & 0xf0) | (0b0100) );

	// Configure for robotics
	// 8200 cpi
	writeReg(Register::Configuration_I, 0xa4);

	// Fixed frame rate
	writeReg(Register::Configuration_II, 0b00001000);

	writeReg(Register::Frame_Period_Max_Bound_Lower, 0x20);
	writeReg(Register::Frame_Period_Max_Bound_Upper, 0x1b);

	modm::delayMilliseconds(1);

	return success;
}
