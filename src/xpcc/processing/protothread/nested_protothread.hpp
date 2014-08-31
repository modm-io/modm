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
	// values of Stop and Abort MUST NOT BE CHANGED!
	Stop = 0,					///< Task finished unsuccessfully
	Abort = 1,					///< Task was aborted by user
	NestingError = (1 << 1),	///< Nested protothread has run out of nesting levels to start this task
	Success = (1 << 2),			///< Task finished successfully

	// reasons to wait
	WrongContext = (1 << 4),	///< Task is already running in a different context
	WrongState = (1 << 5),		///< Another task of the same nested protothread is already running in this context
	Starting = (1 << 6),		///< Task is starting

	// reasons to keep running
	Running = (1 << 7),			///< Task is running
};
/// @}
/// Passing this context will abort a tast and invoke its abortion handler code.
static void * const ContextAbort = reinterpret_cast<void *>(1);

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
 * You may include a condition which indicates that the task completed
 * successfully using the `NPT_SUCCESS_IF` macro.
 * This information is returned by the `NPT_SPAWN` macro and can be used
 * to influence your program flow.
 * If the condition within the `NPT_SUCCESS_IF` macro is not met, the tasks
 * continues execution after this macro, which allows you to recover or try again.
 * You may use multiple `NPT_SUCCESS_IF` macros if your task branches internally
 * or if the task was able to recover successfully, or a "retry n-times"
 * mechanism is desired.
 * Since the `NPT_SUCCESS_IF` macro does not automatically yield the task
 * if the condition is not met, it should be used with caution in a while loop.
 *
 * The execution of any running task may be aborted by passing `xpcc::pt::ContextAbort`
 * as a context pointer.
 * This will cause the task to execute the (optional) abortion code defined in
 * `NPT_END_ON_ABORT(code)` and then immediately return `xpcc::pt::Abort`.
 * If the task is currently spawning another task, the abort signal will be
 * handed down to the deepest spawning task, which will then recursively
 * call the abortion codes of the parent tasks, before returning `xpcc::pt::Abort`.
 * An abortion may also be triggered by a task itself by calling `NPT_ABORT()`,
 * which will also execute abortion code before handing the signal to its parent task.
 *
 * Be aware that when an abort signal is received, the entire chain of tasks must
 * return immediately. It is therefore not possible to use any asynchronous calls
 * in the aborting code! (You *can* try, but it might cause a dead-lock!).
 * Therefore instead of using `NPT_ABORT()` inside your tasks, consider letting
 * your task recover its states (asynchronously) and then exit using `NPT_EXIT()`.
 *
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
 *         // NPT_SUCCESS_IF is optional
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
 *         NPT_SUCCESS_IF(timer.isRunning());
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
		return (nptStateArray[nptLevel] != NPtStopped);
	}

	/// @return the nesting depth in the current task, or -1 if called outside a task
	int8_t ALWAYS_INLINE
	getTaskDepth()
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
	 * @return	>=`Starting` if still running, <=`Successful` if it has finished.
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
	nestingOkNPt()
	{
		return (nptLevel < Depth + 1);
	}

	xpcc::pt::Result inline
	getDefaultNPt()
	{
		if (nptStateArray[nptLevel] > NPtAbort)
			return xpcc::pt::WrongState;
		return nptStateArray[nptLevel];
	}

	/// @return `true` if the task is called in the same context, or the context is not set
	///			`false` if the task is claimed by another context
	bool inline
	beginNPt(void *ctx)
	{
		// only one comparison, if this task is called in the same context
		if (ctx == nptContext)
			return true;

		// three comparisons + assignment, if this task is called for the first time
		if (nptContext == 0 && ctx != ContextAbort) {
			nptContext = ctx;
			return true;
		}

		// three comparisons + two comparisons + one assignment, if this running task is aborted
		if (ctx == ContextAbort) {
			// only if this task is currently not spawning we abort,
			// otherweise we forward the abort context to the spawning task
			if (!(nptLevel < Depth && nptStateArray[nptLevel+1] != NPtStopped))
			{
				nptStateArray[nptLevel] = NPtAbort;
			}
			return true;
		}

		return false;
	}
	/// @}

protected:
	static constexpr NPtState NPtAbort = static_cast<NPtState>(Abort);
	static constexpr NPtState NPtStopped = static_cast<NPtState>(Stop);
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
		return (nptState != NPtStopped);
	}

	int8_t ALWAYS_INLINE
	getTaskDepth()
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
	nestingOkNPt()
	{
		return (nptLevel == -1);
	}

	void ALWAYS_INLINE
	setNPt(NPtState state)
	{
		nptState = state;
	}

	xpcc::pt::Result inline
	getDefaultNPt()
	{
		if (nptState > NPtAbort)
			return xpcc::pt::WrongState;
		return nptState;
	}

	bool inline
	beginNPt(void *ctx)
	{
		if (ctx == nptContext)
			return true;

		if (nptContext == 0 && ctx != ContextAbort) {
			nptContext = ctx;
			return true;
		}

		if (ctx == ContextAbort) {
			nptState = NPtAbort;
			return true;
		}

		return false;
	}
	/// @}

protected:
	static constexpr NPtState NPtAbort = static_cast<NPtState>(Abort);
	static constexpr NPtState NPtStopped = static_cast<NPtState>(Stop);
private:
	NPtState nptState;
	int8_t nptLevel;
	void *nptContext;
};
/// @endcond

} // namespace pt

} // namespace xpcc

#endif // XPCC_NPT_THREAD_HPP
