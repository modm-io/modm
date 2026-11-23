/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_utils
class WcharTest : public unittest::TestSuite
{
public:
	/// The application must use the wchar_t width of the precompiled libraries
	void
	testNewlibFunctions();

	void
	testWideString();

	void
	testSwprintf();
};
