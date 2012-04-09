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

#ifndef XPCC__I2C_EEPROM_HPP
	#error	"Don't include this file directly, use 'i2c_eeprom.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2C>
xpcc::I2cEeprom<I2C>::I2cEeprom(uint8_t deviceAddress):
	deviceAddress(deviceAddress)
{
}

// ----------------------------------------------------------------------------
template <typename I2C>
bool
xpcc::I2cEeprom<I2C>::writeByte(uint16_t address, uint8_t data) const
{
	if (MySyncI2C::startCheck(this->deviceAddress))
	{
		uint8_t buffer[] = {address >> 8, address & 0xff, data};
		return MySyncI2C::write(buffer,3) != xpcc::i2c::BUS_RESET;
	}
	else
		return false;
}

template <typename I2C>
bool
xpcc::I2cEeprom<I2C>::write(uint16_t address, const uint8_t *data, uint8_t bytes) const
{
	if (MySyncI2C::startCheck(this->deviceAddress))
	{
		uint8_t buffer[] = { (uint8_t)(address >> 8), (uint8_t)(address & 0xff) };
		if (MySyncI2C::write(buffer, 2, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET)
		{
			MySyncI2C::stop();
			return false;
		}
		
		return MySyncI2C::write(data, bytes) != xpcc::i2c::BUS_RESET;
	}
	else
		return false;
}

template <typename I2C> template <typename T>
bool
xpcc::I2cEeprom<I2C>::write(uint16_t address, const T& data) const
{
	return write(address, static_cast<const uint8_t *>(&data), sizeof(T));
}

// ----------------------------------------------------------------------------
template <typename I2C>
bool
xpcc::I2cEeprom<I2C>::readByte(uint16_t address, uint8_t &data) const
{
	if (MySyncI2C::startCheck(this->deviceAddress))
	{
		uint8_t buffer[] = {(uint8_t)(address >> 8), (uint8_t)(address & 0xff)};
		if (MySyncI2C::write(buffer, 2, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET) {
			MySyncI2C::stop();
			return false;
		}
		
		if (MySyncI2C::restart(this->deviceAddress) == xpcc::i2c::BUS_RESET) {
			MySyncI2C::stop();
			return false;
		}

		return MySyncI2C::read(&data, 1) != xpcc::i2c::BUS_RESET;
	}
	else
		return false;
}

template <typename I2C>
bool
xpcc::I2cEeprom<I2C>::read(uint16_t address, uint8_t *data, uint8_t bytes) const
{
	if (MySyncI2C::startCheck(this->deviceAddress))
	{
		uint8_t buffer[] = {(uint8_t)(address >> 8), (uint8_t)(address & 0xff)};
		if (MySyncI2C::write(buffer, 2, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return false;
		}
		
		if (MySyncI2C::restart(this->deviceAddress) == xpcc::i2c::BUS_RESET){
			MySyncI2C::stop();
			return false;
		}

		return MySyncI2C::read(data, bytes) != xpcc::i2c::BUS_RESET;
	}
	else
		return false;
}

template <typename I2C> template <typename T>
bool
xpcc::I2cEeprom<I2C>::read(uint16_t address, T& data) const
{
	return read(address, static_cast<uint8_t *>(&data), sizeof(T));
}

// ----------------------------------------------------------------------------
template <typename I2C>
bool
xpcc::I2cEeprom<I2C>::isAvailable() const
{
	if(MySyncI2C::startCheck(this->deviceAddress))
		return (MySyncI2C::write(0, 0) != xpcc::i2c::BUS_RESET);
	else
		return false;
}
