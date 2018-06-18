// coding: utf-8
/*
 * Copyright (c) 2017, Raphael Lehmann
 * Copyright (c) 2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "ltc2984.hpp"

// ----------------------------------------------------------------------------
modm::IOStream&
modm::operator << (modm::IOStream& s, const ltc2984::Data& c)
{
	s.printf("%.3f", (double)c.getTemperatureFloat());
	s << " C " << c.getStatus();
	return s;
}

// ----------------------------------------------------------------------------
modm::IOStream&
modm::operator << (modm::IOStream& s, const ltc2984::Data::Status& c)
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
