#ifndef WIDGET_HPP_
#define WIDGET_HPP_

#include <xpcc/ui/display/graphic_display.hpp>
#include <xpcc/ui/gui/colorpalette.hpp>
#include <xpcc/ui/gui/types.hpp>

#include "../abstract_view.hpp"

#include <xpcc/debug/logger.hpp>


namespace xpcc {

namespace gui {

static int16_t uid_global = 0;

class AbstractView;

class Widget {

public:

	Widget() :
		dimension(Dimension(0,0)),
		activated(false),
		cb_activate(NULL),
		cb_deactivate(NULL),
		color_palette(NULL),
		position(xpcc::glcd::Point(0,0)),
		dirty(true)
	{
		// assign unique id
		uid = uid_global++;
	}

	virtual
	~Widget() {}

	virtual void
	draw(AbstractView* view) = 0;

	virtual void
	activate(const InputEvent& ev, void* data) {

		this->activated = true;
		this->dirty = true;

		// call callback when set
		if(cb_activate != NULL)
			this->cb_activate(ev, this, data);
	};

	virtual void
	deactivate(const InputEvent& ev, void* data) {

		this->activated = false;
		this->dirty = true;

		// call callback when set
		if(cb_deactivate != NULL)
			this->cb_deactivate(ev, this, data);
	};

	ColorPalette*
	getColorPalette()
	{
		return this->color_palette;
	}

	virtual void
	setColorPalette(ColorPalette* cb)
	{
		this->color_palette = cb;
	}

	virtual void
	setPosition(const xpcc::glcd::Point& pos)
	{
		this->position = pos;
	}

	xpcc::glcd::Point
	getPosition()
	{
		return this->position;
	}

	uint16_t
	getWidth()
	{
		return this->dimension.width;
	}

	uint16_t
	getHeight()
	{
		return this->dimension.height;
	}

	virtual bool
	isDirty()
	{
		return this->dirty;
	}

	virtual void
	markDrawn()
	{
		this->dirty = false;
	}

public:
	// Unique id for every widget
	int16_t uid;

	// dimensions
	Dimension dimension;

	// where this widget is active or not (e.g. clicked)
	bool activated;

	// callbacks for activation and deactivation event
	eventCallback cb_activate, cb_deactivate;

	// for now unused, maybe needed later for custom styles
	ColorPalette *color_palette;

	// position on screen (absolute)
	xpcc::glcd::Point position;

	bool dirty;

};


class WidgetGroup : public Widget {

public:

	WidgetGroup(Dimension d)
	{
		this->dimension = d;
	}

	// packs widgets inside WidgetGroup (relative coordinates!)
	bool
	pack(Widget* w, const xpcc::glcd::Point &coord);

	void
	draw(AbstractView* view);

	void
	setColorPalette(ColorPalette* cb);

	void
	setPosition(const xpcc::glcd::Point& pos);

	bool
	isDirty()
	{
		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
		{
			if((*iter)->isDirty())
				return true;
		}
		return false;
	}

	void
	markDrawn()
	{
		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
		{
			(*iter)->markDrawn();
		}
	}

private:
	WidgetContainer widgets;
};

}
}

#endif /* WIDGET_HPP_ */
