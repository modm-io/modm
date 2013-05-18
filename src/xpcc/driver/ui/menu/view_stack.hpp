// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC__VIEWSTACK_HPP
#define XPCC__VIEWSTACK_HPP

#include "../display/graphic_display.hpp"
#include "../../../container/stack.hpp"
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

	private:
		xpcc::GraphicDisplay* display;
		xpcc::Stack< xpcc::AbstractView* , xpcc::LinkedList< xpcc::AbstractView* > > stack;
	};
}

#endif // XPCC__VIEWSTACK_HPP
