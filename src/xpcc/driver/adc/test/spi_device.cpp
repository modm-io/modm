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

#include "spi_device.hpp"

// ----------------------------------------------------------------------------
test::SpiDevice::SpiDevice() :
	transmissions(0),
	transmissionCount(0),
	currentTransmission(0),
	bytesWritten(0),
	selected(false),
	successful(true),
	complete(true)
{
}

test::SpiDevice::~SpiDevice()
{
	// TODO delete error handling
}

// ----------------------------------------------------------------------------
void
test::SpiDevice::select()
{
	if (!selected) {
		if (currentTransmission < transmissionCount) {
			selected = true;
			bytesWritten = 0;
		}
		else {
			successful = false;
			// TODO report error
		}
	}
}

void
test::SpiDevice::deselect()
{
	if (selected)
	{
		const Transmission* t = &transmissions[currentTransmission];
		if (bytesWritten != t->length) {
			successful = false;
			// TODO report error
		}
		
		if (currentTransmission < transmissionCount) {
			currentTransmission++;
			if (currentTransmission == transmissionCount) {
				complete = true;
			}
		}
		else {
			successful = false;
			// TODO report error
		}
	}
	selected = false;
}

uint8_t
test::SpiDevice::write(uint8_t data)
{
	uint8_t out = 0xff;
	
	if (selected)
	{
		const Transmission* t = &transmissions[currentTransmission];
		
		if (bytesWritten < t->length) {
			if (data != t->rx[bytesWritten]) {
				successful = false;
				// TODO Error reporting
			}
			out = t->tx[bytesWritten];
		}
		else {
			successful = false;
			// TODO Error reporting
		}
		bytesWritten++;
	}
	else {
		successful = false;
		// TODO Error reporting
	}
	
	return out;
}

// ----------------------------------------------------------------------------
void
test::SpiDevice::start(const Transmission* transmissions,
		std::size_t transmissionCount)
{
	complete = (transmissionCount == 0);
	successful = true;
	
	this->transmissions = transmissions;
	this->transmissionCount = transmissionCount;
	
	currentTransmission = 0;
	bytesWritten = 0;
	selected = false;
	
	// TODO reset error report
}

bool
test::SpiDevice::isSuccessful()
{
	return (complete && successful);
}

void
test::SpiDevice::reportErrors()
{
	// TODO generate an error report
}
