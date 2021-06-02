/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2017-2018, Niklas Hauser
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

#ifndef MODM_ABSTRACT_VIEW_HPP
#define MODM_ABSTRACT_VIEW_HPP

#include <modm/utils/allocator/static.hpp>

#include "iabstract_view.hpp"

namespace modm
{
	// forward declaration
	template<typename T>
	class ViewStack;

	/**
	 * @brief The AbstractView class is the base class for all screens
	 *        handled by the ViewStack class
	 *
	 *\author Thorsten Lajewski
	 *\ingroup modm_ui_menu
	 */

	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	class AbstractView : public IAbstractView
	{
		template<typename T>
		friend class ViewStack;

	public:
		/**
		 * @param stack pointer to the stack, the screen should be displayed on.
		 * @param identifier can be used to determine which screen is the currently
		 *        displayed on the graphicDisplay
		 */
		AbstractView(modm::ViewStack<Allocator>* stack, uint8_t identifier) :
			IAbstractView(identifier), stack(stack)
		{
		}

		virtual ~AbstractView() = default;

		inline modm::ViewStack<Allocator>*
		getViewStack()
		{
			return stack;
		}

		modm::GraphicDisplay&
		display()
		{
			return stack->getDisplay();
		}

	private:
		modm::ViewStack<Allocator>* stack;
	};
}

#include "view_stack.hpp"

#endif // MODM_ABSTRACT_VIEW_HPP
