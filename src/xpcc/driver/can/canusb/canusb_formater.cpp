#include "canusb_formater.hpp"

#include <stdint.h>
#include <cstring>


bool
xpcc::CanUsbFormater::convertToCanMessage(const char* in,can::Message& out)
{
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

	// read the messge-identifier
	if (out.flags.extended)
	{
		uint16_t id;
		uint16_t id2;

		id  = hexToByte(&in[1]) << 8;
		id |= hexToByte(&in[3]);

		id2  = hexToByte(&in[5]) << 8;
		id2 |= hexToByte(&in[7]);

		out.identifier = (uint32_t) id << 16 | id2;
	}
	else {
		uint16_t id;

		id  = charToByte(&in[1]) << 8;
		id |= hexToByte(&in[2]);

		out.identifier = id;
	}

	// read data if the message is no rtr-frame
	if (!out.flags.rtr)
	{
		const char *buf = &in[dlc_pos + 1];
		uint8_t i;

		for (i=0; i < out.length; i++)
		{
			out.data[i] = hexToByte(buf);
			buf += 2;
		}
	}
	return true;
}


bool
xpcc::CanUsbFormater::convertToString(const can::Message& in, char* out)
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
		dataBegin=10;
	}
	else
	{
		out[1] = byteToHex(*(ptr+1));
		out[2] = byteToHex((*(ptr))>>4);
		out[3] = byteToHex((*(ptr)));
		out[4] = byteToHex(in.length);
		dataBegin=5;

	}

	if (!in.flags.rtr)
	{
		for(uint_fast8_t i = 0 ; i < in.length ; i++)
		{
			out[dataBegin+2*i] = byteToHex(in.data[i]>>4);
			out[dataBegin+2*i+1] = byteToHex(in.data[i]);
		}
		out[dataBegin+2*in.length]='\0';
	}

	return true;
}


uint8_t
xpcc::CanUsbFormater::charToByte(const char *s)
{
	uint8_t t = *s;
	if (t >= 'a')
		t = t - 'a' + 10;
	else if (t >= 'A')
		t = t - 'A' + 10;
	else
		t = t - '0';
	return t;
}

char
xpcc::CanUsbFormater::byteToHex(uint8_t num)
{
	num &= 0x0f;
	if(num<10)
		return '0' + num;
	else
		return  'A' + (num-10);
}
