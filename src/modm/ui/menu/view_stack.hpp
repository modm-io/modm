/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, 2017-2018, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2020, Matthew Arnold
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

#include <modm/ui/display/graphic_display.hpp>
#include <modm/container/stack.hpp>
#include <modm/container/linked_list.hpp>
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
	* \ingroup	modm_ui_menu
	* \author	Thorsten Lajewski
	*/

	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	class ViewStack
	{
	public:
		ViewStack(modm::GraphicDisplay* display, const Allocator allocator = Allocator()) :
			display(display),
			allocator(allocator)
		{
		}

		virtual ~ViewStack()
		{
		}

		/**
		 * @brief get the top view from the stack
		 * @return pointer to view from stack
		 */

		inline modm::AbstractView<Allocator>*
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
		push(modm::AbstractView<Allocator>* view)
		{
			this->stack.push(view);
			this->getDisplay().clear();
			modm::AbstractView<Allocator>* top = this->get();
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
		pop()
		{
			modm::AbstractView<Allocator> *topElement = this->stack.get();
			this->stack.pop();

			allocator.destroy(topElement);
			allocator.deallocate(topElement);
		}

		virtual void
		update()
		{
			modm::AbstractView<Allocator>* top = this->get();

			if (top == NULL)
				return;

			top->update();
			if (top->isAlive())
			{
				if (top->hasChanged())
				{
					top->draw();
					this->display->update();
				}
			}
			else
			{
				// Remove old view
				top->onRemove();
				this->pop();

				// Get new screen
				top = this->get();
				top->update();
				this->display->clear();
				top->draw();
				this->display->update();
			}
		}

		/**
		 * @brief shortButtonPress pass the button press to the current top view
		 * @param button the pressed button
		 */

		void
		shortButtonPress(modm::MenuButtons::Button button)
		{
			modm::AbstractView<Allocator>* top = this->get();
			top->shortButtonPress(button);
		}

	protected:
		modm::GraphicDisplay* display;
		modm::Stack< modm::AbstractView<Allocator>* , modm::LinkedList< modm::AbstractView<Allocator>* > > stack;
		Allocator allocator;
	};
}

#endif // MODM_VIEWSTACK_HPP
