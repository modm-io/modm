// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_COROUTINE_HPP
#define XPCC_COROUTINE_HPP

#include "macros.hpp"
#include <xpcc/architecture/utils.hpp>
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

/// @cond
/// Used to store a coroutines's position (what Dunkels calls a
/// "local continuation").
typedef uint8_t CoState;
/// We use 0 instead of -1 since it might be fast to check
static constexpr CoState CoStopped = CoState(0);
/// @endcond

/**
 * Coroutine base class.
 *
 * This is the base class which must be inherited from for using
 * coroutines in your class.
 * You need to declare the number of coroutines in your class!
 *
 * You must begin each coroutine using `CO_BEGIN(index)` where `index` is
 * the unique index of your coroutine starting a zero.
 * You may exit and return a value by using `CO_RETURN(value)` or
 * return the result of another coroutine using `CO_RETURN_CALL(coroutine())`.
 * This return value is wrapped in a `xpcc::co::Result<Type>` struct
 * and transparently returned by the `CO_CALL` macro so it can be used
 * to influence your program flow.
 * If the coroutine reaches `CO_END()` it will exit automatically,
 * with the result of `0` cast to the return type.
 * Should you wish to return a value at the end, you may use
 * `CO_END_RETURN(value)`.
 * You may also return the result of another coroutine using
 * `CO_END_RETURN_CALL(coroutine())`.
 *
 * Be aware that this class keeps a seperate state for each of your coroutines.
 * This allows each coroutine to be run at the same time.
 * This might require the use of an internal Semaphore or Mutex if such
 * dependencies exist in your use case.
 * Take a look at the `NestedCoroutine` class for mutually exclusive coroutines,
 * which also require a little less memory.
 *
 * @warning	**Coroutines are not thread-safe!** If two threads access the
 * 			same coroutine, you must use a Mutex to regulate access to it.
 *
 * @see NestedCoroutine
 *
 * @ingroup	coroutine
 * @author	Niklas Hauser
 * @tparam	Methods	number of coroutines: the number of coroutines that are
 * 					available in this class. Must be non-zero!
 */
template< uint8_t Methods = 1>
class Coroutine
{
	static_assert(Methods > 0, "The number of coroutine methods must be at least 1!");

protected:
	Coroutine()
	{
		stopAllCoroutines();
	}

public:
	/// Force all coroutines to stop running
	inline void
	stopAllCoroutines()
	{
		for (CoState &level : coStateArray)
		{
			level = CoStopped;
		}
	}

	/// @return	`true` if a coroutine is running, else `false`
	bool inline
	isCoroutineRunning() const
	{
		for (CoState &level : coStateArray)
		{
			if (level != CoStopped)
				return true;
		}
		return false;
	}

#ifdef __DOXYGEN__
	/// @cond
	/**
	 * Run the coroutine.
	 *
	 * You need to implement this method in you subclass yourself.
	 *
	 * @return	>`NestingError` if still running, <=`NestingError` if it has finished.
	 */
	xpcc::co::Result< ReturnType >
	coroutine(...);
	/// @endcond
#endif

protected:
	/// @cond

	/// increases nesting level, call this in the switch statement!
	/// @return current state before increasing nesting level
	CoState ALWAYS_INLINE
	pushCo(uint_fast8_t index)
	{
		return coStateArray[index];
	}

	/// always call this before returning from the run function!
	/// decreases nesting level
	void ALWAYS_INLINE
	popCo()
	{}

	// invalidates the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popCo()!)
	void ALWAYS_INLINE
	stopCo(uint_fast8_t index)
	{
		coStateArray[index] = CoStopped;
	}

	/// sets the state of the parent nesting level
	/// @warning	be aware in which nesting level you call this! (before popCo()!)
	void ALWAYS_INLINE
	setCo(CoState state, uint_fast8_t index)
	{
		coStateArray[index] = state;
	}

	/// @return	`true` if `stopCo()` has been called before
	bool ALWAYS_INLINE
	isStoppedCo(uint_fast8_t index) const
	{
		return (coStateArray[index] == CoStopped);
	}

private:
	CoState coStateArray[Methods];
};

} // namespace co

} // namespace xpcc

#endif // XPCC_COROUTINE_HPP
