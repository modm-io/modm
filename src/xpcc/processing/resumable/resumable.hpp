// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_RESUMABLE_HPP
#define XPCC_RESUMABLE_HPP

#include "macros.hpp"
#include <xpcc/architecture/utils.hpp>
#include <stdint.h>
#include <initializer_list>

namespace xpcc
{

namespace rf
{

/// @cond
/// State of a resumable function.
enum
ResultState
{
	// reasons to stop
	Stop = 0,			///< Resumable function finished
	NestingError = 1,	///< Nested resumable function has run out of nesting levels

	// reasons to wait
	WrongState = 127,	///< A conflicting nested resumable function of the same class is already running

	// reasons to keep running
	Running = 255,		///< Resumable function is running
};

/// Used to store a resumable function's position (what Dunkels calls a
/// "local continuation").
typedef uint8_t State;
/// We use 0 instead of -1 since it might be fast to check
static constexpr State Stopped = State(0);
/// @endcond

} // namespace rf

/// All resumable functions return an encapsulated result type.
/// @warning The result type **must** have a default constructor!
template < typename T >
struct ResumableResult
{
	/// Return only the `state`. The `result` will be initialized by it's default constructor.
	ResumableResult(uint_fast8_t state) : state(state) {}
	/// Return `state` and valid `result`.
	ResumableResult(uint_fast8_t state, T result) : state(state), result(result) {}

	inline uint_fast8_t
	getState()
	{ return state; }

	inline T
	getResult()
	{ return result; }

private:
	/// The `rf::ResultState`.
	uint_fast8_t state;
	/// Custom return value.
	T result;
};
/// @cond
/// void is not an object type
template <>
struct ResumableResult<void>
{
	/// Return only the `state`. There is no result.
	ResumableResult(uint_fast8_t state) : state(state) {}
	/// Constructor with dummy result needed by the `CO_CALL_BLOCKING` macro.
	ResumableResult(uint_fast8_t state, uint_fast8_t /*dummy_result*/) : state(state) {}

	inline uint_fast8_t
	getState()
	{ return state; }

	inline uint_fast8_t
	getResult()
	{ return state; }

private:
	/// The `rf::ResultState`.
	uint_fast8_t state;
};
/// @endcond

/**
 * Resumable base class.
 *
 * This is the base class which must be inherited from for using
 * resumable functions in your class.
 * You need to declare the number of resumable functions in your class!
 *
 * You must begin each resumable function using `CO_BEGIN(index)` where `index` is
 * the unique index of your resumable function starting at zero.
 * You may exit and return a value by using `CO_RETURN(value)` or
 * return the result of another resumable function using `CO_RETURN_CALL(resumable())`.
 * This return value is wrapped in a `xpcc::ResumableResult<Type>` struct
 * and transparently returned by the `CO_CALL` macro so it can be used
 * to influence your program flow.
 * If the resumable function reaches `CO_END()` it will exit automatically,
 * with the result of `0` cast to the return type.
 * Should you wish to return a value at the end, you may use
 * `CO_END_RETURN(value)`.
 * You may also return the result of another resumable function using
 * `CO_END_RETURN_CALL(resumable())`.
 *
 * Be aware that this class keeps a seperate state for each of your resumable functions.
 * This allows each resumable function to be run at the same time.
 * This might require the use of an internal Semaphore or Mutex if such
 * dependencies exist in your use case.
 * Take a look at the `NestedResumable` class for mutually exclusive resumable functions,
 * which also require a little less memory.
 *
 * @warning	**Resumable functions are not thread-safe!** If two threads access the
 * 			same resumable function, you must use a Mutex to regulate access to it.
 *
 * @see NestedResumable
 *
 * @ingroup	resumable
 * @author	Niklas Hauser
 * @tparam	Functions	the number of resumable functions that are available in this class. Must be non-zero!
 */
template< uint8_t Functions = 1>
class Resumable
{
	static_assert(Functions > 0, "The number of resumable functions must be at least 1!");

protected:
	Resumable()
	{
		stopAllResumables();
	}

public:
	/// Force all resumable functions to stop running
	inline void
	stopAllResumables()
	{
		for (rf::State &state : rfStateArray)
		{
			state = rf::Stopped;
		}
	}

	/// Force the specified resumable function to stop running
	inline bool
	stopResumable(uint8_t id)
	{
		if (id < Functions) {
			rfStateArray[id] = rf::Stopped;
			return true;
		}
		return false;
	}

	/// @return	`true` if the specified resumable function is running, else `false`
	bool inline
	isResumableRunning(uint8_t id) const
	{
		return (id < Functions and rfStateArray[id] != rf::Stopped);
	}

	/// @return	`true` if any resumable function of this class is running, else `false`
	bool inline
	areAnyResumablesRunning() const
	{
		for (const rf::State state : rfStateArray)
		{
			if (state != rf::Stopped)
				return true;
		}
		return false;
	}

	/// @return	`true` if any of the specified resumable function are running, else `false`
	bool inline
	areAnyResumablesRunning(std::initializer_list<uint8_t> ids) const
	{
		for (uint8_t id : ids)
		{
			if (isResumableRunning(id))
				return true;
		}
		return false;
	}

	/// @return	`true` if all of the specified resumable function are running, else `false`
	bool inline
	areAllResumablesRunning(std::initializer_list<uint8_t> ids) const
	{
		for (uint8_t id : ids)
		{
			// = not isResumableRunning(id)
			if (id >= Functions or rfStateArray[id] == rf::Stopped)
				return false;
		}
		return true;
	}

	/// @return	`true` if none of the specified resumable function are running, else `false`
	bool inline
	joinResumables(std::initializer_list<uint8_t> ids) const
	{
		return not areAnyResumablesRunning(ids);
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
	resumable(...);
	/// @endcond
#endif

protected:
	/// @cond

	/// increases nesting level, call this in the switch statement!
	/// @return current state before increasing nesting level
	rf::State ALWAYS_INLINE
	pushRf(uint_fast8_t index) const
	{
		return rfStateArray[index];
	}

	/// always call this before returning from the run function!
	/// decreases nesting level
	void ALWAYS_INLINE
	popRf() const
	{}

	// invalidates the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popRf()!)
	void ALWAYS_INLINE
	stopRf(uint_fast8_t index)
	{
		rfStateArray[index] = rf::Stopped;
	}

	/// sets the state of the parent nesting level
	/// @warning	be aware in which nesting level you call this! (before popRf()!)
	void ALWAYS_INLINE
	setRf(rf::State state, uint_fast8_t index)
	{
		rfStateArray[index] = state;
	}

	bool ALWAYS_INLINE
	nestingOkRf() const
	{
		return true;
	}

	/// @return	`true` if `stopRf()` has been called before
	bool ALWAYS_INLINE
	isStoppedRf(uint_fast8_t index) const
	{
		return (rfStateArray[index] == rf::Stopped);
	}

	/// asserts the index is not out of bounds.
	template<uint8_t index>
	static void
	checkRfFunctions()
	{
		static_assert(index < Functions,
				"Index out of bounds! Increase the `Functions` template argument of your Resumable class.");
	}

	/// asserts that this method is called in this parent class
	template<bool isNested>
	static void
	checkRfType()
	{
		static_assert(isNested == false, "You must declare an index for this resumable function!");
	}
	/// @endcond

private:
	rf::State rfStateArray[Functions];
};

} // namespace xpcc

#endif // XPCC_RESUMABLE_HPP
