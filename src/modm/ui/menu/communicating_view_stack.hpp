/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COMMUNICATING_VIEW_STACK_HPP
#define MODM_COMMUNICATING_VIEW_STACK_HPP

#include <modm/communication/modm/communicator.hpp>
#include <modm/communication/modm/communicatable.hpp>

#include "view_stack.hpp"

namespace modm
{
	class CommunicatingViewStack : public ViewStack
	{
	public:
		CommunicatingViewStack(modm::GraphicDisplay* display, modm::Communicator* communicator) :
			ViewStack(display),
			communicator(communicator)
		{
		}
		
		inline modm::Communicator*
		getCommunicator()
		{
			return communicator;
		}
		
	protected:
		modm::Communicator* communicator;
	};
}

#endif // MODM_COMMUNICATING_VIEW_STACK_HPP
