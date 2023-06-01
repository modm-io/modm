/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ABSTRACT_MENU_HPP
#define MODM_ABSTRACT_MENU_HPP

#include "view_stack.hpp"
#include "abstract_view.hpp"

namespace modm{

	 /**
	 * @brief The AbstractMenu class is the same as the AbstractView class,
	 *        except it forces the developer to reimplement the handling
	 *        of button presses.
	 *
	 * \author Thorsten Lajewski
	 * \ingroup modm_ui_menu
	 */
	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	class AbstractMenu : public AbstractView<Allocator>
	{
		public:

			AbstractMenu(modm::ViewStack<Allocator>* stack, uint8_t identifier) :
				modm::AbstractView<Allocator>(stack, identifier)
			{
			}

			virtual ~AbstractMenu() {}

			virtual void
			shortButtonPress(modm::MenuButtons::Button button) = 0;

		private:


	};

}

#endif // MODM_ABSTRACT_MENU_HPP
