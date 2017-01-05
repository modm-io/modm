/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GUI_TAB_PANE_HPP
#define MODM_GUI_TAB_PANE_HPP

#include <modm/debug/logger.hpp>

#include "../../widgets.hpp"

namespace modm
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class TabPanel : public WidgetGroup
{
public:
	TabPanel(Dimension d, float verticalRatio = 0.2) :
		WidgetGroup(d),
		panelDimension(Dimension(d.width, d.height * (1 - verticalRatio))),
		buttonDimension(Dimension(d.width / 3, d.height * verticalRatio))
	{
	}

	void
	packPanel(Widget* w);

	void
	packTabLeft(Widget* w);

	void
	packTabMiddle(Widget* w);

	void
	packTabRight(Widget* w);

public:
	const Dimension panelDimension;
	const Dimension buttonDimension;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_TAB_PANE_HPP
