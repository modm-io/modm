/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2013, 2017-2018, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2020, Matthew Arnold
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

#include <modm/communication/xpcc/communicator.hpp>
#include <modm/communication/xpcc/communicatable.hpp>

#include "view_stack.hpp"

namespace modm
{
	/// @ingroup modm_ui_menu
	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	class CommunicatingViewStack : public ViewStack<Allocator>
	{
	public:
		CommunicatingViewStack(modm::GraphicDisplay* display, xpcc::Communicator* communicator) :
			ViewStack<Allocator>(display),
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

#endif // MODM_COMMUNICATING_VIEW_STACK_HPP
