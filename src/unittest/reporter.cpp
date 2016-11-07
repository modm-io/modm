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

unittest::Reporter::Reporter(xpcc::IODevice& device) :
	outputStream(device), testName(xpcc::accessor::asFlash(invaildName)),
	testsPassed(0), testsFailed(0)
{
}

void
unittest::Reporter::nextTestSuite(xpcc::accessor::Flash<char> name)
{
	testName = name;
}

void
unittest::Reporter::reportPass()
{
	testsPassed++;
}

xpcc::IOStream&
unittest::Reporter::reportFailure(unsigned int lineNumber)
{
	testsFailed++;
	outputStream << xpcc::accessor::asFlash(failHeader)
				 << testName
				 << ':'
				 << lineNumber
				 << xpcc::accessor::asFlash(failColon);
	return outputStream;
}

uint8_t
unittest::Reporter::printSummary()
{
	if (testsFailed == 0) {
		outputStream << xpcc::accessor::asFlash(reportPassed)
					 << testsPassed
					 << xpcc::accessor::asFlash(reportTests)
					 << xpcc::accessor::asFlash(reportOk)
					 << xpcc::endl;
		return 0;
	}
	else {
		outputStream << xpcc::accessor::asFlash(reportFailed)
					 << testsFailed
					 << xpcc::accessor::asFlash(reportOf)
					 << (testsFailed + testsPassed)
					 << xpcc::accessor::asFlash(reportTests)
					 << xpcc::accessor::asFlash(reportFail)
					 << xpcc::endl;
		return 1;
	}
}
