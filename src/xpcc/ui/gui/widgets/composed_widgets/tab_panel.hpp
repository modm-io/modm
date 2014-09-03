#ifndef TAB_PANE_HPP_
#define TAB_PANE_HPP_

#include <xpcc/debug/logger.hpp>

#include "../../widgets.hpp"

namespace xpcc
{
namespace gui
{


class TabPanel : public WidgetGroup {

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

}
}

#endif /* TAB_PANE_HPP_ */
