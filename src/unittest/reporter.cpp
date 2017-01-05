/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "reporter.hpp"

namespace
{
	FLASH_STORAGE_STRING(invaildName) = "invalid";
	
	FLASH_STORAGE_STRING(failHeader) = "FAIL: ";
	FLASH_STORAGE_STRING(failColon) = " : ";
	
	FLASH_STORAGE_STRING(reportPassed) = "\nPassed ";
	FLASH_STORAGE_STRING(reportFailed) = "\nFailed ";
	FLASH_STORAGE_STRING(reportOf) = " of ";
	FLASH_STORAGE_STRING(reportTests) = " tests\n";
	FLASH_STORAGE_STRING(reportOk) = "OK!\n";
	FLASH_STORAGE_STRING(reportFail) = "FAIL!\n";
}

unittest::Reporter::Reporter(modm::IODevice& device) :
	outputStream(device), testName(modm::accessor::asFlash(invaildName)),
	testsPassed(0), testsFailed(0)
{
}

void
unittest::Reporter::nextTestSuite(modm::accessor::Flash<char> name)
{
	testName = name;
}

void
unittest::Reporter::reportPass()
{
	testsPassed++;
}

modm::IOStream&
unittest::Reporter::reportFailure(unsigned int lineNumber)
{
	testsFailed++;
	outputStream << modm::accessor::asFlash(failHeader)
				 << testName
				 << ':'
				 << lineNumber
				 << modm::accessor::asFlash(failColon);
	return outputStream;
}

uint8_t
unittest::Reporter::printSummary()
{
	if (testsFailed == 0) {
		outputStream << modm::accessor::asFlash(reportPassed)
					 << testsPassed
					 << modm::accessor::asFlash(reportTests)
					 << modm::accessor::asFlash(reportOk)
					 << modm::endl;
		return 0;
	}
	else {
		outputStream << modm::accessor::asFlash(reportFailed)
					 << testsFailed
					 << modm::accessor::asFlash(reportOf)
					 << (testsFailed + testsPassed)
					 << modm::accessor::asFlash(reportTests)
					 << modm::accessor::asFlash(reportFail)
					 << modm::endl;
		return 1;
	}
}
