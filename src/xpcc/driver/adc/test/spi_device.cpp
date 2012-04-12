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

#include <unittest/reporter.hpp>
#include <unittest/controller.hpp>
#include <unittest/harness.hpp>

#include "spi_device.hpp"

// ----------------------------------------------------------------------------
test::SpiDevice::SpiDevice() :
	transmissions(0),
	transmissionCount(0),
	currentTransmission(0),
	bytesWritten(0),
	selected(false),
	complete(true),
	error(NO_ERROR),
	reportErrors(true),
	rxBuffer(0)
{
}

test::SpiDevice::~SpiDevice()
{
	delete rxBuffer;
}

// ----------------------------------------------------------------------------
void
test::SpiDevice::select()
{
	if (!selected) {
		selected = true;
		bytesWritten = 0;
	}
	else {
		error |= DOUBLE_SELECT;
	}
}

void
test::SpiDevice::deselect()
{
	std::size_t expectedLength = 0;
	
	if (selected)
	{
		if (!complete)
		{
			expectedLength = transmissions[currentTransmission].length;
			if (bytesWritten != expectedLength) {
				error |= WRONG_LENGTH;
			}
			
			if (currentTransmission + 1 == transmissionCount) {
				complete = true;
			}
		}
		else {
			error |= MAX_TRANSMISSION_COUNT_EXCEEDED;
		}
	}
	
	selected = false;
	
	if (reportErrors)
	{
		unittest::Reporter& reporter = unittest::Controller::instance().getReporter(); 
		if (error == NO_ERROR) {
			reporter.reportPass();
		}
		else if (error & DOUBLE_SELECT) {
			error &= ~DOUBLE_SELECT;
			reporter.reportFailure(lineNumber) << "Double select detected" << xpcc::endl;
		}
		else if (error & WRONG_LENGTH) {
			error &= ~WRONG_LENGTH;
			reporter.reportFailure(lineNumber)
					<< "Wrong length (" << bytesWritten << " != "
					<< expectedLength << ")" << xpcc::endl;
		}
		else if (error & WRONG_DATA_RECEIVED) {
			error &= ~WRONG_DATA_RECEIVED;
			xpcc::IOStream& os = reporter.reportFailure(lineNumber);
			os << "Error in Transmission " << currentTransmission << "\n" " expected = ";
			std::size_t length = transmissions[currentTransmission].length;
			unittest::printArray(os, 0, length, transmissions[currentTransmission].rx);
			os << " received = ";
			unittest::printArray(os, 0, length, rxBuffer);
			os << " tx       = ";
			unittest::printArray(os, 0, length, transmissions[currentTransmission].tx);
		}
	}
	
	currentTransmission++;
}

uint8_t
test::SpiDevice::write(uint8_t data)
{
	uint8_t out = 0xff;
	
	if (selected && !complete)
	{
		const Transmission* t = &transmissions[currentTransmission];
		
		if (bytesWritten < t->length) {
			if (data != t->rx[bytesWritten]) {
				error |= WRONG_DATA_RECEIVED;
			}
			rxBuffer[bytesWritten] = data;
			out = t->tx[bytesWritten];
		}
		else {
			error |= WRONG_LENGTH;
		}
		bytesWritten++;
	}
	
	return out;
}

// ----------------------------------------------------------------------------
void
test::SpiDevice::start(const Transmission* transmissions,
		std::size_t transmissionCount, std::size_t lineNumber, bool reportErrors)
{
	complete = (transmissionCount == 0);
	error = NO_ERROR;
	
	this->transmissions = transmissions;
	this->transmissionCount = transmissionCount;
	this->lineNumber = lineNumber;
	this->reportErrors = reportErrors;
	
	currentTransmission = 0;
	bytesWritten = 0;
	selected = false;
	
	std::size_t maxSize = 0;
	for (std::size_t i = 0; i < transmissionCount; ++i) {
		if (transmissions[i].length > maxSize) {
			maxSize = transmissions[i].length;
		}
	}
	
	if (rxBuffer != 0) {
		delete rxBuffer;
	}
	
	rxBuffer = new uint8_t[maxSize];
}

// ----------------------------------------------------------------------------
void
test::SpiDevice::finish()
{
	unittest::Reporter& reporter = unittest::Controller::instance().getReporter();
	
	if (reportErrors)
	{
		if (selected) {
			deselect();
			
			reporter.reportFailure(lineNumber) << "Device still selected\n";
		}
		
		if ((error & MAX_TRANSMISSION_COUNT_EXCEEDED) || !complete) {
			error &= ~MAX_TRANSMISSION_COUNT_EXCEEDED;
			reporter.reportFailure(lineNumber)
					<< "Transmission Count (" << currentTransmission << " == "
					<< transmissionCount << ")" << xpcc::endl;
		}
	}
}
