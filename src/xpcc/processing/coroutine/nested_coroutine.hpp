// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_NESTED_RESUMABLE_HPP
#define XPCC_NESTED_RESUMABLE_HPP

#include "macros.hpp"
#include <xpcc/architecture/utils.hpp>
#include <stdint.h>

#ifdef __DOXYGEN__

/**
 * Nested Resumables protect against memory corruption by checking if the nesting level
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
@verbatim
[defines]
XPCC_RESUMABLE_CHECK_NESTING_DEPTH = false
@endverbatim
 *
 * @see	NestedResumable
 * @ingroup	coroutine
 */
#define XPCC_RESUMABLE_CHECK_NESTING_DEPTH	true

#else
// by default we check all nesting level depths
#	ifndef	XPCC_RESUMABLE_CHECK_NESTING_DEPTH
#		define XPCC_RESUMABLE_CHECK_NESTING_DEPTH	true
#	endif
#endif

namespace xpcc
{

namespace co
{

/**
 * This is the base class which must be inherited from for using
 * nested coroutines in your class.
 *
 * You are responsible to choosing the right nesting depth!
 * This class will guard itself against calling another coroutine at too
 * deep a nesting level and inform you gently of this by returning
 * `xpcc::co::NestingError` from your called `coroutine()`.
 * It is then up to you to recognise this in your program design
 * and increase the nesting depth or rethink your code.
 * You may disable the check by setting `XPCC_RESUMABLE_CHECK_NESTING_DEPTH`
 * to `false` in your project configuration.
 *
 * The coroutines of this class are mutually exclusive, so only one
 * coroutine of the same object can run at the same time. Even if you
 * call another coroutine, it will simply return `xpcc::co::WrongState`.
 * Using the `CO_CALL(coroutine())` macro, you can wait for these
 * coroutines to become available and then run them, so you usually do
 * not need to worry about those cases.
 *
 * You must begin each coroutine using `CO_BEGIN()`.
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
 * @warning	**Resumables are not thread-safe!** If two threads access the
 * 			same coroutine, you must use a Mutex to regulate access to it.
 *
 * @ingroup	coroutine
 * @author	Niklas Hauser
 * @tparam	Levels	maximum number of nesting levels (should be <128):
 * 					max(number of coroutines that are called within coroutines) + 1
 */
template< uint8_t Levels = 1>
class NestedResumable
{
	static_assert(Levels > 0, "The number of coroutine nesting levels must be at least 1!");

protected:
	/// Construct a new class with nested coroutines
	NestedResumable()
	:	coLevel(0)
	{
		for (CoState &level : coStateArray)
		{
			level = CoStopped;
		}
	}

public:
	/// Force all coroutines to stop running at the current nesting level
	inline void
	stopResumable()
	{
		uint_fast8_t level = coLevel;
		while (level < Levels)
		{
			coStateArray[level++] = CoStopped;
		}
	}

	/// @return	`true` if a coroutine is running at the current nesting level, else `false`
	bool inline
	isResumableRunning() const
	{
		return !isStoppedCo();
	}

	/// @return the nesting depth in the current coroutine, or -1 if called outside any coroutine
	int8_t inline
	getResumableDepth() const
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
	coroutine(...);
	/// @endcond
#endif

protected:
	/// @cond

	/// increases nesting level, call this in the switch statement!
	/// @return current state before increasing nesting level
	CoState inline
	pushCo(uint8_t /*index*/)
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
	stopCo(uint8_t /*index*/)
	{
		coStateArray[coLevel-1] = CoStopped;
	}

	/// sets the state of the parent nesting level
	/// @warning	be aware in which nesting level you call this! (before popCo()!)
	void inline
	setCo(CoState state, uint8_t /*index*/)
	{
		coStateArray[coLevel-1] = state;
	}

	/// @return `true` if the nesting depth allows for another level.
	/// @warning	be aware in which nesting level you call this! (before pushCo()!)
	bool inline
	nestingOkCo() const
	{
#if XPCC_RESUMABLE_CHECK_NESTING_DEPTH
		return (coLevel < Levels);
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

	/// compatibility with Resumable class
	template<uint8_t index>
	static void
	checkCoMethods()
	{}

	/// asserts that this method is called in this parent class
	template<bool isNested>
	static void
	checkCoType()
	{
		static_assert(isNested == true, "You cannot declare an index for a _nested_ coroutine!");
	}
	/// @endcond
private:
	uint_fast8_t coLevel;
	CoState coStateArray[Levels];
};

// ----------------------------------------------------------------------------
// we won't document the specialisation again
/// @cond
template <>
class NestedResumable<1>
{
protected:
	NestedResumable() :
		coLevel(-1), coState(CoStopped)
	{
	}

public:
	void inline
	stopResumable()
	{
		coState = CoStopped;
	}

	bool inline
	isResumableRunning() const
	{
		return !isStoppedCo();
	}

	int8_t inline
	getResumableDepth() const
	{
		return coLevel;
	}

protected:
	CoState inline
	pushCo(uint8_t /*index*/)
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
	stopCo(uint8_t /*index*/)
	{
		coState = CoStopped;
	}

	bool inline
	nestingOkCo() const
	{
#if XPCC_RESUMABLE_CHECK_NESTING_DEPTH
		return (coLevel != 0);
#else
		return true;
#endif
	}

	void inline
	setCo(CoState state, uint8_t /*index*/)
	{
		coState = state;
	}

	bool inline
	isStoppedCo() const
	{
		return (coState == CoStopped);
	}

	template<uint8_t index>
	static void
	checkCoMethods()
	{}

	template<bool isNested>
	static void
	checkCoType()
	{
		static_assert(isNested == true, "You cannot declare an index for a _nested_ coroutine!");
	}

private:
	int_fast8_t coLevel;
	CoState coState;
};
/// @endcond

} // namespace co

} // namespace xpcc

#endif // XPCC_NESTED_RESUMABLE_HPP
