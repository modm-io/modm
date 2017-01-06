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

#ifndef MODM_VIEWSTACK_HPP
#define MODM_VIEWSTACK_HPP

#include "../display/graphic_display.hpp"
#include "../../container/stack.hpp"
#include "../../container/linked_list.hpp"
#include "menu_buttons.hpp"
#include "abstract_view.hpp"

namespace modm
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
		ViewStack(modm::GraphicDisplay* display);
		
		virtual ~ViewStack();
		
		/**
		 * @brief get the top view from the stack
		 * @return pointer to view from stack
		 */

		inline modm::AbstractView* 
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
		push(modm::AbstractView* view)
		{
			this->stack.push(view);
			this->getDisplay().clear();
			modm::AbstractView* top = this->get();
			top->draw();
			this->display->update();
		}

		/**
		 * @brief getDisplay access underlying GraphicDisplay
		 */
		inline modm::GraphicDisplay&
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
		shortButtonPress(modm::MenuButtons::Button button);

	protected:
		modm::GraphicDisplay* display;
		modm::Stack< modm::AbstractView* , modm::LinkedList< modm::AbstractView* > > stack;
	};
}

#endif // MODM_VIEWSTACK_HPP
