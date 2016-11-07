/*
 * Copyright (c) 2014-2015, Niklas Hauser
 * Copyright (c) 2014, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
 * If the allocated nesting level is exceeded, the resumable function does not execute, but returns
 * the `xpcc::rf::NestingError` state value.
 * However, the `PT_CALL()` or `RF_CALL()` macros are not constructed to handle this error and
 * will interpret this error as a normal resumable function stop and therefore immediately continue program
 * execution.
 *
 * Since the required nesting depth of a nested resumable function can be known before runtime
 * by looking at the source code and counting the nesting levels, this check acts only as a
 * protection against executing random code, when entering too many levels.
 * So instead of the entire program crashing, the resumable function simply does not execute at all,
 * which can make it easier to track down the problem too few allocated nesting levels.
 *
 * You as a programmer are responible for allocating the correct amount of nesting levels!
 * However, if you are sure that you have counted the levels correctly and your code is
 * not breaking, you may remove this check from all resumable functions in your `project.cfg`:
 *
@verbatim
[defines]
XPCC_RESUMABLE_CHECK_NESTING_DEPTH = false
@endverbatim
 *
 * @see	NestedResumable
 * @ingroup	resumable
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

/**
 * This is the base class which must be inherited from for using
 * nested resumable functions in your class.
 *
 * You are responsible to choosing the right nesting depth!
 * This class will guard itself against calling another resumable function at too
 * deep a nesting level and inform you gently of this by returning
 * `xpcc::rf::NestingError` from your called `resumable()`.
 * It is then up to you to recognise this in your program design
 * and increase the nesting depth or rethink your code.
 * You may disable the check by setting `XPCC_RESUMABLE_CHECK_NESTING_DEPTH`
 * to `false` in your project configuration.
 *
 * The resumable functions of this class are mutually exclusive, so only one
 * resumable function of the same object can run at the same time. Even if you
 * call another resumable function, it will simply return `xpcc::rf::WrongState`.
 * Using the `RF_CALL(resumable())` macro, you can wait for these
 * resumable functions to become available and then run them, so you usually do
 * not need to worry about those cases.
 *
 * You must begin each resumable function using `RF_BEGIN()`.
 * You may exit and return a value by using `RF_RETURN(value)` or
 * return the result of another resumable function using `RF_RETURN_CALL(resumable())`.
 * This return value is wrapped in a `xpcc::ResumableResult<Type>` struct
 * and transparently returned by the `RF_CALL` macro so it can be used
 * to influence your program flow.
 * If the resumable function reaches `RF_END()` it will exit automatically,
 * with the result of `0` cast to the return type.
 * Should you wish to return a value at the end, you may use
 * `RF_END_RETURN(value)`.
 * You may also return the result of another resumable function using
 * `RF_END_RETURN_CALL(resumable())`.
 *
 * @warning	**Resumables are not thread-safe!** If two threads access the
 * 			same resumable function, you must use a Mutex to regulate access to it.
 *
 * @ingroup	resumable
 * @author	Niklas Hauser
 * @tparam	Levels	maximum number of nesting levels (should be <128):
 * 					max(number of resumable functions that are called within resumable functions) + 1
 */
template< uint8_t Levels = 1>
class NestedResumable
{
	static_assert(Levels > 0, "The number of nesting levels must be at least 1!");

protected:
	/// Construct a new class with nested resumable functions
	NestedResumable()
	:	rfLevel(0)
	{
		for (rf::State &level : rfStateArray)
		{
			level = rf::Stopped;
		}
	}

public:
	/// Force all resumable functions to stop running at the current nesting level
	inline void
	stopResumable()
	{
		uint_fast8_t level = rfLevel;
		while (level < Levels)
		{
			rfStateArray[level++] = rf::Stopped;
		}
	}

	/// @return	`true` if a resumable function is running at the current nesting level, else `false`
	bool inline
	isResumableRunning() const
	{
		return !isStoppedRf();
	}

	/// @return the nesting depth in the current resumable function, or -1 if called outside any resumable function
	int8_t inline
	getResumableDepth() const
	{
		return static_cast<int8_t>(rfLevel) - 1;
	}

#ifdef __DOXYGEN__
	/// @cond
	/**
	 * Run the resumable function.
	 *
	 * You need to implement this method in you subclass yourself.
	 *
	 * @return	>`NestingError` if still running, <=`NestingError` if it has finished.
	 */
	xpcc::ResumableResult< ReturnType >
	resumable function(...);
	/// @endcond
#endif

protected:
	/// @cond

	/// increases nesting level, call this in the switch statement!
	/// @return current state before increasing nesting level
	rf::State inline
	pushRf(uint8_t /*index*/)
	{
		return rfStateArray[rfLevel++];
	}

	/// always call this before returning from the run function!
	/// decreases nesting level
	void inline
	popRf()
	{
		rfLevel--;
	}

	// invalidates the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popRf()!)
	void inline
	stopRf(uint8_t /*index*/)
	{
		rfStateArray[rfLevel-1] = rf::Stopped;
	}

	/// sets the state of the parent nesting level
	/// @warning	be aware in which nesting level you call this! (before popRf()!)
	void inline
	setRf(rf::State state, uint8_t /*index*/)
	{
		rfStateArray[rfLevel-1] = state;
	}

	/// @return `true` if the nesting depth allows for another level.
	/// @warning	be aware in which nesting level you call this! (before pushRf()!)
	bool inline
	nestingOkRf() const
	{
#if XPCC_RESUMABLE_CHECK_NESTING_DEPTH
		return (rfLevel < Levels);
#else
		return true;
#endif
	}

	/// @return	`true` if `stopRf()` has been called before
	bool inline
	isStoppedRf() const
	{
		return (rfStateArray[rfLevel] == rf::Stopped);
	}

	/// compatibility with Resumable class
	template<uint8_t index>
	static void
	checkRfFunctions()
	{}

	/// asserts that this method is called in this parent class
	template<bool isNested>
	static void
	checkRfType()
	{
		static_assert(isNested == true, "You cannot declare an index for a _nested_ resumable function!");
	}
	/// @endcond
private:
	uint_fast8_t rfLevel;
	rf::State rfStateArray[Levels];
};

// ----------------------------------------------------------------------------
// we won't document the specialisation again
/// @cond
template <>
class NestedResumable<1>
{
protected:
	NestedResumable() :
		rfLevel(-1), rfState(rf::Stopped)
	{
	}

public:
	void inline
	stopResumable()
	{
		rfState = rf::Stopped;
	}

	bool inline
	isResumableRunning() const
	{
		return !isStoppedRf();
	}

	int8_t inline
	getResumableDepth() const
	{
		return rfLevel;
	}

protected:
	rf::State inline
	pushRf(uint8_t /*index*/)
	{
		rfLevel = 0;
		return rfState;
	}

	void inline
	popRf()
	{
		rfLevel = -1;
	}

	void inline
	stopRf(uint8_t /*index*/)
	{
		rfState = rf::Stopped;
	}

	bool inline
	nestingOkRf() const
	{
#if XPCC_RESUMABLE_CHECK_NESTING_DEPTH
		return (rfLevel != 0);
#else
		return true;
#endif
	}

	void inline
	setRf(rf::State state, uint8_t /*index*/)
	{
		rfState = state;
	}

	bool inline
	isStoppedRf() const
	{
		return (rfState == rf::Stopped);
	}

	template<uint8_t index>
	static void
	checkRfFunctions()
	{}

	template<bool isNested>
	static void
	checkRfType()
	{
		static_assert(isNested == true, "You cannot declare an index for a _nested_ resumable function!");
	}

private:
	int_fast8_t rfLevel;
	rf::State rfState;
};
/// @endcond

} // namespace xpcc

#endif // XPCC_NESTED_RESUMABLE_HPP
