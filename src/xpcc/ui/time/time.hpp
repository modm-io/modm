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
