// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_TAB_PANE_HPP
#define XPCC_GUI_TAB_PANE_HPP

#include <xpcc/debug/logger.hpp>

#include "../../widgets.hpp"

namespace xpcc
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

}	// namespace xpcc

#endif  // XPCC_GUI_TAB_PANE_HPP
