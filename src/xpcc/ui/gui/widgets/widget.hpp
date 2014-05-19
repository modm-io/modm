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

	Widget(Dimension dimension, bool is_interactive) :
		dimension(dimension),
		activated(false),
		cb_activate(NULL),
		cb_deactivate(NULL),
		color_palette(NULL),
		position(xpcc::glcd::Point(0,0)),
		dirty(true),
		is_interactive(is_interactive),
		font(xpcc::accessor::asFlash(xpcc::font::FixedWidth5x8))
	{
		// assign unique id
		uid = uid_global++;
	}

	virtual
	~Widget() {}

	virtual void
	render(AbstractView* view) = 0;

	void
	draw(AbstractView *view)
	{

		// render widget on screen
		this->render(view);
		this->markDrawn();

		// if there are widgets on top, redraw them
		if(this->hasIntersections()) {
			for(auto iter = this->intersecting_widgets.begin(); iter != this->intersecting_widgets.end(); ++iter)
			{
				(*iter)->draw(view);
			}
		}
	}

	bool
	checkIntersection(Widget* w);

	void
	updateIntersections(WidgetContainer *widgets);

	bool
	hasIntersections();

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

	inline xpcc::glcd::Point
	getPosition()
	{
		return this->position;
	}

	inline xpcc::gui::Dimension
	getDimension()
	{
		return this->dimension;
	}

	inline uint16_t
	getWidth()
	{
		return this->dimension.width;
	}

	inline uint16_t
	getHeight()
	{
		return this->dimension.height;
	}

	virtual bool
	isDirty()
	{
		return this->dirty;
	}

	bool
	isInteractive()
	{
		return this->is_interactive;
	}

	virtual void
	markDrawn()
	{
		this->dirty = false;
	}

	virtual void
	setFont(const uint8_t *newFont)
	{
		this->font = xpcc::accessor::asFlash(newFont);
	}

	virtual void
	setFont(const xpcc::accessor::Flash<uint8_t> *font)
	{
		this->font = *font;
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

	// has changes to be drawed
	bool dirty;

	// whether widget will receive events
	bool is_interactive;

	// widget specific font
	xpcc::accessor::Flash<uint8_t> font;

	// list of widgets that intersect with this widget
	WidgetContainer intersecting_widgets;

};


class WidgetGroup : public Widget {

public:

	WidgetGroup(Dimension d) :
		Widget(d, true),
		widgets()
	{
	}

	// packs widgets inside WidgetGroup (relative coordinates!)
	bool
	pack(Widget* w, const xpcc::glcd::Point &coord);

	void
	render(AbstractView* view);

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
