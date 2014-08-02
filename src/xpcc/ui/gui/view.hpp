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

#ifndef XPCC__GUI_ABSTRACT_VIEW_HPP
#define XPCC__GUI_ABSTRACT_VIEW_HPP

#include "../display/graphic_display.hpp"

#include "types.hpp"
#include "widgets/widget.hpp"
#include "colorpalette.hpp"

#include "../menu/abstract_view.hpp"

namespace xpcc
{
namespace gui
{
	// forward declaration
	class GuiViewStack;
	
	/**
	 * @brief The View class is the base class for all screens
	 *        handled by the ViewStack class
	 *
	 *\author Thorsten Lajewski
	 *\ingroup display_menu
	 */

	class View : public xpcc::AbstractView
	{
		friend class GuiViewStack;
		
	public:
		/**
		 * @param stack pointer to the stack, the screen should be displayed on.
		 * @param identifier can be used to determine which screen is the currently
		 *        displayed on the graphicDisplay
		 */
		View(xpcc::gui::GuiViewStack* stack, uint8_t identifier, xpcc::gui::Dimension dimension);
		
		virtual ~View() = 0;

		/**
		 * @brief May be called as often as possible. Handles input events
		 *        located in the parent GuiViewStack
		 */
		virtual void
		update();

		virtual void
		preUpdate()
		{
		}

		virtual void
		postUpdate()
		{
		}
		
		/**
		 * @brief draw determine the output on the Graphic Display
		 */
		virtual void
		draw();
		
		/**
		 *  @brief add widget to view
		 */
		bool
		pack(Widget *w, const xpcc::glcd::Point &coord);

		/**
		 * @brief remove the view from the screen. The viewStack handles the deletion.
		 */
		void
		remove();

		/**
		 * @brief set color palette for every contained widget
		 */
		void
		setColorPalette(ColorPalette& cp);

		ColorPalette&
		getColorPalette()
		{
			return this->colorpalette;
		}

		void
		markDirty();

		void
		markDrawn();
		
		inline xpcc::gui::GuiViewStack*
		getViewStack()
		{
			return stack;
		}

	protected:
		xpcc::gui::GuiViewStack* stack;
		Dimension dimension;
		WidgetContainer widgets;
		xpcc::gui::ColorPalette colorpalette;
	};
}
}

#endif // XPCC__GUI_ABSTRACT_VIEW_HPP
