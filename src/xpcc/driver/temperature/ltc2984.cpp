// coding: utf-8
/* Copyright (c) 2017, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "ltc2984.hpp"

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const ltc2984::Data& c)
{
	s.printf("%.3f", (double)c.getTemperatureFloat());
	s << " C " << c.getStatus();
	return s;
}

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const ltc2984::Data::Status& c)
{
	s	<< "( ";
	if(c & ltc2984::Data::Status::ValidIf1)
		s << "valid ";
	if(c & ltc2984::Data::Status::SensorHardFault)
		s << "sensorHardFault ";
	if(c & ltc2984::Data::Status::AdcHardFault)
		s << "adcHardFault ";
	if(c & ltc2984::Data::Status::CJHardFault)
		s << "CJHardFault ";
	if(c & ltc2984::Data::Status::CJSoftFault)
		s << "CJSoftFault ";
	if(c & ltc2984::Data::Status::SensorOverRangeFault)
		s << "sensorOverRangeFault ";
	if(c & ltc2984::Data::Status::SensorUnderRangeFault)
		s << "sensorUnderRangeFault ";
	if(c & ltc2984::Data::Status::AdcOutOfRangeFault)
		s << "adcOutOfRangeFault ";
	s	<< ")";
	return s;
}
