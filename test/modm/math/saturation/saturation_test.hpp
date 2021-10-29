/*
 * Copyright (c) 2021, Thomas Sommer
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>
#include <modm/debug/logger.hpp>

#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

/// @ingroup modm_test_test_math
class SaturatedTest : public unittest::TestSuite
{
public:
	template<typename A = uint8_t, typename B = A, typename C = B>
	void
	testGeneric();

	void
	test_uint8_t__int16_t() {
		MODM_LOG_INFO << __FUNCTION__ << modm::endl;
		testGeneric<uint8_t, int16_t>();
	}

	test_reference_all_uint8_t() {
		uint8_t u8;
		Saturated<&uint8_t> a(u8);
	}

/*	void
	test_all_uint8_t()
	{
		MODM_LOG_INFO << __FUNCTION__ << modm::endl;
		testGeneric<uint8_t>();
	}

 	void
	test_all_int8_t()
	{
		MODM_LOG_INFO << __FUNCTION__ << modm::endl;
		testGeneric<int8_t>();
	}

	void
	test_all_uint16_t()
	{
		MODM_LOG_INFO << __FUNCTION__ << modm::endl;
		testGeneric<int16_t>();
	}

	void
	test_all_int16_t()
	{
		MODM_LOG_INFO << __FUNCTION__ << modm::endl;
		testGeneric<int16_t>();
	} */
};

#include "saturation_test_impl.hpp"