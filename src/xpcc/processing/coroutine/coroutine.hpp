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
#include <xpcc/utils/arithmetic_traits.hpp>

#ifdef __DOXYGEN__

/**
 * Nested Coroutines protect against memory corruption by checking if the nesting level
 * is within the allocated nesting level depth.
 * If the allocated nesting level is exceeded, the coroutine does not execute, but returns
 * the `xpcc::co::NestingError` state value.
 * However, the `PT_CALL()` or `CO_CALL()` macros are not constructed to handle this error and
 * will interpret this error as a normal coroutine stop and therefore immediately continue program
 * execution.
 *
 * Since the required nesting depth of a nested coroutine can be known before runtime
 * by looking at the source code and counting the nesting levels, this check acts only as a
 * protection against executing random code, when entering too many levels.
 * So instead of the entire program crashing, the coroutine simply does not execute at all,
 * which can make it easier to track down the problem too few allocated nesting levels.
 *
 * You as a programmer are responible for allocating the correct amount of nesting levels!
 * However, if you are sure that you have counted the levels correctly and your code is
 * not breaking, you may remove this check from all coroutines in your `project.cfg`:
 *
 * [defines]
 * XPCC_COROUTINE_CHECK_NESTING_DEPTH = false
 *
 * @see	NestedCoroutine
 * @ingroup	coroutine
 */
#define XPCC_COROUTINE_CHECK_NESTING_DEPTH	true

#else
// by default we check all nesting level depths
#	ifndef	XPCC_COROUTINE_CHECK_NESTING_DEPTH
#		define XPCC_COROUTINE_CHECK_NESTING_DEPTH	true
#	endif
#endif

namespace xpcc
{

namespace co
{

/// @cond
/// State of a coroutine.
enum State
{
	// reasons to stop
	Stop = 0,			///< Coroutine finished
	NestingError = 1,	///< Nested Coroutine has run out of nesting levels

	// reasons to wait
	WrongContext = 2,	///< Coroutine already running in a different context
	WrongState = 3,		///< Another coroutine of the same class is already running in this context

	// reasons to keep running
	Running = 4,		///< Coroutine is running
};
/// @endcond

/// All Coroutines return an encapsulated result type.
template < typename T >
struct Result
{
	/// Return only the `state`. The `result` will be initialized by it's default constructor.
	Result(uint8_t state) : state(state) {}
	/// Return `state` and valid `result`.
	Result(uint8_t state, T result) : state(state), result(result) {}
	/// The `co::State`.
	uint8_t state;
	/// Custom return value.
	T result;
};
/// @cond
/// void is not an object type
template <>
struct Result<void>
{
	/// Return only the `state`. There is no result.
	Result(uint8_t state) : state(state) {}
	/// Constructor with dummy result needed by the `CO_CALL_BLOCKING` macro.
	Result(uint8_t state, uint8_t /*dummy_result*/) : state(state) {}
	// This anonymous union reduces the size to only 1 byte, instead of two
	// `CO_CALL_BLOCKING` will still return something, but it will be either 0 or 1 (`Stop` or `NestingError`).
	union
	{
		/// The `co::State`.
		uint8_t state;
		/// Dummy result needed by the `CO_CALL_BLOCKING` macro.
		uint8_t result;
	};
};
/// @endcond

/**
 * This is the base class which must be inherited from for using
 * coroutines in your class.
 *
 * You are responsible to choosing the right nesting depth!
 * This class will guard itself against calling another coroutine at too
 * deep a nesting level and inform you gently of this by returning
 * `xpcc::co::NestingError` from your called `coroutine(ctx)`.
 * It is then up to you to recognise this in your program design
 * and increase the nesting depth or rethink your code.
 *
 * Be aware that only one coroutine of the same object can run at the
 * same time. Even if you call another coroutine, it will simply not
 * run until the conflicting coroutine finished.
 *
 * Each coroutine requires a context pointer, which is used to ensure the
 * coroutine only executes in the context that it was originally started from.
 * If you call a coroutine in a different context, it will return
 * `xpcc::co::WrongContext`.
 * Similarly, if you call a different coroutine of the same class in
 * the same context, while another coroutine is running, it will return
 * `xpcc::co::WrongState`.
 * Using the `CO_CALL(coroutine(ctx))` macro, you can wait for these
 * coroutines to become available and then run them, so you usually do
 * not need to worry about those cases.
 *
 * You may exit and return a value by using `CO_RETURN(value)` or
 * return the result of another coroutine using `CO_RETURN_CALL(coroutine(ctx))`.
 * This return value is wrapped in a `xpcc::co::Result<Type>` struct
 * and transparently returned by the `CO_CALL` macro so it can be used
 * to influence your program flow.
 * If the coroutine reaches `CO_END()` it will exit automatically,
 * with the result of `0` cast to the return type.
 * Should you wish to return a value at the end, you may use
 * `CO_END_RETURN(value)`.
 * You may also return the result of another coroutine using
 * `CO_END_RETURN_CALL(coroutine(ctx))`.
 *
 * @ingroup	coroutine
 * @author	Niklas Hauser
 * @tparam	Depth	maximum nesting depth: the maximum number of
 * 					coroutines that are called within coroutines (should be <128).
 */
template< uint8_t Depth >
class NestedCoroutine
{
	/// Used to store a coroutines's position (what Dunkels calls a
	/// "local continuation").
	typedef uint8_t CoState;
protected:
	/// Construct a new class with nested coroutines
	NestedCoroutine()
	:	coLevel(0), coContext(0)
	{
		for (CoState &level : coStateArray)
		{
			level = CoStopped;
		}
	}

public:
	/// Force all coroutines to stop running at the current nesting level
	inline void
	stopCoroutine()
	{
		uint_fast8_t level = coLevel;
		while (level < Depth + 1)
		{
			coStateArray[level++] = CoStopped;
		}
		if (coLevel == 0)
			coContext = 0;
	}

	/// @return	`true` if a coroutine is running at the current nesting level, else `false`
	bool inline
	isCoroutineRunning() const
	{
		return !isStoppedCo();
	}

	/// @return the nesting depth in the current coroutine, or -1 if called outside any coroutine
	int8_t inline
	getCoroutineDepth() const
	{
		return static_cast<int8_t>(coLevel) - 1;
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
	coroutine(void *ctx, ...);
	/// @endcond
#endif

protected:
	/// @cond

	/// increases nesting level, call this in the switch statement!
	/// @return current state before increasing nesting level
	CoState inline
	pushCo()
	{
		return coStateArray[coLevel++];
	}

	/// always call this before returning from the run function!
	/// decreases nesting level
	void inline
	popCo()
	{
		coLevel--;
	}

	// invalidates the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popCo()!)
	void inline
	stopCo()
	{
		coStateArray[coLevel-1] = CoStopped;
		if (coLevel == 1)
			coContext = 0;
	}

	/// sets the state of the parent nesting level
	/// @warning	be aware in which nesting level you call this! (before popCo()!)
	void inline
	setCo(CoState state)
	{
		coStateArray[coLevel-1] = state;
	}

	/// @return `true` if the nesting depth allows for another level.
	/// @warning	be aware in which nesting level you call this! (before pushCo()!)
	bool inline
	nestingOkCo() const
	{
#if XPCC_COROUTINE_CHECK_NESTING_DEPTH
		return (coLevel < Depth + 1);
#else
		return true;
#endif
	}

	/// @return	`true` if `stopCo()` has been called before
	bool inline
	isStoppedCo() const
	{
		return (coStateArray[coLevel] == CoStopped);
	}

	/// @return `true` if the coroutine is called in the same context, or the context is not set
	///			`false` if the coroutine is claimed by another context
	bool inline
	beginCo(void *ctx)
	{
		// only one comparison, if this coroutine is called in the same context
		if (ctx == coContext)
			return true;

		// two comparisons + assignment, if this coroutine is called for the first time
		if (coContext == 0)
		{
			coContext = ctx;
			return true;
		}

		return false;
	}

public:
	static constexpr CoState CoStopped = static_cast<CoState>(0);
	/// @endcond
private:
	uint8_t coLevel;
	CoState coStateArray[Depth+1];
	void *coContext;
};

// ----------------------------------------------------------------------------
// we won't document the specialisation again
/// @cond
template <>
class NestedCoroutine<0>
{
	typedef uint8_t CoState;
protected:
	NestedCoroutine() :
		coState(CoStopped), coLevel(-1), coContext(0)
	{
	}

public:
	void inline
	stopCoroutine()
	{
		this->stopCo();
	}

	bool inline
	isCoroutineRunning() const
	{
		return !isStoppedCo();
	}

	int8_t inline
	getCoroutineDepth() const
	{
		return coLevel;
	}

protected:
	CoState inline
	pushCo()
	{
		coLevel = 0;
		return coState;
	}

	void inline
	popCo()
	{
		coLevel = -1;
	}

	void inline
	stopCo()
	{
		coState = CoStopped;
		coContext = 0;
	}

	bool inline
	nestingOkCo() const
	{
#if XPCC_COROUTINE_CHECK_NESTING_DEPTH
		return (coLevel != 0);
#else
		return true;
#endif
	}

	void inline
	setCo(CoState state)
	{
		coState = state;
	}

	bool inline
	isStoppedCo() const
	{
		return (coState == CoStopped);
	}

	bool inline
	beginCo(void *ctx)
	{
		if (ctx == coContext)
			return true;

		if (coContext == 0)
		{
			coContext = ctx;
			return true;
		}

		return false;
	}

public:
	static constexpr CoState CoStopped = static_cast<CoState>(0);
private:
	CoState coState;
	int8_t coLevel;
	void *coContext;
};
/// @endcond

typedef NestedCoroutine<0> Coroutine;

} // namespace co

} // namespace xpcc

#endif // XPCC_COROUTINE_HPP
