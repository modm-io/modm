/*
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2011-2012, 2015-2016, Niklas Hauser
 * Copyright (c) 2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HMC5883L_HPP
#define MODM_HMC5883L_HPP

#include "hmc58x3.hpp"
#include <cmath>

namespace modm
{

struct hmc5883 : public hmc58x3
{
	enum class
	MeasurementAverage : uint8_t
	{
		Average8 = int(ConfigA::MA1) | int(ConfigA::MA0),
		Average4 = int(ConfigA::MA1),
		Average2 = int(ConfigA::MA0),
		Average1 = 0
	};

	enum class
	Gain : uint8_t
	{
		Ga0_88 = 0,
		Ga1_3 = int(ConfigB::GN0),
		Ga1_9 = int(ConfigB::GN1),
		Ga2_5 = int(ConfigB::GN1) | int(ConfigB::GN2),
		Ga4_0 = int(ConfigB::GN2),
		Ga4_7 = int(ConfigB::GN2) | int(ConfigB::GN0),
		Ga5_6 = int(ConfigB::GN2) | int(ConfigB::GN1),
		Ga8_1 = int(ConfigB::GN2) | int(ConfigB::GN1) | int(ConfigB::GN0),
	};

	enum class
	MeasurementRate : uint8_t
	{
		Hz0_75 = 0,
		Hz1_5 = int(ConfigA::DO0),
		Hz3 = int(ConfigA::DO1),
		Hz7_5 = int(ConfigA::DO1) | int(ConfigA::DO0),
		Hz15 = int(ConfigA::DO2),
		Hz30 = int(ConfigA::DO2) | int(ConfigA::DO0),
		Hz75 = int(ConfigA::DO2) | int(ConfigA::DO1),
	};
};	// struct hmc5883

/**
 * HMC5843 digital compass driver.
 *
 * This class extends the Hmc58x3 family driver only by the addition of
 * device specific Register bit masks.
 *
 * For further information on this device consult the
 * <a href="http://www.insed.de/HMC5883L.pdf">
 * datasheet</a>.
 *
 * @see Hmc58x3
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < typename I2cMaster >
class Hmc5883 : public hmc5883, public Hmc58x3< I2cMaster >
{
	/// binary scaled gain values (scalar is 30)
	static constexpr uint8_t gainValues[8] = {
			lround(0.88 * 30),
			lround(1.3 * 30),
			lround(1.9 * 30),
			lround(2.5 * 30),
			lround(4.0 * 30),
			lround(4.7 * 30),
			lround(5.6 * 30),
			lround(8.1 * 30),
	};

public:
	/// Constructor, requires a hmc58x3::Data object, sets address to default of 0x1e
	Hmc5883(Data &data, uint8_t address=0x1e)
	:	Hmc58x3<I2cMaster>(data, address)
	{
	}

	modm::ResumableResult<bool> inline
	configure(MeasurementRate rate=MeasurementRate::Hz15,
			  Gain gain=Gain::Ga1_3,
			  MeasurementAverage average=MeasurementAverage::Average1)
	{ return this->configureRaw(uint8_t(rate), uint8_t(gain), gainValues, uint8_t(average)); }

	modm::ResumableResult<bool> inline
	setMeasurementAverage(MeasurementAverage average)
	{ return this->updateConfigA(ConfigA_t(uint8_t(average)), ConfigA::MA_Mask); }

	modm::ResumableResult<bool> inline
	setMeasurementRate(MeasurementRate rate)
	{ return this->updateConfigA(ConfigA_t(uint8_t(rate)), ConfigA::DO_Mask); }

	modm::ResumableResult<bool> inline
	setGain(Gain gain)
	{ return this->setGainRaw(uint8_t(gain), gainValues); }
};

}	// namespace modm

#endif // MODM_HMC5883L_HPP
