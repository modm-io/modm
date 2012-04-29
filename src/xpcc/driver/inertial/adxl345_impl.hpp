// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC__ADXL345_HPP
#	error  "Don't include this file directly, use 'adxl345.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::ADXL345<I2cMaster>::ADXL345(uint8_t address)
:	readPointer(data), isReading(false), isWriteRead(false),
	newData(false), deviceAddress(address << 1)
{
}

template < typename I2cMaster >
bool
xpcc::ADXL345<I2cMaster>::initialize(adxl345::Bandwidth bandwidth, bool streamMode)
{
	bool ok = writeRegister(adxl345::REGISTER_POWER_CTL, adxl345::POWER_MEASURE);
	ok &= writeRegister(adxl345::REGISTER_DATA_FORMAT, adxl345::DATAFORMAT_FULL_RES);
	ok &= writeRegister(adxl345::REGISTER_BW_RATE, bandwidth);
//	ok &= writeRegister(adxl345::REGISTER_INT_ENABLE, adxl345::INTERRUPT_DATA_READY);
	if (streamMode) ok &= writeRegister(adxl345::REGISTER_FIFO_CTL, adxl345::FIFO_CTL_MODE_STREAM);
	return ok;
}

template < typename I2cMaster >
bool
xpcc::ADXL345<I2cMaster>::readAccelerometer()
{
	buffer[0] = adxl345::REGISTER_DATA_X0;
	writeSize = 1;
	readSize = 6;
	isWriteRead = true;
	
	return I2cMaster::start(this);
}

template < typename I2cMaster >
bool
xpcc::ADXL345<I2cMaster>::isDataReady()
{
	return readRegister(adxl345::REGISTER_INT_SOURCE) & adxl345::INTERRUPT_DATA_READY;
}

template < typename I2cMaster >
bool
xpcc::ADXL345<I2cMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename I2cMaster >
uint8_t*
xpcc::ADXL345<I2cMaster>::getData()
{
	newData = false;
	return data;
}

// ----------------------------------------------------------------------------
template < typename I2cMaster >
bool
xpcc::ADXL345<I2cMaster>::writeRegister(adxl345::Register reg, uint8_t data)
{
	buffer[0] = reg;
	buffer[1] = data;
	writeSize = 2;
	readSize = 0;
	isWriteRead = false;
	
	return I2cMaster::startSync(this);
}

template < typename I2cMaster >
uint8_t
xpcc::ADXL345<I2cMaster>::readRegister(adxl345::Register reg)
{
	buffer[0] = reg;
	writeSize = 1;
	readPointer = buffer;
	readSize = 1;
	isWriteRead = true;
	
	if (I2cMaster::startSync(this))
	{
		return buffer[0];
	}
	return 0;
}

// MARK: delegate
template < typename I2cMaster >
bool
xpcc::ADXL345<I2cMaster>::attaching()
{
	return true;
}

template < typename I2cMaster >
xpcc::i2c::Delegate::Starting
xpcc::ADXL345<I2cMaster>::started()
{
	Starting s;
	s.address = deviceAddress;
	s.next = isReading ? READ_OP : WRITE_OP;
	isReading = !isReading;
	return s;
}

template < typename I2cMaster >
xpcc::i2c::Delegate::Writing
xpcc::ADXL345<I2cMaster>::writing()
{
	Writing w;
	w.buffer = buffer;
	w.size = writeSize;
	w.next = isWriteRead ? WRITE_RESTART : WRITE_STOP;
	return w;
}

template < typename I2cMaster >
xpcc::i2c::Delegate::Reading
xpcc::ADXL345<I2cMaster>::reading()
{
	Reading r;
	r.buffer = readPointer;
	r.size = readSize;
	r.next = READ_STOP;
	return r;
}

template < typename I2cMaster >
void
xpcc::ADXL345<I2cMaster>::stopped(DetachCause cause)
{
	isReading = false;
	readPointer = data;
	
	if (cause == NORMAL_STOP && readSize == 6)
		newData = true;
}

