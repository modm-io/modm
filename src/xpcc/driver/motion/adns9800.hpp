/*
 * Copyright (c) 2016, Sascha Schade
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

#ifndef XPCC_ADNS9800_HPP
#define XPCC_ADNS9800_HPP

namespace xpcc
{

struct adns9800
{
public:
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		Product_ID                   = 0x00,
		Revision_ID                  = 0x01,
		Motion                       = 0x02,
		Delta_X_L                    = 0x03,
		Delta_X_H                    = 0x04,
		Delta_Y_L                    = 0x05,
		Delta_Y_H                    = 0x06,
		SQUAL                        = 0x07,
		Pixel_Sum                    = 0x08,
		Maximum_Pixel                = 0x09,
		Minimum_Pixel                = 0x0a,
		Shutter_Lower                = 0x0b,
		Shutter_Upper                = 0x0c,
		Frame_Period_Lower           = 0x0d,
		Frame_Period_Upper           = 0x0e,
		Configuration_I              = 0x0f,
		Configuration_II             = 0x10,
		Frame_Capture                = 0x12,
		SROM_Enable                  = 0x13,
		Run_Downshift                = 0x14,
		Rest1_Rate                   = 0x15,
		Rest1_Downshift              = 0x16,
		Rest2_Rate                   = 0x17,
		Rest2_Downshift              = 0x18,
		Rest3_Rate                   = 0x19,
		Frame_Period_Max_Bound_Lower = 0x1a,
		Frame_Period_Max_Bound_Upper = 0x1b,
		Frame_Period_Min_Bound_Lower = 0x1c,
		Frame_Period_Min_Bound_Upper = 0x1d,
		Shutter_Max_Bound_Lower      = 0x1e,
		Shutter_Max_Bound_Upper      = 0x1f,
		LASER_CTRL0                  = 0x20,
		Observation                  = 0x24,
		Data_Out_Lower               = 0x25,
		Data_Out_Upper               = 0x26,
		SROM_ID                      = 0x2a,
		Lift_Detection_Thr           = 0x2e,
		Configuration_V              = 0x2f,
		Configuration_IV             = 0x39,
		Power_Up_Reset               = 0x3a,
		Shutdown                     = 0x3b,
		Inverse_Product_ID           = 0x3f,
		Motion_Burst                 = 0x50,
		SROM_Load_Burst              = 0x62,
		Pixel_Burst                  = 0x64,
	};
};

template < typename Spi, typename Cs >
class Adns9800 : public adns9800
{
public:
	static bool
	initialise();

	static bool
	isNewMotionDataAvailable();

	static void
	getDeltaXY(int16_t &delta_x, int16_t &delta_y);

protected:       
	static uint8_t
	readReg(Register const reg);

	static void
	writeReg(Register const reg, uint8_t const data);

	static void
	uploadFirmware();
};

} // xpcc namespace

#include "adns9800_impl.hpp"

#endif // XPCC_ADNS9800_HPP
