/*
 * Copyright (c) 2014, Kevin Laeufer
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc_build_info.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

int
main()
{
	// Let's print some information that is provided in the xpcc_build_info.hpp
	XPCC_LOG_INFO << "Machine:  " << XPCC_BUILD_MACHINE  << xpcc::endl;
	XPCC_LOG_INFO << "User:     " << XPCC_BUILD_USER     << xpcc::endl;
	XPCC_LOG_INFO << "Os:       " << XPCC_BUILD_OS       << xpcc::endl;
	XPCC_LOG_INFO << "Compiler: " << XPCC_BUILD_COMPILER << xpcc::endl;

	XPCC_LOG_INFO << "Compare xpcc's printf and xpcc with glibc's printf" << xpcc::endl;

	int32_t ii = -42;

	char format_string[] = ">>%5d<<";

	XPCC_LOG_INFO << "xpcc  ";
	XPCC_LOG_INFO.printf(format_string, ii);
	XPCC_LOG_INFO << xpcc::endl;

	char buf[32] = {' '};
	sprintf(buf, format_string, ii);
	XPCC_LOG_INFO.printf("glibc %s", buf);
	XPCC_LOG_INFO << xpcc::endl;

	float ff_testvector[] = {123.556789, -123.4};

	for (size_t ii = 0; ii < XPCC_ARRAY_SIZE(ff_testvector); ++ii) {
		float ff = ff_testvector[ii];

		for (uint_fast8_t width = 1; width < 10; ++width)
		{
			for (uint_fast8_t width_frac = 1; width_frac < 10; ++width_frac)
			{
				char fmt_str[10];
				sprintf(fmt_str, ">>%%%d.%df<<", width, width_frac);
				XPCC_LOG_INFO << "xpcc  ";
				XPCC_LOG_INFO.printf(fmt_str, ff);
				XPCC_LOG_INFO << xpcc::endl;

				char buf[23];
				sprintf(buf, fmt_str, ff);
				XPCC_LOG_INFO.printf("glibc %s", buf);

				XPCC_LOG_INFO << xpcc::endl << xpcc::endl;
			}
		}
	}

	return 0;
}
