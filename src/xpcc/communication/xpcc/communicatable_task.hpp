// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_COMMUNICATABLE_TASK_HPP
#define	XPCC_COMMUNICATABLE_TASK_HPP

#include "communicatable.hpp"
#include "communicator.hpp"

#include <xpcc/processing/task.hpp>

namespace xpcc
{

/**
 * A state-machine able to communicate via xpcc
 *
 * Needs to be part of a xpcc::AbstractComponent
 *
 * @see		xpcc::Task
 * @see		xpcc::Communicator
 * @see		xpcc::Communicatable
 *
 * @ingroup	xpcc_com
 * @author	Fabian Greif
 */
class CommunicatableTask : public Task, public Communicatable
{
public:
	CommunicatableTask(Communicator *parent) :
		parent(parent)
	{
	}

protected:
	Communicator *parent;
};

}	// namespace xpcc

#endif	// XPCC_COMMUNICATABLE_TASK_HPP
