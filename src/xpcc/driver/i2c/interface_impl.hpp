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

#ifndef XPCC_I2C__INTERFACE_HPP
	#error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T> bool xpcc::i2c::AsynchronousInterface<T>::error;
template <typename T> T xpcc::i2c::AsynchronousInterface<T>::interface;

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::i2c::AsynchronousInterface<T>::read(
		uint8_t address, uint8_t *data, std::size_t size)
{
	uint8_t *p = data;
	if (interface.start(address | READ))
	{
		for (std::size_t i = 0; i < size; i++)
		{
			bool ack = (i == (size - 1)) ? false : true;
			uint8_t byte = interface.read(ack);
			*p++ = byte;
		}
		error = false;
	}
	else {
		error = true;
	}
	
	interface.stop();
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::i2c::AsynchronousInterface<T>::write(
		uint8_t address, const uint8_t *data, std::size_t size)
{
	error = false;
	if (interface.start(address | WRITE))
	{
		for (std::size_t i = 0; i < size; ++i)
		{
			if (!interface.write(*data++))
			{
				error = true;
				break;
			}
		}
	}
	else {
		error = true;
	}
	
	interface.stop();
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::i2c::AsynchronousInterface<T>::writeRead(uint8_t address, uint8_t *data,
		std::size_t writeSize, std::size_t readSize)
{
	error = true;
	if (interface.start(address | WRITE))
	{
		const uint8_t *writeBuffer = data;
		for (std::size_t i = 0; i < writeSize; ++i)
		{
			if (!interface.write(*writeBuffer++)) {
				goto abort;
			}
		}
		
		uint8_t *readBuffer = data;
		if (interface.start(address | READ))
		{
			for (std::size_t i = 0; i < readSize; i++)
			{
				bool ack = (i == (readSize - 1)) ? false : true;
				uint8_t byte = interface.read(ack);
				*readBuffer++ = byte;
			}
			
			error = false;
		}
	}
	
abort:
	interface.stop();
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::i2c::AsynchronousInterface<T>::isBusy()
{
	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
inline xpcc::i2c::Status
xpcc::i2c::AsynchronousInterface<T>::getStatus()
{
	if (error) {
		return ERROR;
	}
	else {
		return SUCCESS;
	}
}
