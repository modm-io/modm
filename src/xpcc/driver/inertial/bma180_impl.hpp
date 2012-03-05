// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: bma180_impl.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BMA180_HPP
#	error  "Don't include this file directly, use 'bma180.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename TwiMaster >
xpcc::BMA180<TwiMaster>::BMA180(uint8_t address):
deviceAddress(address << 1)
{
}

template < typename TwiMaster >
bool
xpcc::BMA180<TwiMaster>::initialize(bma180::Range range, bma180::Bandwidth bandwidth, bma180::ModeConfig mode, bool interrupt)
{
	if (TwiMaster::getBusyState() != xpcc::i2c::FREE) return false;
	
	newData = false;
	// enable image writing
	bool ok = writeRegister(bma180::REGISTER_CTRL0, bma180::EE_W_bm);
	// set range and sample skipping
	ok &= writeMaskedRegister(bma180::REGISTER_OFFSET_LSB1, bma180::RANGE_gm | bma180::SAMPLE_SKIPPING_bm, bma180::SAMPLE_SKIPPING_bm | range);
	// set bandwidth
	ok &= writeRegister(bma180::REGISTER_BW_TCS, bandwidth);
	// set mode configuration
	ok &= writeMaskedRegister(bma180::REGISTER_TCO_Z, bma180::MODE_CONFIG_gm, mode);
	// set interrupt
	if (interrupt) ok &= writeRegister(bma180::REGISTER_CTRL3, bma180::NEW_DATA_INT_bm);
	
	return ok;
}

template < typename TwiMaster >
bool
xpcc::BMA180<TwiMaster>::readAccelerometer()
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		data[0] = bma180::REGISTER_DATA_X0;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(data, 1, 7);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::BMA180<TwiMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename TwiMaster >
uint8_t*
xpcc::BMA180<TwiMaster>::getData()
{
	newData = false;
	return data;
}

template < typename TwiMaster >
bool
xpcc::BMA180<TwiMaster>::reset()
{
	return writeRegister(bma180::REGISTER_RESET, bma180::RESET_gc);
}

template < typename TwiMaster >
bool
xpcc::BMA180<TwiMaster>::writeMaskedRegister(bma180::Register reg, uint8_t mask, uint8_t value)
{
	uint8_t buffer = readRegister(reg);
	return writeRegister(reg, (buffer & ~mask) | value);
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
bool
xpcc::BMA180<TwiMaster>::writeRegister(bma180::Register reg, uint8_t data)
{
	uint8_t buffer[2] = {reg, data};
	if (TwiMaster::startWrite(deviceAddress, buffer, 2))
	{
		while (TwiMaster::getBusyState() != xpcc::i2c::FREE)
			;
		return true;
	}
	return false;
}

template < typename TwiMaster >
uint8_t
xpcc::BMA180<TwiMaster>::readRegister(bma180::Register reg)
{
	uint8_t buffer[1] = {reg};
	if (TwiMaster::startWriteRead(deviceAddress, buffer, 1, 1))
	{
		while (TwiMaster::getBusyState() != xpcc::i2c::FREE)
			;
		return buffer[0];
	}
	return 0;
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
void
xpcc::BMA180<TwiMaster>::twiCompletion(const uint8_t */*data*/, std::size_t /*index*/, bool /*reading*/)
{
	newData = true;
}

