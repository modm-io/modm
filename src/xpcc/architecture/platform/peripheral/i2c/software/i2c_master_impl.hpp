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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SOFTWARE_I2C_HPP
#	error	"Don't include this file directly, use 'software_i2c.hpp' instead!"
#endif

// debugging for serious dummies
//#define DEBUG_SW_I2C(x) xpcc::lpc::BufferedUart1::write(x)
#define DEBUG_SW_I2C(x)

template <typename Scl, typename Sda, int32_t Frequency>
Scl xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::scl;
template <typename Scl, typename Sda, int32_t Frequency>
Sda xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::sda;

template <typename Scl, typename Sda, int32_t Frequency>
xpcc::I2c::Operation xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::nextOperation;
template <typename Scl, typename Sda, int32_t Frequency>
xpcc::I2cDelegate *xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::myDelegate(0);
template <typename Scl, typename Sda, int32_t Frequency>
xpcc::I2cMaster::Error xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::errorState(xpcc::I2cMaster::Error::NoError);

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
template <xpcc::I2cMaster::DataRate rate>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::initialize()
{
	scl.set();
	sda.set();
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::reset(DetachCause cause)
{
	if (myDelegate) myDelegate->stopped(cause);
	myDelegate = 0;
}

template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::start(xpcc::I2cDelegate *delegate)
{
	if (!myDelegate && delegate && delegate->attaching())
	{
		myDelegate = delegate;
		
		while(1)
		{
			DEBUG_SW_I2C('\n');
			DEBUG_SW_I2C('s');
			startCondition();

			xpcc::I2cDelegate::Starting s = myDelegate->starting();
			uint8_t address = s.address;
			
			address &= 0xfe;
			if (s.next == xpcc::I2c::Operation::Read)
				address |= xpcc::I2c::READ;
			
			if (!write(address))
				return true;
			if (nextOperation == xpcc::I2c::Operation::Restart) {DEBUG_SW_I2C('R');}
			nextOperation = s.next;
			
			do {
				switch (nextOperation)
				{
					case xpcc::I2c::Operation::Read:
					{
						xpcc::I2cDelegate::Reading r = myDelegate->reading();
						for (uint8_t i=0; i < r.size-1; ++i) {
							*r.buffer++ = read(true);
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('a');
						}
						*r.buffer = read(false);
						DEBUG_SW_I2C('\n');
						DEBUG_SW_I2C('n');
						nextOperation = static_cast<xpcc::I2c::Operation>(r.next);
					}
						break;
						
					case xpcc::I2c::Operation::Write:
					{
						xpcc::I2cDelegate::Writing w = myDelegate->writing();
						for (uint8_t i=0; i < w.size; ++i) {
							if (!write(*w.buffer++))
								return true;
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('A');
						}
						nextOperation = static_cast<xpcc::I2cDelegate::Operation>(w.next);
					}
						break;
					
					case xpcc::I2c::Operation::Stop:
						DEBUG_SW_I2C('S');
						myDelegate->stopped(xpcc::I2c::DetachCause::NormalStop);
						myDelegate = 0;
						stopCondition();
						return true;
						
					default:
						break;
				}
			} while (nextOperation != xpcc::I2c::Operation::Restart);
		}
	}
	return false;
}

template <typename Scl, typename Sda, int32_t Frequency>
xpcc::I2cMaster::Error
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::getErrorState()
{
	return errorState;
}

// MARK: - private
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::error()
{
	DEBUG_SW_I2C('E');
	stopCondition();
	errorState = Error::DataNack;
	reset(true);
}

// MARK: bus control
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::startCondition()
{
	sda.set();
	while((sda.read() == Gpio::LOW))
		;
	delay();

	sclSetAndWait();
	delay();

	// here both pins are HIGH, ready for start
	sda.reset();
	delay();
	scl.reset();
	delay();
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::stopCondition()
{
	scl.reset();
	sda.reset();
	
	delay();
	sclSetAndWait();
	delay();
	sda.set();
	delay();
}

// MARK: byte operations
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::write(uint8_t data)
{
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x80);
		data <<= 1;
	}
	
	// release sda
	sda.set();
	
	// return acknowledge bit
	if (readBit()) {
		DEBUG_SW_I2C('N');
		error();
		return false;
	}
	return true;
}

template <typename Scl, typename Sda, int32_t Frequency>
uint8_t
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::read(bool ack)
{
	sda.set();
	
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
	sda.set();
	
	return data;
}

// MARK: bit operations
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::readBit()
{
	delay();
	sclSetAndWait();
	
	delay();

	bool bit = sda.read();
	
	scl.reset;
	
	return bit;
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::writeBit(bool bit)
{
	if (bit) {
		sda.set();
	}
	else {
		sda.reset();
	}
	delay();
	
	sclSetAndWait();
	
	delay();

	scl.reset();
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::sclSetAndWait()
{
	scl.set();
	// wait for clock stretching by slave
	// only wait a maximum of 250 half clock cycles
	uint_fast8_t deadlockPreventer = 250;
	while (scl.read() == Gpio::LOW && deadlockPreventer)
	{
		xpcc::delay_us(delayTime/2);
		deadlockPreventer--;
		// double the read amount
		if (scl.read() != Gpio::LOW) return;
		xpcc::delay_us(delayTime/2);
	}
	// if extreme clock stretching occurs, then there might be an external error
	if (!deadlockPreventer)
	{
		error();
	}
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2cMaster<Scl, Sda, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}
