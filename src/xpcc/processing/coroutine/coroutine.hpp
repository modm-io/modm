// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_COROUTINE_COMMON_HPP
#define XPCC_COROUTINE_COMMON_HPP

#include <stdint.h>

namespace xpcc
{

namespace co
{

/// @cond
/// State of a coroutine.
enum
State
{
	// reasons to stop
	Stop = 0,			///< Coroutine finished
	NestingError = 1,	///< Nested Coroutine has run out of nesting levels

	// reasons to wait
	WrongState = 254,		///< Another coroutine of the same class is already running in this context

	// reasons to keep running
	Running = 255,		///< Coroutine is running
};
/// @endcond

/// All Coroutines return an encapsulated result type.
/// @warning The result type **must** have a default constructor!
template < typename T >
struct Result
{
	/// Return only the `state`. The `result` will be initialized by it's default constructor.
	Result(uint_fast8_t state) : state(state) {}
	/// Return `state` and valid `result`.
	Result(uint_fast8_t state, T result) : state(state), result(result) {}
	/// The `co::State`.
	uint_fast8_t state;
	/// Custom return value.
	T result;
};
/// @cond
/// void is not an object type
template <>
struct Result<void>
{
	/// Return only the `state`. There is no result.
	Result(uint_fast8_t state) : state(state) {}
	/// Constructor with dummy result needed by the `CO_CALL_BLOCKING` macro.
	Result(uint_fast8_t state, uint_fast8_t /*dummy_result*/) : state(state) {}
	// This anonymous union reduces the size to only 1 byte, instead of two
	// `CO_CALL_BLOCKING` will still return something, but it will be either 0 or 1 (`Stop` or `NestingError`).
	union
	{
		/// The `co::State`.
		uint_fast8_t state;
		/// Dummy result needed by the `CO_CALL_BLOCKING` macro.
		uint_fast8_t result;
	};
};
/// @endcond

} // namespace co

} // namespace xpcc

#endif // XPCC_COROUTINE_COMMON_HPP
