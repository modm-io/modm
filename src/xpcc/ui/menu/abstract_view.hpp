// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

	protected:

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
		const uint8_t identifier;
		bool alive;
	};
}

#include "view_stack.hpp"

#endif // XPCC__ABSTRACT_VIEW_HPP
