/*
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "can_lawicel_formatter.hpp"

#include <stdint.h>
#include <cstring>


bool
modm::CanLawicelFormatter::convertToCanMessage(const char* in, can::Message& out)
{
	bool error = false;
	uint8_t dlc_pos;

	if (in[0] == 'R' || in[0] == 'T') {
		out.flags.extended = true;
		dlc_pos = 9;
	}
	else {
		out.flags.extended = false;
		dlc_pos = 4;
	}

	if (std::strlen(in) < dlc_pos + 1U)
		return false;

	// get the number of data-bytes for this message
	out.length = in[dlc_pos] - '0';
	if (out.length > 8)
		return false;		// too many data-bytes

	if (in[0] == 'r' || in[0] == 'R') {
		out.flags.rtr = true;
		if (std::strlen(in) != (dlc_pos + 1U))
			return false;
	}
	else {
		out.flags.rtr = false;
		if (std::strlen(in) != (out.length * 2 + dlc_pos + 1U))
			return false;
	}

	// read the message-identifier
	if (out.flags.extended)
	{
		uint16_t id;
		uint16_t id2;

		id  = hexToByte(&in[1], error) << 8;
		id |= hexToByte(&in[3], error);

		id2  = hexToByte(&in[5], error) << 8;
		id2 |= hexToByte(&in[7], error);

		out.identifier = (uint32_t) id << 16 | id2;

		// check that id does not exceed 29 bits (0x1fffffff)
		if(out.identifier > 0x1fffffff)
			return false;
	}
	else {
		uint16_t id;

		id  = charToByte(in[1], error) << 8;
		id |= hexToByte(&in[2], error);

		out.identifier = id;

		// check that id does not exceed 11 bits (0x7ff)
		if(out.identifier > 0x7ff)
			return false;
	}

	// read data if the message is no rtr-frame
	if (!out.flags.rtr)
	{
		const char *buf = &in[dlc_pos + 1];
		uint8_t i;

		for (i=0; i < out.length; i++)
		{
			out.data[i] = hexToByte(buf, error);
			buf += 2;
		}
	}
	return !error;
}


bool
modm::CanLawicelFormatter::convertToString(const can::Message& in, char* out)
{
	if(in.flags.extended){
		if(in.flags.rtr){
			out[0]='R';
		}
		else{
			out[0]='T';
		}
	}
	else{
		if(in.flags.rtr){
			out[0]='r';
		}
		else{
			out[0]='t';
		}
	}

	const uint8_t* ptr=reinterpret_cast<const uint8_t*>(&in.identifier);
	uint8_t dataBegin;
	if(in.flags.extended)
	{
		for(int i=3; i>=0; i--){
			out[2*i+1] = byteToHex((*ptr)>>4);
			out[2*i+2] = byteToHex(*ptr);
			++ptr;
		}
		out[9] = byteToHex(in.length);
		out[10] = '\0';
		dataBegin=10;
	}
	else
	{
		out[1] = byteToHex(*(ptr+1));
		out[2] = byteToHex((*(ptr))>>4);
		out[3] = byteToHex((*(ptr)));
		out[4] = byteToHex(in.length);
		out[5] = '\0';	// terminate if no data is appended.
		dataBegin=5;

	}

	if (!in.flags.rtr)
	{
		uint_fast8_t i = 0;
		for( ; i < in.length ; i++)
		{
			out[dataBegin+2*i] = byteToHex(in.data[i]>>4);
			out[dataBegin+2*i+1] = byteToHex(in.data[i]);
		}
		out[dataBegin+2*i]='\0';
	}

	return true;
}


uint8_t
modm::CanLawicelFormatter::charToByte(const char cc, bool& error)
{
	if(cc >= '0' && cc <= '9') {
		return cc - '0';
	} else if(cc >= 'A' && cc <= 'F') {
		return cc - 'A' + 10;
	} else if(cc >= 'a' && cc <= 'f') {
		return cc - 'a' + 10;
	} else {
		error = true;
		return 0;
	}
}

char
modm::CanLawicelFormatter::byteToHex(uint8_t num)
{
	num &= 0x0f;
	if(num<10)
		return '0' + num;
	else
		return 'A' + (num-10);
}
