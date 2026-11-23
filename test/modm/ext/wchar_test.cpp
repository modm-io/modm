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

#include "wchar_test.hpp"
#include <cstdlib>
#include <cwchar>
#include <string>

static_assert(sizeof(wchar_t) == 4);

namespace
{
const wchar_t *volatile text{L"modm"};
const char *volatile narrow{"modm"};
}

void
WcharTest::testNewlibFunctions()
{
	TEST_ASSERT_EQUALS(std::wcslen(text), 4u);
	TEST_ASSERT_EQUALS(std::wcscmp(text, L"modm"), 0);
	TEST_ASSERT_TRUE(std::wcschr(text, L'd') == text + 2);

	wchar_t converted[8]{};
	TEST_ASSERT_EQUALS(std::mbstowcs(converted, narrow, 8), 4u);
	TEST_ASSERT_EQUALS(std::wcscmp(converted, text), 0);
}

void
WcharTest::testWideString()
{
	const std::wstring string{text};
	TEST_ASSERT_EQUALS(string.size(), 4u);
	TEST_ASSERT_TRUE(string == L"modm");
}
