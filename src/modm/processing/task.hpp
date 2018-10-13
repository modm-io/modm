/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_TASK_HPP
#define	MODM_TASK_HPP

namespace modm
{

/**
 * Interface for a state-machine
 *
 * Every task must provide a start() method which resets the internal state
 * and starts execution. It is not declared here because the task may need
 * some specific information which should be passed as a parameter.
 *
 * @ingroup	modm_processing
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

}	// namespace modm

#endif	// MODM_TASK_HPP
