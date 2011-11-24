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

#ifndef XPCC__DS1631_HPP
	#error	"Don't include this file directly, use 'ds1631.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2C>
xpcc::Ds1631<I2C>::Ds1631(uint8_t deviceAddress):
	deviceAddress(deviceAddress)
{
}

// ----------------------------------------------------------------------------
template <typename I2C>
void
xpcc::Ds1631<I2C>::configure(ds1631::Resolution resolution, bool continuousMode)
{
	if(MySyncI2C::startCheck(this->deviceAddress)){
		uint8_t config = resolution;
		if (!continuousMode) {
			config |= 0x01;
		}

		uint8_t buffer[] = {0xac, config};
		MySyncI2C::write(buffer, 2);
	}
}

template <typename I2C>
void
xpcc::Ds1631<I2C>::reset()
{
	if(MySyncI2C::startCheck(this->deviceAddress)){
		uint8_t buffer[] = {0x54};
		MySyncI2C::write(buffer, 1);
	}
}

// ----------------------------------------------------------------------------
template <typename I2C>
void
xpcc::Ds1631<I2C>::startConversion()
{
	if(MySyncI2C::startCheck(this->deviceAddress)){
		uint8_t buffer[] = {0x51};
		MySyncI2C::write(buffer, 1);
	}
}

template <typename I2C>
void
xpcc::Ds1631<I2C>::stopConversion()
{
	if(MySyncI2C::startCheck(this->deviceAddress)){
		uint8_t buffer[] = {0x22};
		MySyncI2C::write(buffer, 1);
	}
}

template <typename I2C>
bool
xpcc::Ds1631<I2C>::isConversionDone()
{
	if(MySyncI2C::startCheck(this->deviceAddress)){
		uint8_t buffer[] = {0xac};
		if (MySyncI2C::write(buffer, 1, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return false;
		}

		if (MySyncI2C::restart(this->deviceAddress) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return false;
		}

		if(MySyncI2C::read(buffer, 1) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return false;
		}

		return ((buffer[0]&0x80) == 0x80);
	}
	else{
		return false;
	}
}

// ----------------------------------------------------------------------------
template <typename I2C>
int16_t
xpcc::Ds1631<I2C>::readTemperature()
{
	if(MySyncI2C::startCheck(this->deviceAddress)){
		{
			uint8_t buffer[] = {0xaa};
			if (MySyncI2C::write(buffer, 1, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET){
				MySyncI2C::stop();
				return 0;
			}
		}

		if (MySyncI2C::restart(this->deviceAddress) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return 0;
		}

		uint8_t buffer[2];
		if(MySyncI2C::read(buffer, 2) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return 0;
		}

		int16_t temperature = buffer[0]<<8|buffer[1];
		return temperature;
	}
	else{
		return 0;
	}
}

// ----------------------------------------------------------------------------
template <typename I2C>
bool
xpcc::Ds1631<I2C>::isAvailable() const
{
	if(MySyncI2C::startCheck(this->deviceAddress))
		return (MySyncI2C::write(0, 0) != xpcc::i2c::BUS_RESET);
	else
		return false;
}
