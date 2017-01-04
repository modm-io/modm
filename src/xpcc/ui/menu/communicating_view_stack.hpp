/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__COMMUNICATING_VIEW_STACK_HPP
#define XPCC__COMMUNICATING_VIEW_STACK_HPP

#include <modm/communication/xpcc/communicator.hpp>
#include <modm/communication/xpcc/communicatable.hpp>

#include "view_stack.hpp"

namespace xpcc
{
	class CommunicatingViewStack : public ViewStack
	{
	public:
		CommunicatingViewStack(xpcc::GraphicDisplay* display, xpcc::Communicator* communicator) :
			ViewStack(display),
			communicator(communicator)
		{
		}
		
		inline xpcc::Communicator*
		getCommunicator()
		{
			return communicator;
		}
		
	protected:
		xpcc::Communicator* communicator;
	};
}

#endif // XPCC__COMMUNICATING_VIEW_STACK_HPP
