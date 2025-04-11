/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <chrono>
#include <ctime>
#include <string_view>
#include <cstring>
#include <sys/time.h>
#include <charconv>

namespace modm
{

/// Efficient representation of a date and time
/// @ingroup modm_math_calendar
class DateTime
{
public:
	using duration = std::chrono::milliseconds;
	using precision = typename duration::period;

	constexpr DateTime() = default;

	/// This is an efficient conversion.
	constexpr explicit
	DateTime(uint16_t year, uint8_t month, uint8_t day,
			 uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0,
			 uint16_t millisecond = 0, uint8_t weekday = 0)
	:	data(year - epoch, month, day, hour, minute, second, millisecond), _weekday(weekday)
	{}

	/// This computes the weekday from the date, which is somewhat expensive
	constexpr explicit
	DateTime(const std::chrono::year_month_day& ymd,
			 const std::chrono::hours& hours = std::chrono::hours::zero(),
			 const std::chrono::minutes& minutes = std::chrono::minutes::zero(),
			 const std::chrono::seconds& seconds = std::chrono::seconds::zero(),
			 const std::chrono::milliseconds& subseconds = std::chrono::milliseconds::zero())
	:	DateTime(uint16_t(int(ymd.year())), uint8_t(unsigned(ymd.month())), uint8_t(unsigned(ymd.day())),
				 uint8_t(hours.count()), uint8_t(minutes.count()), uint8_t(seconds.count()),
				 uint16_t(subseconds.count()), std::chrono::weekday{ymd}.c_encoding())
	{}

	constexpr std::chrono::year
	year() const
	{ return std::chrono::year{epoch + data.year}; }

	constexpr std::chrono::month
	month() const
	{ return std::chrono::month{data.month}; }

	constexpr std::chrono::day
	day() const
	{ return std::chrono::day{data.day}; }

	/// This is an efficient conversion.
	constexpr std::chrono::year_month_day
	year_month_day() const
	{ return std::chrono::year_month_day{year(), month(), day()}; }

	constexpr std::chrono::weekday
	weekday() const
	{ return std::chrono::weekday{_weekday}; }

	constexpr std::chrono::days
	day_of_year() const
	{
		uint16_t yday = m2d[data.month] + data.day - 1u;
		if ((data.year & 0b11) == 0b10 and data.month > 2u) yday++;
		return std::chrono::days{yday};
	}


	constexpr std::chrono::hours
	hours() const
	{ return std::chrono::hours{data.hour}; }

	constexpr std::chrono::minutes
	minutes() const
	{ return std::chrono::minutes{data.minute}; }

	constexpr std::chrono::seconds
	seconds() const
	{ return std::chrono::seconds{data.second}; }

	constexpr std::chrono::milliseconds
	subseconds() const
	{ return std::chrono::milliseconds{data.millisecond}; }

	/// @warning This function is *very* inefficient due to an unnecessary conversion from hh:mm:ss.ms to ms
	///          then back to hh:mm:ss.ms in the constructor. This is a limitation of the stdc++ constructor.
	constexpr std::chrono::hh_mm_ss<duration>
	hh_mm_ss() const
	{
		uint32_t ms = ((data.hour * 60ul + data.minute) * 60ul + data.second) * 1000ul + data.millisecond;
		return std::chrono::hh_mm_ss{duration{ms}};
	}

	/// This is an efficient conversion.
	constexpr std::tm
	tm() const
	{
		std::tm tm{};

		tm.tm_sec  = data.second;
		tm.tm_min  = data.minute;
		tm.tm_hour = data.hour;

		tm.tm_mday = data.day; // 1-indexed
		tm.tm_mon  = data.month - 1u; // 0-indexed
		tm.tm_year = data.year + epoch - 1900u;

		tm.tm_wday = weekday().c_encoding(); // 0-indexed

		tm.tm_yday = day_of_year().count(); // 0-indexed

		return tm;
	}

	/// @warning This function is inefficient since it always converts the datetime to seconds.
	constexpr std::time_t
	time_t() const
	{
		return (data.year * seconds_per_year +
				((data.year + 1u) / 4u + day_of_year().count()) * seconds_per_day +
				(data.hour * 60l + data.minute) * 60l + data.second);
	}

	/// @warning This function is inefficient since it always converts the datetime to microseconds.
	constexpr struct timeval
	timeval() const
	{
		return {time_t(), data.millisecond * 1000};
	}

	/// @warning This function is inefficient since it always converts the datetime to milliseconds.
	constexpr duration
	time_since_epoch() const
	{
		return duration{time_t() * 1000 + data.millisecond};
	}


	constexpr auto operator<=>(const DateTime& other) const
	{ return data.value <=> other.data.value; }

	constexpr auto operator==(const DateTime& other) const
	{ return data.value == other.data.value; }

private:
	union Data
	{
		constexpr Data() = default;
		constexpr explicit
		Data(uint8_t year, uint8_t month, uint8_t day,
			 uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond)
		:	millisecond(millisecond), second(second), minute(minute), hour(hour),
			day(day), month(month), year(year) {}
		struct
		{
			uint16_t millisecond;
			uint8_t second;
			uint8_t minute;
			uint8_t hour;

			uint8_t day;
			uint8_t month;
			uint8_t year;
		} modm_packed;
		uint64_t value;
	};

	Data data{};
	uint8_t _weekday{};

	static constexpr uint16_t epoch{1970};
	static constexpr uint32_t seconds_per_day{24*60*60};
	static constexpr uint64_t seconds_per_year{365*seconds_per_day};
	// accumulated (non-leap) days per month, 1-indexed!
	static constexpr uint16_t m2d[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

public:
	/// Efficient conversion
	static constexpr DateTime
	from_tm(const std::tm& tm)
	{
		return DateTime(uint16_t(tm.tm_year + 1900), uint8_t(tm.tm_mon + 1), uint8_t(tm.tm_mday),
						uint8_t(tm.tm_hour), uint8_t(tm.tm_min), uint8_t(tm.tm_sec), 0u, uint8_t(tm.tm_wday));
	}

	/// Really expensive conversion!
	static constexpr DateTime
	from_time_t(std::time_t tt, const std::chrono::milliseconds& subseconds = std::chrono::milliseconds::zero())
	{
		const auto seconds = std::chrono::seconds(tt);
		const auto days = std::chrono::floor<std::chrono::days>(seconds);
		const auto ymd = std::chrono::year_month_day(std::chrono::sys_days(days));
		const auto hms = std::chrono::hh_mm_ss(seconds - days);
		return DateTime(ymd, hms.hours(), hms.minutes(), hms.seconds(), subseconds);
	}

	/// Really expensive conversion!
	static constexpr DateTime
	from_timeval(const struct timeval& tv)
	{
		return from_time_t(std::time_t(tv.tv_sec), std::chrono::milliseconds(tv.tv_usec / 1000ul));
	}

	static consteval DateTime
	fromBuildTime()
	{
		// Example: "Mon Dec 23 17:45:35 2024" or "Wed Jan  1 14:16:06 2025"
		const std::string_view timestamp{__TIMESTAMP__};

		const auto to_uint = [=](uint8_t offset, uint8_t length) -> uint16_t
		{
			auto str = timestamp.substr(offset, length);
			if (str[0] == ' ') str = timestamp.substr(offset+1, length-1);
			int integer;
			(void) std::from_chars(str.begin(), str.end(), integer);
			return uint16_t(integer);
		};
		// All easy to parse integers
		const uint16_t cyear{to_uint(20, 4)};
		const auto cday{uint8_t(to_uint(8, 2))};
		const auto chour{uint8_t(to_uint(11, 2))};
		const auto cminute{uint8_t(to_uint(14, 2))};
		const auto csecond{uint8_t(to_uint(17, 2))};

		// Annoying to parse strings
		const std::string_view months[]
			{"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
		uint8_t cmonth{1u};
		while (months[cmonth] != timestamp.substr(4, 3) and cmonth <= 12u) ++cmonth;

		const std::string_view weekdays[]
			{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
		uint8_t cweekday{};
		while (weekdays[cweekday] != timestamp.substr(0, 3) and cweekday < 7u) ++cweekday;

		return DateTime{cyear, cmonth, cday, chour, cminute, csecond, 0, cweekday};
	}
};

} // namespace modm

#if MODM_HAS_IOSTREAM
#include <inttypes.h>
#include <modm/io/iostream.hpp>

namespace modm
{

/// @ingroup modm_math_calendar
inline modm::IOStream&
operator << (modm::IOStream& s, const DateTime& dt)
{
	// ISO encoding: 2024-12-22 18:39:21.342
	s.printf("%04" PRIu16 "-%02" PRIu8 "-%02" PRIu8 " %02" PRIu8 ":%02" PRIu8 ":%02" PRIu8 ".%03" PRIu16,
			 uint16_t(int(dt.year())), uint8_t(unsigned(dt.month())), uint8_t(unsigned(dt.day())),
			 uint8_t(dt.hours().count()), uint8_t(dt.minutes().count()), uint8_t(dt.seconds().count()),
			 uint16_t(dt.subseconds().count()));
	return s;
}

} // modm namespace

#endif
