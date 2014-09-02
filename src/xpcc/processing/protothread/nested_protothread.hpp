// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_NPT_THREAD_HPP
#define XPCC_NPT_THREAD_HPP

#include "npt_macros.hpp"
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{

namespace pt
{

/// State returned for all tasks in nested protothreads.
/// @{
typedef uint8_t Result;

enum
{
	// reasons to stop
	Stop = 0,					///< Task finished unsuccessfully
	NestingError = (1 << 0),	///< Nested protothread has run out of nesting levels to start this task
	Success = (1 << 1),			///< Task finished successfully

	// reasons to wait
	WrongContext = (1 << 5),	///< Task is already running in a different context
	WrongState = (1 << 6),		///< Another task of the same nested protothread is already running in this context

	// reasons to keep running
	Running = (1 << 7),			///< Task is running
};
/// @}

/**
 * A version of `Protothread` which allows nested calling of tasks.
 *
 * This base class and its macros allows you to implement and use several
 * protothreads tasks in one class.
 * This allows you to modularize your code by placing it into its own methods
 * instead of the placing everything into the `run()` method.
 * It also allows you to call and run tasks within your tasks, so you can
 * reuse their functionality.
 *
 * You are responsible to choosing the right nesting depth!
 * This class will guard itself against calling another task at too
 * deep a nesting level and inform you gently of this by returning
 * `xpcc::pt::NestingError` from your called `task(ctx)`.
 * It is then up to you to recognise this in your program design
 * and increase the nesting depth or rethink your code.
 *
 * Be aware that only one of these tasks can run at the same time.
 * Even if you call other tasks, they will not allow you to run them
 * until the conflicting tasks finished.
 *
 * Each task requires a context pointer, which is used to ensure the
 * task only executes in the context that it was originally started from.
 * If you call a task in a different context, it will return
 * `xpcc::pt::WrongContext`.
 * Similarly, if you call a different task of the same class in
 * the same context, while another task is running, it will return
 * `xpcc::pt::WrongState`.
 * Using the `NPT_SPAWN` macro, you can wait for these tasks to become
 * available, so you usually do not need to worry about those cases.
 *
 * You may exit the task successfully by using `NPT_EXIT_SUCCESS`.
 * This information is returned by the `NPT_SPAWN` macro and can be used
 * to influence your program flow.
 * You may exit the task unsuccessfully by using `NPT_EXIT()`.
 * If the task reaches `NPT_END()` it will exit unsuccessfully automatically.
 *
 * Note that you must call nested protothread tasks within a regular protothread.
 * It is sufficient to use the `this` pointer of the class as context when calling the tasks.
 *
 * Here is a (slightly over-engineered) example:
 *
 * @code
 * #include <xpcc/architecture.hpp>
 * #include <xpcc/processing/protothread.hpp>
 * #include <xpcc/processing/timeout.hpp>
 *
 * typedef GpioOutputB0 Led;
 *
 * class BlinkingLight : public xpcc::pt::Protothread, private xpcc::pt::NestedProtothread<1>
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
 *             PT_RUN_TASK(waitForTimer(this)))
 *
 *             Led::reset();
 *             PT_RUN_TASK(setTimer(this, 200));
 *
 *             PT_WAIT_UNTIL(timer.isExpired());
 *         }
 *
 *         PT_END();
 *     }
 *
 *     xpcc::pt::Result
 *     waitForTimer(void *ctx)
 *     {
 *         NPT_BEGIN(ctx);
 *
 *         // nested calling is allowed
 *         if (NPT_SPAWN(setTimer(ctx, 100)))
 *         {
 *             NPT_WAIT_UNTIL(timer.isExpired());
 *         }
 *
 *         // NPT_EXIT_SUCCESS is optional
 *
 *         NPT_END();
 *     }
 *
 *     xpcc::pt::Result
 *     setTimer(void *ctx, uint16_t timeout)
 *     {
 *         NPT_BEGIN(ctx);
 *
 *         timer.restart(timeout);
 *
 *         if(timer.isRunning())
 *             NPT_EXIT_SUCCESS();
 *
 *         // clean up code goes here
 *
 *         NPT_END();
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
 * @ingroup	protothread
 * @author	Niklas Hauser
 * @tparam	Depth	the nesting depth: the maximum of tasks that are called within tasks (should be < 128).
 */
template< uint8_t Depth >
class NestedProtothread
{
protected:
	/// Used to store a protothread's position (what Dunkels calls a
	/// "local continuation").
	typedef uint16_t NPtState;

	/// Construct a new nested protothread which will be stopped
	NestedProtothread()
	:	nptLevel(0), nptContext(0)
	{
		this->stopTask();
	}

public:
	/// Force the task to stop running at the current nesting level
	/// @warning	This will not allow the task to clean itself up!
	inline void
	stopTask()
	{
		uint_fast8_t level = nptLevel;
		while (level < Depth + 1)
		{
			nptStateArray[level++] = NPtStopped;
		}
		if (nptLevel == 0)
			nptContext = 0;
	}

	/// @return	`true` if a task is running at the current nesting level, else `false`
	bool ALWAYS_INLINE
	isTaskRunning() const
	{
		return !isStoppedNPt();
	}

	/// @return the nesting depth in the current task, or -1 if called outside a task
	int8_t ALWAYS_INLINE
	getTaskDepth() const
	{
		return static_cast<int8_t>(nptLevel) - 1;
	}

#ifdef __DOXYGEN__
	/**
	 * Run the task.
	 *
	 * This handles very much like a protothread, however you can have more than
	 * one task in your class.
	 *
	 * You need to implement this method in you subclass yourself.
	 *
	 * @return	>`Success` if still running, <=`Success` if it has finished.
	 */
	xpcc::pt::Result
	task(void *ctx, ...);
#endif

protected:
	/// @internal
	/// @{

	/// increases nesting level, call this in the switch statement!
	/// @return current state before increasing nesting level
	NPtState ALWAYS_INLINE
	pushNPt()
	{
		return nptStateArray[nptLevel++];
	}

	/// always call this before returning from the run function!
	/// decreases nesting level
	void ALWAYS_INLINE
	popNPt()
	{
		nptLevel--;
	}

	// invalidates the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popNPt()!)
	void inline
	stopNPt()
	{
		nptStateArray[nptLevel-1] = NPtStopped;
		if (nptLevel == 1)
			nptContext = 0;
	}

	/// sets the state of the parent nesting level
	/// @warning	be aware in which nesting level you call this! (before popNPt()!)
	void ALWAYS_INLINE
	setNPt(NPtState state)
	{
		nptStateArray[nptLevel-1] = state;
	}

	/// @return `true` if the nesting depth allows for another level.
	/// @warning	be aware in which nesting level you call this! (before pushNPt()!)
	bool ALWAYS_INLINE
	nestingOkNPt() const
	{
		return (nptLevel < Depth + 1);
	}

	bool ALWAYS_INLINE
	isStoppedNPt() const
	{
		return (nptStateArray[nptLevel] == NPtStopped);
	}

	/// @return `true` if the task is called in the same context, or the context is not set
	///			`false` if the task is claimed by another context
	bool inline
	beginNPt(void *ctx)
	{
		// only one comparison, if this task is called in the same context
		if (ctx == nptContext)
			return true;

		// two comparisons + assignment, if this task is called for the first time
		if (nptContext == 0)
		{
			nptContext = ctx;
			return true;
		}

		return false;
	}
	/// @}

protected:
	static constexpr NPtState NPtStopped = static_cast<NPtState>(0);
private:
	uint8_t nptLevel;
	NPtState nptStateArray[Depth+1];
	void *nptContext;
};

// ----------------------------------------------------------------------------
// we won't document the specialisation again
/// @cond
template <>
class NestedProtothread<0>
{
protected:
	typedef uint16_t NPtState;

	NestedProtothread() :
		nptState(NPtStopped), nptLevel(-1), nptContext(0)
	{
	}

public:
	void ALWAYS_INLINE
	stopTask()
	{
		this->stopNPt();
	}

	bool ALWAYS_INLINE
	isTaskRunning() const
	{
		return !isStoppedNPt();
	}

	int8_t ALWAYS_INLINE
	getTaskDepth() const
	{
		return nptLevel;
	}

protected:
	/// @internal
	/// @{
	NPtState ALWAYS_INLINE
	pushNPt()
	{
		nptLevel = 0;
		return nptState;
	}

	void ALWAYS_INLINE
	popNPt()
	{
		nptLevel = -1;
	}

	void ALWAYS_INLINE
	stopNPt()
	{
		nptState = NPtStopped;
		nptContext = 0;
	}

	bool ALWAYS_INLINE
	nestingOkNPt() const
	{
		return (nptLevel != 0);
	}

	void ALWAYS_INLINE
	setNPt(NPtState state)
	{
		nptState = state;
	}

	bool ALWAYS_INLINE
	isStoppedNPt() const
	{
		return (nptState == NPtStopped);
	}

	bool inline
	beginNPt(void *ctx)
	{
		if (ctx == nptContext)
			return true;

		if (nptContext == 0)
		{
			nptContext = ctx;
			return true;
		}

		return false;
	}
	/// @}

protected:
	static constexpr NPtState NPtStopped = static_cast<NPtState>(0);
private:
	NPtState nptState;
	int8_t nptLevel;
	void *nptContext;
};
/// @endcond

} // namespace pt

} // namespace xpcc

#endif // XPCC_NPT_THREAD_HPP
