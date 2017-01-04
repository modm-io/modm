/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_VIEWSTACK_HPP
#define XPCC_VIEWSTACK_HPP

#include "../display/graphic_display.hpp"
#include "../../container/stack.hpp"
#include "../../container/linked_list.hpp"
#include "menu_buttons.hpp"
#include "abstract_view.hpp"

namespace xpcc
{
	/**
	* \brief Stack which handles the displaying
	*        of views on the graphic display.
	*
	* This class also deallocates the views passed
	* to the stack.
	*
	* \ingroup	display_menu
	* \author	Thorsten Lajewski
	*/

	class ViewStack
	{
	public:
		ViewStack(xpcc::GraphicDisplay* display);
		
		virtual ~ViewStack();
		
		/**
		 * @brief get the top view from the stack
		 * @return pointer to view from stack
		 */

		inline xpcc::AbstractView* 
		get()
		{
			return this->stack.get();
		}
		
		/**
		 * @brief push new view on top of stack the new
		 *        view will be displayed instead of the old
		 *        one
		 *
		 * @param view next displayed view
		 */
		inline void
		push(xpcc::AbstractView* view)
		{
			this->stack.push(view);
			this->getDisplay().clear();
			xpcc::AbstractView* top = this->get();
			top->draw();
			this->display->update();
		}

		/**
		 * @brief getDisplay access underlying GraphicDisplay
		 */
		inline xpcc::GraphicDisplay&
		getDisplay()
		{
			return *this->display;
		}
		
		/**
		 * @brief pop remove top view from the stack. The removed
		 *        view is deleted
		 */

		void
		pop();
		
		virtual void
		update();

		/**
		 * @brief shortButtonPress pass the button press to the current top view
		 * @param button the pressed button
		 */
		
		void
		shortButtonPress(xpcc::MenuButtons::Button button);

	protected:
		xpcc::GraphicDisplay* display;
		xpcc::Stack< xpcc::AbstractView* , xpcc::LinkedList< xpcc::AbstractView* > > stack;
	};
}

#endif // XPCC_VIEWSTACK_HPP
