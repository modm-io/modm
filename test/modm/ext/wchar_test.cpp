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

void
WcharTest::testSwprintf()
{
	wchar_t buffer[16];
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"%s-%d", narrow, 42), 7);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"modm-42"), 0);

	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"100%%ls"), 6);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"100%ls"), 0);

	// Truncates and reports the error
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 4, L"%d", 12345), -1);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"123"), 0);

	// An exact fit needs space for the terminator
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 5, L"%s", narrow), 4);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, text), 0);
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 4, L"%s", narrow), -1);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"mod"), 0);

	// Does not write to an empty buffer
	buffer[0] = L'x';
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 0, L"%d", 1), -1);
	TEST_ASSERT_EQUALS(buffer[0], L'x');

	// Formats and widths
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"%04x|%-3d|%c", 0xabu, 7, 'z'), 10);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"00ab|7  |z"), 0);
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"%.*d", 3, 7), 3);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"007"), 0);
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"%lu", 123456ul), 6);
	TEST_ASSERT_EQUALS(std::wcscmp(buffer, L"123456"), 0);

	// Wide arguments and non-ASCII formats are not supported
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"%ls", text), -1);
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"%lc", L'x'), -1);
	TEST_ASSERT_EQUALS(std::swprintf(buffer, 16, L"\u00fc"), -1);
	TEST_ASSERT_EQUALS(buffer[0], L'\0');
}
