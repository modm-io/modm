/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <bits/functexcept.h>

//#include <modm/architecture/interface/assert.hpp>

namespace
{
	void
	__modm_assert()
	{
		// FIXME: this increases flash usage by ~200 bytes
		//modm_assert(false, "stdc++", "", "abort");
	}
}

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

	void
	__throw_bad_exception()
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_alloc()
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_cast()
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_typeid()
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_logic_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_domain_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_invalid_argument(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_length_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_out_of_range(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_runtime_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_range_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_overflow_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_underflow_error(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_optional_access()
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_variant_access(const char* __s __attribute__((unused)))
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_function_call()
	{ __modm_assert(); __builtin_abort(); }

	void
	__throw_bad_any_cast()
	{ __modm_assert(); __builtin_abort(); }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace
