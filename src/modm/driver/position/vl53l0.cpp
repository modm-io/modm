// coding: utf-8
/*
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "vl53l0.hpp"

namespace modm
{

namespace vl53l0_private
{

FLASH_STORAGE(BinaryConfiguration configurationFlash[]) =
{
	// the infamous binary blob of black magic
	{0xFF, 0x01},
	{0x00, 0x00},
	{0xFF, 0x00},
	{0x09, 0x00},
	{0x10, 0x00},
	{0x11, 0x00},
	{0x24, 0x01},
	{0x25, 0xff},
	{0x75, 0x00},
	{0xFF, 0x01},
	{0x4e, 0x2c},
	{0x48, 0x00},
	{0x30, 0x20},
	{0xFF, 0x00},
	{0x30, 0x09},
	{0x54, 0x00},
	{0x31, 0x04},
	{0x32, 0x03},
	{0x40, 0x83},
	{0x46, 0x25},
	{0x60, 0x00},
	{0x27, 0x00},
	{0x50, 0x06},
	{0x51, 0x00},
	{0x52, 0x96},
	{0x56, 0x08},
	{0x57, 0x30},
	{0x61, 0x00},
	{0x62, 0x00},
	{0x64, 0x00},
	{0x65, 0x00},
	{0x66, 0xa0},
	{0xFF, 0x01},
	{0x22, 0x32},
	{0x47, 0x14},
	{0x49, 0xff},
	{0x4a, 0x00},
	{0xFF, 0x00},
	{0x7a, 0x0a},
	{0x7b, 0x00},
	{0x78, 0x21},
	{0xFF, 0x01},
	{0x23, 0x34},
	{0x42, 0x00},
	{0x44, 0xff},
	{0x45, 0x26},
	{0x46, 0x05},
	{0x40, 0x40},
	{0x0E, 0x06},
	{0x20, 0x1a},
	{0x43, 0x40},
	{0xFF, 0x00},
	{0x34, 0x03},
	{0x35, 0x44},
	{0xFF, 0x01},
	{0x31, 0x04},
	{0x4b, 0x09},
	{0x4c, 0x05},
	{0x4d, 0x04},
	{0xFF, 0x00},
	{0x44, 0x00},
	{0x45, 0x20},
	{0x47, 0x08},
	{0x48, 0x28},
	{0x67, 0x00},
	{0x70, 0x04},
	{0x71, 0x01},
	{0x72, 0xfe},
	{0x76, 0x00},
	{0x77, 0x00},
	{0xFF, 0x01},
	{0x0d, 0x01},
	{0xFF, 0x00},
	{0x80, 0x01},
	{0x01, 0xF8},
	{0xFF, 0x01},
	{0x8e, 0x01},
	{0x00, 0x01},
	{0xFF, 0x00},
	{0x80, 0x00},
};
modm::accessor::Flash<BinaryConfiguration> configuration(configurationFlash);

}	// namespace vl53l0_private

}	// namespace modm

// ----------------------------------------------------------------------------
modm::IOStream&
modm::operator << (modm::IOStream& s, const vl53l0::RangeErrorCode& c)
{
	switch(c)
	{
		case vl53l0::RangeErrorCode::NoError: s << "NoError"; break;
		case vl53l0::RangeErrorCode::VCSEL_ContinuityTest: s << "VCSEL_ContinuityTest"; break;
		case vl53l0::RangeErrorCode::VCSEL_WatchdogTest: s << "VCSEL_WatchdogTest"; break;
		case vl53l0::RangeErrorCode::VHV_NoValueFound: s << "VHV_NoValueFound"; break;
		case vl53l0::RangeErrorCode::MSRC_NoTarget: s << "MSRC_NoTarget"; break;
		case vl53l0::RangeErrorCode::SNR_Check: s << "SNR_Check"; break;
		case vl53l0::RangeErrorCode::RangePhaseCheck: s << "RangePhaseCheck"; break;
		case vl53l0::RangeErrorCode::SigmaThresholdCheck: s << "SigmaThresholdCheck"; break;
		case vl53l0::RangeErrorCode::TCC: s << "TCC"; break;
		case vl53l0::RangeErrorCode::PhaseConsistency: s << "PhaseConsistency"; break;
		case vl53l0::RangeErrorCode::MinClip: s << "MinClip"; break;
		case vl53l0::RangeErrorCode::RangeComplete: s << "RangeComplete"; break;
		case vl53l0::RangeErrorCode::RangingAlgoUnderflow: s << "RangingAlgoUnderflow"; break;
		case vl53l0::RangeErrorCode::RangingAlgoOverflow: s << "RangingAlgoOverflow"; break;
		case vl53l0::RangeErrorCode::RangeIgnoreThreshold: s << "RangeIgnoreThreshold"; break;
	}

	return s;
}
