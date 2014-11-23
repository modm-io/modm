// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_VIEWSTACK_HPP
#define XPCC_GUI_VIEWSTACK_HPP

#include "../display/graphic_display.hpp"
#include "../../container/stack.hpp"
#include "../../container/linked_list.hpp"
#include "../menu/menu_buttons.hpp"
#include "view.hpp"
#include "types.hpp"

namespace xpcc
{

namespace gui
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

	class GuiViewStack : public xpcc::ViewStack
	{
	public:
		GuiViewStack(xpcc::GraphicDisplay* display, xpcc::gui::inputQueue* queue);

		virtual ~GuiViewStack();

		/**
		 * @brief get the top view from the stack
		 * @return pointer to view from stack
		 */

		inline xpcc::gui::View*
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
		push(xpcc::gui::View* view)
		{
			this->stack.push(view);
			this->getDisplay().clear();
			xpcc::gui::View* top = this->get();
			top->draw();
			this->display->update();
		}

		/**
		 * @brief get event input queue from GuiViewStack
		 */
		inline xpcc::gui::inputQueue*
		getInputQueue()
		{
			return this->input_queue;
		}

		/**
		 * @brief pop remove top view from the stack. The removed
		 *        view is deleted
		 */

		void
		pop();

		virtual void
		update();

	private:
		xpcc::Stack< xpcc::gui::View* , xpcc::LinkedList< xpcc::gui::View* > > stack;
		xpcc::gui::inputQueue *input_queue;
	};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_VIEWSTACK_HPP
