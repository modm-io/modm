/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
			reporter.reportFailure(lineNumber) << "Double select detected" << modm::endl;
		}
		else if (error & WRONG_LENGTH) {
			error &= ~WRONG_LENGTH;
			reporter.reportFailure(lineNumber)
					<< "Wrong length (" << bytesWritten << " != "
					<< expectedLength << ")" << modm::endl;
		}
		else if (error & WRONG_DATA_RECEIVED) {
			error &= ~WRONG_DATA_RECEIVED;
			modm::IOStream& os = reporter.reportFailure(lineNumber);
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
					<< transmissionCount << ")" << modm::endl;
		}
	}
}
