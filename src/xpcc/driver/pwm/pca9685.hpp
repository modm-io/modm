// coding: utf-8
// ----------------------------------------------------------------------------
 /*
  * Copyright (c) 2015, Christian Menard
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  *     * Redistributions of source code must retain the above copyright
  *       notice, this list of conditions and the following disclaimer.
  *     * Redistributions in binary form must reproduce the above copyright
  *       notice, this list of conditions and the following disclaimer in the
  *       documentation and/or other materials provided with the distribution.
  *     * Neither the name of the copyright holder nor the names of its
  *       contributors may be used to endorse or promote products derived from
  *       this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */
// ----------------------------------------------------------------------------

#ifndef XPCC__PCA9685_HPP
#define XPCC__PCA9685_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>

namespace xpcc
{

namespace pca9685
{
	enum Register
	{
		REG_MODE1         = 0x00,
		REG_MODE2         = 0x01,
		REG_SUBADR1       = 0x02,
		REG_SUBADR2       = 0x03,
		REG_SUBADR3       = 0x04,
		REG_ALLCALLADR    = 0x05,
		REG_LED0_ON_L     = 0x06,
		REG_LED0_ON_H     = 0x07,
		REG_LED0_OFF_L    = 0x08,
		REG_LED0_OFF_H    = 0x09,
		REG_LED1_ON_L     = 0x0a,
		REG_LED1_ON_H     = 0x0b,
		REG_LED1_OFF_L    = 0x0c,
		REG_LED1_OFF_H    = 0x0d,
		REG_LED2_ON_L     = 0x0e,
		REG_LED2_ON_H     = 0x0f,
		REG_LED2_OFF_L    = 0x10,
		REG_LED2_OFF_H    = 0x11,
		REG_LED3_ON_L     = 0x12,
		REG_LED3_ON_H     = 0x13,
		REG_LED3_OFF_L    = 0x14,
		REG_LED3_OFF_H    = 0x15,
		REG_LED4_ON_L     = 0x16,
		REG_LED4_ON_H     = 0x17,
		REG_LED4_OFF_L    = 0x18,
		REG_LED4_OFF_H    = 0x19,
		REG_LED5_ON_L     = 0x1a,
		REG_LED5_ON_H     = 0x1b,
		REG_LED5_OFF_L    = 0x1c,
		REG_LED5_OFF_H    = 0x1d,
		REG_LED6_ON_L     = 0x1e,
		REG_LED6_ON_H     = 0x1f,
		REG_LED6_OFF_L    = 0x20,
		REG_LED6_OFF_H    = 0x21,
		REG_LED7_ON_L     = 0x22,
		REG_LED7_ON_H     = 0x23,
		REG_LED7_OFF_L    = 0x24,
		REG_LED7_OFF_H    = 0x25,
		REG_LED8_ON_L     = 0x26,
		REG_LED8_ON_H     = 0x27,
		REG_LED8_OFF_L    = 0x28,
		REG_LED8_OFF_H    = 0x29,
		REG_LED9_ON_L     = 0x2a,
		REG_LED9_ON_H     = 0x2b,
		REG_LED9_OFF_L    = 0x2c,
		REG_LED9_OFF_H    = 0x2d,
		REG_LED10_ON_L    = 0x2e,
		REG_LED10_ON_H    = 0x2f,
		REG_LED10_OFF_L   = 0x30,
		REG_LED10_OFF_H   = 0x31,
		REG_LED11_ON_L    = 0x32,
		REG_LED11_ON_H    = 0x33,
		REG_LED11_OFF_L   = 0x34,
		REG_LED11_OFF_H   = 0x35,
		REG_LED12_ON_L    = 0x36,
		REG_LED12_ON_H    = 0x37,
		REG_LED12_OFF_L   = 0x38,
		REG_LED12_OFF_H   = 0x39,
		REG_LED13_ON_L    = 0x3a,
		REG_LED13_ON_H    = 0x3b,
		REG_LED13_OFF_L   = 0x3c,
		REG_LED13_OFF_H   = 0x3d,
		REG_LED14_ON_L    = 0x3e,
		REG_LED14_ON_H    = 0x3f,
		REG_LED14_OFF_L   = 0x40,
		REG_LED14_OFF_H   = 0x41,
		REG_LED15_ON_L    = 0x42,
		REG_LED15_ON_H    = 0x43,
		REG_LED15_OFF_L   = 0x44,
		REG_LED15_OFF_H   = 0x45,
		/*
		 * 0x46 - 0xf9 reserved for future use
		 */
		REG_ALL_LED_ON_L  = 0xfa,
		REG_ALL_LED_ON_H  = 0xfb,
		REG_ALL_LED_OFF_L = 0xfc,
		REG_ALL_LED_OFF_H = 0xfd,
		REG_PRE_SCALE     = 0xfe,
		REG_TestMode      = 0xfe,
	};

	enum Mode1 {
		MODE1_RESTART = 0x80,
		MODE1_EXTCLK  = 0x40,
		MODE1_AI      = 0x20,
		MODE1_SLEEP   = 0x10,
		MODE1_SUB1    = 0x08,
		MODE1_SUB2    = 0x04,
		MODE1_SUB3    = 0x02,
		MODE1_ALLCALL = 0x01,
	};

	enum Mode2 {
		MODE2_INVRT   = 0x10,
		MODE2_OCH     = 0x08,
		MODE2_OUTDRV  = 0x04,
		MODE2_OUTNE1  = 0x02,
		MODE2_OUTNE0  = 0x01,
	};
}	// namespace pca9685

/**
 * PCA9685 16-channel, 12-bit PWM LED controller, I2C-bus
 *
 * This class allows for basic (and for most use cases sufficient) control
 * of a PCA9685. It implements initialization (setting of MODE1 and MODE2
 * registers), writing values for single channels, and writing a value for
 * all channels.
 *
 * This driver has the following limitations:
 *  - no register read access
 *  - no arbitrary register write access
 *  - no address reconfiguration
 *  - registers LED*_ON_* are fixed at 0
 *
 * @tparam I2cMaster I2C interface
 *
 * @author Christian Menard
 * @ingroup driver_pwm
 */
template<typename I2cMaster>
class Pca9685 : public xpcc::I2cDevice< I2cMaster, 1, I2cWriteAdapter >
{
	uint8_t buffer[3];

public:
	/**
	 * Constructor.
	 *
	 * @param address I2C address
	 */
	Pca9685(uint8_t address);

	/**
	 * Initialize the device.
	 *
	 * Note: The bit AI (Auto Increment) in MODE2 register will always be
	 * set as it is essential for correct operation of this driver.
	 *
	 * @param mode1 value to be written to MODE1 register
	 * @param mode2 value to be written to MODE2 register
	 */
	xpcc::co::Result<bool>
	initialize(uint8_t mode1 = 0, uint8_t mode2 = 0);

	/**
	 * Set the 12-bit PWM value of a single channel.
	 *
	 * Checks if the specified channel is valid (return false otherwise)
	 * and masks out the upper 4 bits of value to ensure that always a
	 * 12-bit value is written.
	 *
	 * @param channel one of the 16 channels (0-15)
	 * @param value   12-bit PWM value to be written
	 */
	xpcc::co::Result<bool>
	setChannel(uint8_t channel, uint16_t value);

	/**
	 * Set all 16 12-bit PWM channels to the same value.
	 *
	 * The upper 4 bits of value are masked out to ensure that always a
	 * 12-bit value is written.
	 *
	 * @param value 12-bit PWM value to be written
	 */
	xpcc::co::Result<bool>
	setAllChannels(uint16_t value);
};

}	// namespace xpcc

#include "pca9685_impl.hpp"

#endif // XPCC__PCA9685_HPP
