// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_DS1302_HPP
#define XPCC_DS1302_HPP

namespace xpcc
{
struct ds1302
{
	struct Config
	{
		using Ce   = void; // required
		using Sclk = void; // required
		using Io   = void; // required
	};

	struct xpcc_packed
	Data
	{
		inline uint8_t
		getSeconds() {
			return doubleBcdToDecimal(data[0]);
		}

		inline uint8_t
		getMinutes() {
			return doubleBcdToDecimal(data[1]);
		}

		inline bool
		is24hours() {
			return not (data[2] & (1 << 7));
		}

		inline bool
		isPm() {
			return data[2] & (1 << 5);
		}

		inline bool
		isAm() {
			return not isPm();
		}

		inline uint8_t
		getHours() {
			if (is24hours()) {
				return doubleBcdToDecimal(data[2]);
			} else {
				return doubleBcdToDecimal(data[2] & 0x1f);
			}
		}

		inline uint8_t
		getDate() {
			return doubleBcdToDecimal(data[3]);
		}

		inline uint8_t
		getMonth() {
			return doubleBcdToDecimal(data[4]);
		}

		inline uint8_t
		getDayOfWeek() {
			return data[5];
		}

		inline uint8_t
		getYear() {
			return data[6];
		}

		inline bool
		isWriteProtected() {
			return data[7] & (1 << 7);
		}

		/// Local copy of the date data of the RTC.
		/// 31 bytes of RAM and trickle-charge register
		/// are ignored.
		uint8_t data[8];

protected:
		inline uint8_t
		doubleBcdToDecimal(uint8_t val)
		{
			uint8_t tens = ((val & 0x70) >> 4) * 10;
			uint8_t ones = ( val & 0x0f);
			return (tens + ones);
		}
	};
};

/**
 * Realtime clock DS1302
 *
 * @ingroup  ds1302
 * @author   Sascha Schade (strongly-typed)
 */
template < class PinSet >
class Ds1302
{
	using Ce   = typename PinSet::Ce;
	using Sclk = typename PinSet::Sclk;
	using Io   = typename PinSet::Io;

public:
	static void
	initialize();

public:
	// protected:
	static void
	writeByte(uint8_t byte);

	static void
	write(const uint8_t addr, const uint8_t data);

	static uint8_t
	read(const uint8_t addr);

public:
		/// Set or reset the write protect. Write protection must be disabled before any write to the device.
		static void
		writeProtect(const bool wp) {
				write(0x8e, wp ? 0x80 : 0x00);
		}

		/// Enable the oscillator of RTC. Write protection must be disabled before.
		static void
		enableOscillator() {
				write(0x80, 0x00);
		}

		static void
		readRtc(ds1302::Data &data);

protected:
	/// Required CE to CLK Setup Time can be as long as 4 usec and 2 Volts.
	static constexpr uint8_t delay_ce = 4; // microseconds

	/// Required CLK Low/High Time can be as long as 1 usec and 2 Volts.
	static constexpr uint8_t delay_clk = 1; // microsecond
};

}

#include "ds1302_impl.hpp"

#endif // XPCC_DS1302_HPP
