/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
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

#ifndef MODM_COMMUNICATING_VIEW_HPP
#define MODM_COMMUNICATING_VIEW_HPP

#include <modm/communication/modm/communicatable.hpp>

#include "communicating_view_stack.hpp"

namespace modm
{
	class CommunicatingView : public modm::Communicatable
	{
	public:
		CommunicatingView(modm::CommunicatingViewStack* /*stack*/)
		{
		}
		
	protected:
		inline modm::CommunicatingViewStack*
		getCommunicatingViewStack(modm::ViewStack* viewStack)
		{
			return static_cast<modm::CommunicatingViewStack*>(viewStack);
		}
	};
}

#endif // MODM_COMMUNICATING_VIEW_HPP
