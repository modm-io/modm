// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HMC5843_HPP
#define XPCC_HMC5843_HPP

#include "hmc58.hpp"
#include <cmath>

namespace xpcc
{

struct hmc5843 : public hmc58x3
{
	enum class
	Gain : uint8_t
	{
		Ga0_7 = 0,
		Ga1_0 = ConfigB::GN0,
		Ga1_5 = ConfigB::GN1,
		Ga2_0 = int(ConfigB::GN1) | int(ConfigB::GN0),
		Ga3_2 = ConfigB::GN2,
		Ga3_8 = int(ConfigB::GN2) | int(ConfigB::GN0),
		Ga4_5 = int(ConfigB::GN2) | int(ConfigB::GN1),
		Ga6_5 = int(ConfigB::GN2) | int(ConfigB::GN1) | int(ConfigB::GN0),
	};

	enum class
	MeasurementRate : uint8_t
	{
		Hz0_5 = 0,
		Hz1 = ConfigA::DO0,
		Hz2 = int(ConfigA::DO1),
		Hz5 = int(ConfigA::DO1) | int(ConfigA::DO0),
		Hz10 = ConfigA::DO2,
		Hz20 = int(ConfigA::DO2) | int(ConfigA::DO0),
		Hz50 = int(ConfigA::DO2) | int(ConfigA::DO1),
	};

};	// struct hmc5843

/**
 * HMC5843 digital compass driver.
 *
 * This class extends the Hmc58x3 family driver only by the addition of
 * device specific Register bit masks.
 *
 * For further information on this device consult the
 * <a href="http://www.sparkfun.com/datasheets/Sensors/Magneto/HMC5843.pdf">
 * datasheet</a>.
 *
 * @see Hmc58x3
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < typename I2cMaster >
class Hmc5843 : public hmc5843, public Hmc58x3< I2cMaster >
{
	/// binary scaled gain values (scalar is 30)
	static constexpr uint8_t gainValues[8] = {
			lround(0.7 * 30),
			lround(1.0 * 30),
			lround(1.5 * 30),
			lround(2.0 * 30),
			lround(3.2 * 30),
			lround(3.8 * 30),
			lround(4.5 * 30),
			lround(6.5 * 30),
	};

public:
	/// Constructor, requires a hmc58x3::Data object, sets address to default of 0x1e
	Hmc5843(Data &data, uint8_t address=0x1e)
	:	Hmc58x3<I2cMaster>(data, address)
	{
	}

	xpcc::co::Result<bool> inline
	configure(MeasurementRate rate=MeasurementRate::Hz10, Gain gain=Gain::Ga1_0)
	{ return this->configureRaw(uint8_t(rate), uint8_t(gain), gainValues); }

	xpcc::co::Result<bool> inline
	setMeasurementRate(MeasurementRate rate)
	{ return this->updateConfigA(ConfigA_t(uint8_t(rate)), ConfigA::DO_Mask); }

	xpcc::co::Result<bool> inline
	setGain(Gain gain)
	{ return this->setGainRaw(uint8_t(gain), gainValues); }
};

}	// namespace xpcc

#endif // XPCC_HMC5843_HPP
