// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/io/iostream.hpp>
#include <xpcc/architecture/interface/can.hpp>

namespace xpcc
{

xpcc::IOStream&
operator << (xpcc::IOStream& stream, const xpcc::Can::BusState state)
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

} // xpcc namespace
