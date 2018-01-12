// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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
