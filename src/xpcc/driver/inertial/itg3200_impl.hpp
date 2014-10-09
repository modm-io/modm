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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ITG3200_HPP
#	error  "Don't include this file directly, use 'itg3200.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Itg3200<I2cMaster>::Itg3200(uint8_t* data, uint8_t address)
:	I2cWriteReadAdapter(address), status(0), data(data)
{
	configureWriteRead(buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::configure(uint8_t divider, itg3200::Filter filter, bool interrupt)
{
	buffer[0] = itg3200::REGISTER_SMPLRT_DIV;
	// sample rate register
	buffer[1] = divider;
	// scale and filter register
	buffer[2] = itg3200::SCALE_FS_2000 | filter;
	// interrupt register
	buffer[3] = interrupt ? itg3200::INTERRUPT_RAW_RDY_EN : 0;
	configureWriteRead(buffer, 4, data, 0);
	
	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
void
xpcc::Itg3200<I2cMaster>::readGyroscope()
{
	status |= READ_GYROSCOPE_PENDING;
}

template < typename I2cMaster >
void
xpcc::Itg3200<I2cMaster>::readTemperatureGyroscope()
{
	status |= READ_BOTH_PENDING;
}

template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::isDataReady()
{
	return readRegister(itg3200::REGISTER_STATUS) & itg3200::STATUS_RAW_RDY;
}

template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::isNewDataAvailable()
{
	return status & NEW_GYROSCOPE_DATA;
}

template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::isNewTemperatureDataAvailable()
{
	return status & NEW_TEMPERATURE_DATA;
}

template < typename I2cMaster >
uint8_t*
xpcc::Itg3200<I2cMaster>::getData()
{
	status &= ~NEW_GYROSCOPE_DATA;
	return &data[2];
}

template < typename I2cMaster >
uint8_t*
xpcc::Itg3200<I2cMaster>::getTemperatureData()
{
	status &= ~NEW_TEMPERATURE_DATA;
	return data;
}

template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::setSampleRateDivider(uint8_t divider)
{
	return writeRegister(itg3200::REGISTER_SMPLRT_DIV, divider);
}

template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::setInterrupts(itg3200::Interrupt interrupts)
{
	return writeRegister(itg3200::REGISTER_INTERRUPT, interrupts);
}

template < typename I2cMaster >
void
xpcc::Itg3200<I2cMaster>::update()
{
	if (running != NOTHING_RUNNING)
	{
		switch (getAdapterState())
		{
			case xpcc::I2c::AdapterState::Idle:
				if (running == READ_BOTH_RUNNING) {
					status |= NEW_GYROSCOPE_DATA | NEW_TEMPERATURE_DATA;
				}
				else if (running == READ_GYROSCOPE_RUNNING) {
					status |= NEW_GYROSCOPE_DATA;
				}
			case xpcc::I2c::AdapterState::Error:
				running = NOTHING_RUNNING;
				
			default:
				break;
		}
	}
	else {
		if (status & READ_BOTH_PENDING)
		{
			buffer[0] = itg3200::REGISTER_DATA_T0;
			configureWriteRead(buffer, 1, data, 8);
			
			if (I2cMaster::start(this)) {
				status &= ~READ_BOTH_PENDING;
				running = READ_BOTH_RUNNING;
			}
		}
		else if (status & READ_GYROSCOPE_PENDING)
		{
			buffer[0] = itg3200::REGISTER_DATA_X0;
			configureWriteRead(buffer, 1, data+2, 6);
			
			if (I2cMaster::start(this)) {
				status &= ~READ_GYROSCOPE_PENDING;
				running = READ_GYROSCOPE_RUNNING;
			}
		}
	}
}

// MARK: - private
template < typename I2cMaster >
bool
xpcc::Itg3200<I2cMaster>::writeRegister(itg3200::Register reg, uint8_t value)
{
	while (getAdapterState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	buffer[1] = value;
	configureWriteRead(buffer, 2, data, 0);
	
	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
uint8_t
xpcc::Itg3200<I2cMaster>::readRegister(itg3200::Register reg)
{
	while (getAdapterState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	configureWriteRead(buffer, 1, buffer, 1);
	
	while (!I2cMaster::startBlocking(this))
		;
	return buffer[0];
}

