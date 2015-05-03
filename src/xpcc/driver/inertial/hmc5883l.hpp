// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HMC5883L_HPP
#define XPCC_HMC5883L_HPP

#include "hmc58x3.hpp"
#include <cmath>

namespace xpcc
{

struct hmc5883 : public hmc58x3
{
	enum class
	MeasurementAverage : uint8_t
	{
		Average8 = int(ConfigA::MA1) | int(ConfigA::MA0),
		Average4 = ConfigA::MA1,
		Average2 = ConfigA::MA0,
		Average1 = 0
	};

	enum class
	Gain : uint8_t
	{
		Ga0_88 = 0,
		Ga1_3 = ConfigB::GN0,
		Ga1_9 = ConfigB::GN1,
		Ga2_5 = int(ConfigB::GN1) | int(ConfigB::GN2),
		Ga4_0 = ConfigB::GN2,
		Ga4_7 = int(ConfigB::GN2) | int(ConfigB::GN0),
		Ga5_6 = int(ConfigB::GN2) | int(ConfigB::GN1),
		Ga8_1 = int(ConfigB::GN2) | int(ConfigB::GN1) | int(ConfigB::GN0),
	};

	enum class
	MeasurementRate : uint8_t
	{
		Hz0_75 = 0,
		Hz1_5 = ConfigA::DO0,
		Hz3 = int(ConfigA::DO1),
		Hz7_5 = int(ConfigA::DO1) | int(ConfigA::DO0),
		Hz15 = ConfigA::DO2,
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

	xpcc::co::ResumableResult<bool> inline
	configure(MeasurementRate rate=MeasurementRate::Hz15,
			  Gain gain=Gain::Ga1_3,
			  MeasurementAverage average=MeasurementAverage::Average1)
	{ return this->configureRaw(uint8_t(rate), uint8_t(gain), gainValues, uint8_t(average)); }

	xpcc::co::ResumableResult<bool> inline
	setMeasurementAverage(MeasurementAverage average)
	{ return this->updateConfigA(ConfigA_t(uint8_t(average)), ConfigA::MA_Mask); }

	xpcc::co::ResumableResult<bool> inline
	setMeasurementRate(MeasurementRate rate)
	{ return this->updateConfigA(ConfigA_t(uint8_t(rate)), ConfigA::DO_Mask); }

	xpcc::co::ResumableResult<bool> inline
	setGain(Gain gain)
	{ return this->setGainRaw(uint8_t(gain), gainValues); }
};

}	// namespace xpcc

#endif // XPCC_HMC5883L_HPP
