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

#include <stdint.h>
#include "npt_macros.hpp"
#include <xpcc/utils/arithmetic_traits.hpp>
#include <cstring>

namespace xpcc
{

namespace pt
{

enum State
{
	Stop = 0,
	NestingError = 2,
	Starting = 4,
	Running = 6,
};

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
 * Be aware that only one of these tasks can run at the same time, even
 * if it calls other tasks in its implementation.
 * If you need to run two seperate threads, use regular Protothreads.
 *
 * You are responsible to choosing the right nesting depth!
 * This class will guard itself against calling another task at too
 * deep a nesting level and inform you gently of this by returning
 * `false` from `startTask(task)`.
 * It is then up to you to recognise this in your program design
 * and increase the nesting depth or rethink your code.
 *
 * Note that nested protothreads do not replace regular protothreads
 * (not even with a nesting level of zero)!
 * However, you can use a nested protothread within a regular protohread,
 * as you can see in the following (albeit slightly senseless) example:
 *
 * @code
 * #include <xpcc/architecture.hpp>
 * #include <xpcc/utils/protothread.hpp>
 * #include <xpcc/utils/timeout.hpp>
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
 *             this->startTask(TASK_WAIT_FOR_TIMER);
 *             PT_WAIT_UNTIL(runTaskTimer());
 *
 *             Led::reset();
 *             this->startTask(TASK_SET_TIMER);
 *             PT_WAIT_UNTIL(runTaskSetTimer(200));
 *             PT_WAIT_UNTIL(this->timer.isExpired());
 *         }
 *
 *         PT_END();
 *     }
 *
 *     bool
 *     runTaskTimer()
 *     {
 *         NPT_BEGIN(TASK_WAIT_FOR_TIMER);
 *
 *         this->startTask(TASK_SET_TIMER);
 *         NPT_WAIT_UNTIL(runTaskSetTimer(100));
 *
 *         NPT_WAIT_UNTIL(this->timer.isExpired());
 *
 *         NPT_END();
 *     }
 *
 *     bool
 *     runTaskSetTimer(uint16_t timeout)
 *     {
 *         NPT_BEGIN(TASK_SET_TIMER);
 *
 *         this->timer.restart(timeout);
 *
 *         NPT_END();
 *     }
 *
 * private:
 *    enum
 *    {
 *        TASK_SET_TIMER,
 *        TASK_WAIT_FOR_TIMER
 *    };
 *
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
template< uint8_t Depth = 0 >
class NestedProtothread
{
protected:
	/// Used to store a protothread's position (what Dunkels calls a
	/// "local continuation").
	typedef uint16_t NPtState;

public:
	/// Construct a new nested protothread which will be stopped
	NestedProtothread()
	:	nptLevel(0)
	{
		stopTask();
	}

	/// Start a specific task at the current nesting level
	/// @return	`true` if successful, `false` if the nesting is too deep
	bool ALWAYS_INLINE
	startTask(uint8_t task)
	{
		if (nestingOkNPt() && !isTaskRunning())
		{
			nptStateArray[nptLevel] = stateFromTask(task);
			return true;
		}
		return false;
	}

	/// Force the task to stop running at the current nesting level
	/// @warning	This will not allow the task to clean itself up!
	inline void
	stopTask()
	{
		uint8_t level = nptLevel;
		while(level < Depth + 1)
		{
			nptStateArray[level++] = NPtStopped;
		}
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
	 * You can call and run other tasks within your tasks using the
	 * `startTask(task)` method, depending on the nesting depth you choose.
	 *
	 * It is safe to call the method of a task that has not been started,
	 * as it will simply return `false`, but not change any internal status.
	 *
	 * You need to implement this method in you subclass yourself.
	 *
	 * @return	`true` if still running, `false` if it has finished.
	 */
	bool
	runTask();
#endif

protected:
	/// @internal
	/// @{

	// call this in the switch statement!
	// @return current state before increasing nesting level
	NPtState ALWAYS_INLINE
	pushNPt()
	{
		return nptStateArray[nptLevel++];
	}

	// always call this before returning from the run function!
	// decreases nesting level
	void ALWAYS_INLINE
	popNPt()
	{
		nptLevel--;
	}

	// invalidates the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popNPt()!)
	void ALWAYS_INLINE
	stopNPt()
	{
		nptStateArray[nptLevel-1] = NPtStopped;
	}

	// sets the state of the parent nesting level
	// @warning	be aware in which nesting level you call this! (before popNPt()!)
	void ALWAYS_INLINE
	setNPt(NPtState state)
	{
		nptStateArray[nptLevel-1] = state;
	}

	bool ALWAYS_INLINE
	nestingOkNPt()
	{
		return (nptLevel < Depth + 1);
	}

	// the task will be subtracted from the maximum of the NPtState
	// and with this being an uint16_t, you would have to have a file
	// with more than 65.000 lines to come into trouble here.
	// Also if you have more than 255 tasks in the same class, there
	// is something wrong with your concept...
	static constexpr NPtState
	stateFromTask(uint8_t task)
	{
		return static_cast<NPtState>(xpcc::ArithmeticTraits<NPtState>::max - 1 - task);
	}
	/// @}

protected:
	static constexpr NPtState NPtStopped = static_cast<NPtState>(xpcc::ArithmeticTraits<NPtState>::max);
private:
	uint8_t nptLevel;
	NPtState nptStateArray[Depth+1];
};

// ----------------------------------------------------------------------------
// we won't document the specialisation again
/// @cond
template <>
class NestedProtothread<0>
{
public:
	typedef uint16_t NPtState;

	NestedProtothread() :
		nptState(NPtStopped), inNPt(false)
	{
	}

	inline bool
	startTask(uint8_t task)
	{
		if ((nptState == NPtStopped) && !inNPt)
		{
			nptState = stateFromTask(task);
			return true;
		}
		return false;
	}

	void ALWAYS_INLINE
	stopTask()
	{
		nptState = NPtStopped;
	}

	bool ALWAYS_INLINE
	isTaskRunning() const
	{
		return (nptState != NPtStopped);
	}

	int8_t ALWAYS_INLINE
	getTaskDepth()
	{
		return inNPt ? 0 : -1;
	}

protected:
	/// @internal
	/// @{
	NPtState ALWAYS_INLINE
	pushNPt()
	{
		inNPt = true;
		return nptState;
	}

	void ALWAYS_INLINE
	popNPt()
	{
		inNPt = false;
	}

	void ALWAYS_INLINE
	stopNPt()
	{
		nptState = NPtStopped;
	}

	bool ALWAYS_INLINE
	nestingOkNPt()
	{
		return !inNPt;
	}

	void ALWAYS_INLINE
	setNPt(NPtState state)
	{
		nptState = state;
	}

	static constexpr NPtState
	stateFromTask(uint8_t task)
	{
		return static_cast<NPtState>(xpcc::ArithmeticTraits< NPtState >::max - 1 - task);
	}
	/// @}

protected:
	static constexpr NPtState NPtStopped = static_cast<NPtState>(xpcc::ArithmeticTraits<NPtState>::max);
private:
	NPtState nptState;
	bool inNPt;
};
/// @endcond

} // namespace pt

} // namespace xpcc

#endif // XPCC_NPT_THREAD_HPP
