// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class Mcp2515CanBitTimingsTest : public unittest::TestSuite
{
public:
	void
	testPrecalculatedValues();
};
