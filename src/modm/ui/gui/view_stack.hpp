/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GUI_VIEWSTACK_HPP
#define MODM_GUI_VIEWSTACK_HPP

#include "../display/graphic_display.hpp"
#include "../../container/stack.hpp"
#include "../../container/linked_list.hpp"
#include "../menu/menu_buttons.hpp"
#include "view.hpp"
#include "types.hpp"

namespace modm
{

namespace gui
{

/**
 * Stack which handles the displaying
 *        of views on the graphic display.
 *
 * This class also deallocates the views passed
 * to the stack.
 *
 * @ingroup	gui
 * @author	Thorsten Lajewski
 */
class GuiViewStack : public modm::ViewStack
{
public:
	GuiViewStack(modm::GraphicDisplay* display, modm::gui::inputQueue* queue);

	virtual
	~GuiViewStack();

	/// Get the top view from the stack
	/// @return	pointer to view from stack
	inline modm::gui::View*
	get()
	{
		return this->stack.get();
	}

	/// Push new view on top of stack the new view
	/// will be displayed instead of the old one.
	inline void
	push(modm::gui::View* view)
	{
		this->stack.push(view);
		this->getDisplay().clear();
		modm::gui::View* top = this->get();
		top->draw();
		this->display->update();
	}

	/// Get event input queue from GuiViewStack
	inline modm::gui::inputQueue*
	getInputQueue()
	{
		return this->input_queue;
	}

	/// Pop remove top view from the stack.
	/// The removed view is deleted.
	void
	pop();

	virtual void
	update();

private:
	modm::Stack< modm::gui::View* , modm::LinkedList< modm::gui::View* > > stack;
	modm::gui::inputQueue *input_queue;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_VIEWSTACK_HPP
