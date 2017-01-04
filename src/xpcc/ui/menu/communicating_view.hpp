/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
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

#ifndef XPCC__COMMUNICATING_VIEW_HPP
#define XPCC__COMMUNICATING_VIEW_HPP

#include <modm/communication/xpcc/communicatable.hpp>

#include "communicating_view_stack.hpp"

namespace xpcc
{
	class CommunicatingView : public xpcc::Communicatable
	{
	public:
		CommunicatingView(xpcc::CommunicatingViewStack* /*stack*/)
		{
		}
		
	protected:
		inline xpcc::CommunicatingViewStack*
		getCommunicatingViewStack(xpcc::ViewStack* viewStack)
		{
			return static_cast<xpcc::CommunicatingViewStack*>(viewStack);
		}
	};
}

#endif // XPCC__COMMUNICATING_VIEW_HPP
