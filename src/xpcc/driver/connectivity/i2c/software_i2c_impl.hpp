// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SOFTWARE_I2C_HPP
	#error	"Don't include this file directly, use 'software_i2c.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
Scl xpcc::SoftwareI2C<Scl, Sda, Frequency>::scl;

template <typename Scl, typename Sda, int32_t Frequency>
Sda xpcc::SoftwareI2C<Scl, Sda, Frequency>::sda;

template <typename Scl, typename Sda, int32_t Frequency>
bool xpcc::SoftwareI2C<Scl, Sda, Frequency>::occupied = false;

template <typename Scl, typename Sda, int32_t Frequency>
uint8_t xpcc::SoftwareI2C<Scl, Sda, Frequency>::address;

template <typename Scl, typename Sda, int32_t Frequency>
xpcc::i2c::BusState xpcc::SoftwareI2C<Scl, Sda, Frequency>::busState;

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::start(uint8_t slaveAddress)
{
	if (!occupied){
		occupied = true;
		restart(slaveAddress);
		return true;
	}
	return false;
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::restart(uint8_t slaveAddress)
{
	address = slaveAddress;
	busState = xpcc::i2c::BUS_STANDBY;
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::stop()
{
	occupied = false;
	stopCondition();
}


template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::read(uint8_t *data, std::size_t size, xpcc::i2c::ReadParameter param)
{
	uint8_t *p = data;
	if (startCondition(address | xpcc::i2c::READ))
	{
		for (std::size_t i = 0; i < size; i++)
		{
			bool ack = i != (size - 1);
			uint8_t byte = read(ack);
			*p++ = byte;
		}
		if (param == xpcc::i2c::READ_STOP){
			busState = xpcc::i2c::BUS_STOPPED;
		}
		else{
			busState = xpcc::i2c::BUS_HOLD;
		}
	}
	else {
		busState = xpcc::i2c::BUS_RESET;
	}
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::write(const uint8_t *data, std::size_t size)
{
	bool error = false;
	if (busState == xpcc::i2c::BUS_WRITE || startCondition(address | xpcc::i2c::WRITE))
	{
		for (std::size_t i = 0; i < size; ++i)
		{
			if (!write(*data++))
			{
				error = true;
				break;
			}
		}
	}
	else {
		error = true;
	}

	busState = error ?
			xpcc::i2c::BUS_RESET:
			xpcc::i2c::BUS_WRITE;
}

template <typename Scl, typename Sda, int32_t Frequency>
xpcc::i2c::BusyState
xpcc::SoftwareI2C<Scl, Sda, Frequency>::getBusyState()
{
	return  occupied ?
			xpcc::i2c::OCCUPIED :
			xpcc::i2c::FREE;
}

template <typename Scl, typename Sda, int32_t Frequency>
xpcc::i2c::BusState
xpcc::SoftwareI2C<Scl, Sda, Frequency>::getBusState()
{
	return busState;
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::initialize()
{
	scl.setInput();
	scl.reset();
	
	sda.setInput();
	sda.reset();
}

template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::startCondition(uint8_t address)
{
	if (sda.read() == gpio::LOW){// startcondition needs a high sda and scl, so we adjust here
		if (scl.read() == gpio::HIGH){
			// a very illegal state
			// avoid generating a stop condition and hope no other is writing on the bus
			scl.setOutput();
			delay();
			sda.setInput();
			delay();
			scl.setInput();
			delay();
		}
		else { //  we are doing a restart
			sda.setInput();
			delay();
		}
	}
	else if(scl.read() == gpio::LOW){
		scl.setInput();
		delay();
	}

	// here both pins are HIGH, ready for start
	sda.setOutput();
	delay();
	scl.setOutput();
	delay();
	
	return write(address);
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::stopCondition()
{
	scl.setOutput();
	sda.setOutput();
	
	delay();
	scl.setInput();
	delay();
	sda.setInput();
	delay();
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::write(uint8_t data)
{
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x80);
		data <<= 1;
	}
	
	// release sda
	sda.setInput();
	
	// return acknowledge bit
	return !readBit();
}

template <typename Scl, typename Sda, int32_t Frequency>
uint8_t
xpcc::SoftwareI2C<Scl, Sda, Frequency>::read(bool ack)
{
	sda.setInput();
	
	uint8_t data = 0;
	for(uint8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (readBit()) {
			data |= 0x01;
		}
	}
	
	// generate acknowledge bit
	writeBit(!ack);
	
	// release sda
	sda.setInput();
	
	return data;
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::readBit()
{
	delay();
	scl.setInput();
	
	delay();
	while (scl.read() == gpio::LOW)
		;
	
	bool bit = sda.read();
	
	scl.setOutput();
	
	return bit;
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::writeBit(bool bit)
{
	if (bit) {
		sda.setInput();
	}
	else {
		sda.setOutput();
	}
	delay();
	
	scl.setInput();
	delay();
	
	while (scl.read() == gpio::LOW)
		;
	
	scl.setOutput();
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}
