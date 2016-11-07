/*
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/driver/accessor.hpp>
#include "time.hpp"

// ----------------------------------------------------------------------------
FLASH_STORAGE(uint8_t monthDays[]) = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// ----------------------------------------------------------------------------
/* convert calendar time (seconds since 1970) to broken-time
 * This only works for dates between 01-01-1970 00:00:00 and 
 * 19-01-2038 03:14:07
 * 
 * A leap year is 
 *   ((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0)) 
 * but since we have no fancy years between 1970 and 2038 we can do:
 */
#define LEAP_YEAR(year) 	((year % 4) == 0)
#define SECONDS_PER_DAY		(60 * 60 * 24L)

// ----------------------------------------------------------------------------
void
xpcc::UnixTime::toDate(xpcc::Date* date) const
{
	uint32_t seconds = time;
	
	date->second = seconds % 60;
	seconds /= 60; // now it is minutes
	date->minute = seconds % 60;
	seconds /= 60; // now it is hours
	date->hour = seconds % 24;
	seconds /= 24; // now it is days
	
	// January 1, 1970 was a Thursday.
	date->dayOfTheWeek = (seconds + 4) % 7;
	
	uint16_t year = 1970;
	uint32_t days = 0;
	while ((days += (LEAP_YEAR(year) ? 366 : 365)) <= seconds) {
		year++;
	}
	date->year = year - 1900;
	
	days -= LEAP_YEAR(year) ? 366 : 365;
	seconds -= days; // now it is days in this year, starting at 0
	date->dayOfTheYear = seconds;
	
	uint8_t month;
	uint8_t monthLength;
	for (month = 0; month < 12; month++) {
		if (month == 1) { // Februar
			if (LEAP_YEAR(year)) {
				monthLength = 29;
			}
			else {
				monthLength = 28;
			}
		}
		else {
			monthLength = monthDays[month];
		}
		
		if (seconds >= monthLength) {
			seconds -= monthLength;
		}
		else
		{
			break;
		}
	}
	date->month = month;
	date->day = seconds + 1;
}

// ----------------------------------------------------------------------------
xpcc::UnixTime
xpcc::Date::toUnixTimestamp() const
{
	uint16_t currentYear = this->year + 1900;
	uint32_t seconds;
	
	// Seconds from 1970 till 1 Januar 00:00:00 this year
	seconds = (currentYear - 1970) * (SECONDS_PER_DAY * 365);
	
	// Add extra days for leap years
	for (uint_fast16_t i = 1970; i < currentYear; i++) {
		if (LEAP_YEAR(i)) {
			seconds += SECONDS_PER_DAY;
		}
	}
	
	// Add days for this year
	for (uint_fast8_t i = 0; i < this->month; i++) {
		if (i == 1 && LEAP_YEAR(currentYear)) {
			seconds += SECONDS_PER_DAY * 29;
		}
		else {
			seconds += SECONDS_PER_DAY * monthDays[i];
		}
	}
	
	seconds += (this->day - 1) * SECONDS_PER_DAY;
	seconds +=  this->hour * 60 * 60;
	seconds +=  this->minute * 60;
	seconds +=  this->second;
	
	return seconds;
}
