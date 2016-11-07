/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
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

#ifndef XPCC__ABSTRACT_VIEW_HPP
#define XPCC__ABSTRACT_VIEW_HPP

#include "../display/graphic_display.hpp"

#include "menu_buttons.hpp"

namespace xpcc
{
	// forward declaration
	class ViewStack;
	
	/**
	 * @brief The AbstractView class is the base class for all screens
	 *        handled by the ViewStack class
	 *
	 *\author Thorsten Lajewski
	 *\ingroup display_menu
	 */

	class AbstractView
	{
		friend class ViewStack;
		
	public:
		/**
		 * @param stack pointer to the stack, the screen should be displayed on.
		 * @param identifier can be used to determine which screen is the currently
		 *        displayed on the graphicDisplay
		 */
		AbstractView(xpcc::ViewStack* stack, uint8_t identifier);
		
		virtual ~AbstractView() = 0;

		/**
		 * @brief update The update function of the top most display gets called
		 *        as often as possible. Only the update of the top view in each
		 *        ViewStack gets called.
		 */
		virtual void
		update();

		/**
		 * @brief hasChanged indicates the current displayed view has changed.
		 *        This function prevents unnecessary drawing of the display
		 * @return if true the display has to be redrawn.
		 */
		virtual bool
		hasChanged() = 0;
		
		/**
		 * @brief draw determine the output on the Graphic Display
		 */
		virtual void
		draw() = 0;
		

		/**
		 * @brief shortButtonPress handle the action for the pressed button
		 */
		virtual void 
		shortButtonPress(xpcc::MenuButtons::Button button);

		/**
		 * @brief isAlive tells the ViewStack if it should remove this screen.
		 * @return
		 */
		bool
		isAlive() const;

		/**
		 * @brief remove the view from the screen. The viewStack handles the deletion.
		 */
		void
		remove();

		/**
		 * @brief getIdentifier of the view.
		 */
		inline uint8_t getIdentifier(){
			return this->identifier;
		}

	public:

		xpcc::GraphicDisplay&
		display();
		
		/**
		 * @brief onRemove will be called right before the view gets deleted,
		 *        can be reimplemented to reset external data.
		 */
		virtual void
		onRemove();
		
		inline xpcc::ViewStack*
		getViewStack()
		{
			return stack;
		}
		
	private:
		xpcc::ViewStack* stack;

	public:
		const uint8_t identifier;
		bool alive;
	};
}

#include "view_stack.hpp"

#endif // XPCC__ABSTRACT_VIEW_HPP
