// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/io/iostream.hpp>
#include <modm/architecture/interface/can.hpp>

namespace modm
{

modm::IOStream&
operator << (modm::IOStream& stream, const modm::Can::BusState state)
{
	switch (state)
	{
		case Can::BusState::Connected:
			stream << "connected";
			break;
		case Can::BusState::ErrorWarning:
			stream << "error warning";
			break;
		case Can::BusState::ErrorPassive:
			stream << "error passive";
			break;
		case Can::BusState::Off:
			stream << "off";
			break;
	}
	return stream;
}

} // modm namespace
