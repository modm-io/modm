/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__TIME_HPP
#define XPCC__TIME_HPP

#include <stdint.h>

namespace xpcc
{
	// forward declaration
	class Date;

	/**
	 * Number of Seconds since 00:00, Jan 1 1970 UTC
	 *
	 * @author	Fabian Greif
	 * @ingroup	ui
	 */
	class UnixTime
	{
	public:
		UnixTime(uint32_t t) :
			time(t)
		{
		}

		operator uint32_t () const
		{
			return time;
		}

		/**
		 * Converts given time since epoch as xpcc::UnixTime value into
		 * calendar time, expressed in Coordinated Universal Time (UTC).
		 */
		void
		toDate(Date* date) const;

	private:
		uint32_t time;
	};

	/**
	 * @brief	Calender Date and Time
	 *
	 * @author	Fabian Greif
	 * @ingroup	ui
	 */
	class Date
	{
	public:
		/**
		 * Converts calendar time to a time since epoch as a
		 * xpcc::UnixTime object.
		 */
		UnixTime
		toUnixTimestamp() const;

	public:
		uint8_t second;			///< Seconds after the minute [0, 60]
		uint8_t minute;			///< Minutes after the hour [0, 59]
		uint8_t hour;			///< Hours since midnight [0, 23]
		uint8_t day;			///< Day of the month [1, 31]
		uint8_t month;			///< Months since January [0, 11]
		uint8_t year;			///< Years since 1900 (up to 2099)
		uint8_t dayOfTheWeek;	///< Days since Sunday [0, 6] (0=Sunday, 6=Saturday)
		uint8_t dayOfTheYear;	///< Days since January 1 [0, 365]
	};
}

#endif	// XPCC__TIME_HPP
