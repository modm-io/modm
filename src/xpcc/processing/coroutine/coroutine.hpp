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

namespace xpcc
{

namespace co
{

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

/// All Coroutines return an encapsulated result type.
template < typename T >
struct Result
{
	uint8_t state;
	T result;
};
/// void is not an object type
/// @internal
template <>
struct Result<void>
{
	uint8_t state;
	uint8_t result;
};

/// Used to store a coroutines's position (what Dunkels calls a
/// "local continuation").
typedef uint8_t CoState;

/**
 * An implementation of Coroutines which allow for nested calling.
 *
 * This base class and its macros allows you to implement and use several
 * coroutines in one class.
 * This allows you to modularize your code by placing it into its own coroutines
 * instead of the placing everything into one big method.
 * It also allows you to call and run coroutines within your coroutines,
 * so you can reuse their functionality.
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
 * You may exit and return a value by using `CO_RETURN(value)`.
 * This information is wrapped in `xpcc::co::Result<Type>` struct
 * and transparently returned by the `CO_CALL` macro so it can be used
 * to influence your program flow.
 * If the coroutine reaches `CO_END()` it will exit automatically,
 * however the return type is set to `0` or equivalent.
 *
 * Note that you should call coroutines within a protothreads.
 * It is sufficient to use the `this` pointer of the class as context
 * when calling the coroutines.
 *
 * Here is a (slightly over-engineered) example:
 *
 * @code
 * #include <xpcc/architecture.hpp>
 * #include <xpcc/processing/protothread.hpp>
 * #include <xpcc/coroutine/coroutine.hpp>
 * #include <xpcc/processing/timeout.hpp>
 *
 * typedef GpioOutputB0 Led;
 *
 * class BlinkingLight : public xpcc::pt::Protothread, private xpcc::pt::NestedCoroutine<1>
 * {
 * public:
 *     bool
 *     run()
 *     {
 *         PT_BEGIN();
 *
 *         // set everything up
 *         Led::setOutput();
 *         Led::set();
 *
 *         while (true)
 *         {
 *             Led::set();
 *             PT_CALL(waitForTimer(this)))
 *
 *             Led::reset();
 *             PT_CALL(setTimer(this, 200));
 *
 *             PT_WAIT_UNTIL(timer.isExpired());
 *         }
 *
 *         PT_END();
 *     }
 *
 *     xpcc::co::Result<bool>
 *     waitForTimer(void *ctx)
 *     {
 *         CO_BEGIN(ctx);
 *
 *         // nested calling is allowed
 *         if (CO_CALL(setTimer(ctx, 100)))
 *         {
 *             CO_WAIT_UNTIL(timer.isExpired());
 *         }
 *
 *         // CO_RETURN is optional
 *
 *         CO_END();
 *     }
 *
 *     xpcc::co::Result<bool>
 *     setTimer(void *ctx, uint16_t timeout)
 *     {
 *         CO_BEGIN(ctx);
 *
 *         timer.restart(timeout);
 *
 *         if(timer.isRunning())
 *             CO_RETURN(true);
 *
 *         // clean up code goes here
 *
 *         CO_END();
 *     }
 *
 * private:
 *     xpcc::Timeout<> timer;
 * };
 *
 *
 * ...
 * BlinkingLight light;
 *
 * while (...) {
 *     light.run();
 * }
 * @endcode
 *
 * For other examples take a look in the `examples` folder in the XPCC
 * root folder.
 *
 * @ingroup	coroutine
 * @author	Niklas Hauser
 * @tparam	Depth	maximum nesting depth: the maximum number of
 * 					coroutines that are called within coroutines (should be <128).
 */
template< uint8_t Depth >
class NestedCoroutine
{
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
	/**
	 * Run the coroutine.
	 *
	 * You need to implement this method in you subclass yourself.
	 *
	 * @return	>`NestingError` if still running, <=`NestingError` if it has finished.
	 */
	xpcc::co::Result< ReturnType >
	coroutine(void *ctx, ...);
#endif

protected:
	/// @internal
	/// @{

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
		return (coLevel < Depth + 1);
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
	/// @}

protected:
	static constexpr CoState CoStopped = static_cast<CoState>(0);
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
	/// @internal
	/// @{
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
		return (coLevel != 0);
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
	/// @}

protected:
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
