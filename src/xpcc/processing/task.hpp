// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_TASK_HPP
#define	XPCC_TASK_HPP

namespace xpcc
{

/**
 * Interface for a state-machine
 *
 * Every task must provide a start() method which resets the internal state
 * and starts execution. It is not declared here because the task may need
 * some specific information which should be passed as a parameter.
 *
 * @ingroup	processing
 * @author	Fabian Greif
 */
class Task
{
public:
	virtual ~Task()
	{
	}

#ifdef __DOXYGEN__
	void
	start(...);
#endif

	virtual bool
	isFinished() = 0;

	virtual void
	update() = 0;
};

}	// namespace xpcc

#endif	// XPCC_TASK_HPP
