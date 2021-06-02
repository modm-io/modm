/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2015, Niklas Hauser
 * Copyright (c) 2020, Matthew Arnold
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef CHOICE_MENU_HPP
#define CHOICE_MENU_HPP
#include "abstract_menu.hpp"
#include "scrollable_text.hpp"
#include "choice_menu_entry.hpp"
#include <modm/container/doubly_linked_list.hpp>
#include <modm/processing/timer.hpp>

namespace modm{

	 /**
	 * @brief The ChoiceMenu class provides a simmliar list like the Standard Menu.
	 *        In the ChoiceMenu each entry is associated with a boolean variable,
	 *        which can be changed by pressing the OK Button.
	 *
	 *        Controls:
	 *        DOWN - select next entry
	 *        UP  -  select previous entry
	 *        OK - toogle boolean value of selected entry
	 *        LEFT - remove screen from stack
	 *        RIGHT - put next screen on stack
	 *
	 *    \author Thorsten Lajewski
	 *    \ingroup modm_ui_menu
	 *
	 */
	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	class ChoiceMenu : public AbstractMenu<Allocator>
	{
	public:

		ChoiceMenu(modm::ViewStack<Allocator>* stack, uint8_t identifier);

		ChoiceMenu(modm::ViewStack<Allocator>* stack, uint8_t identifier, const char* title);

		/**
		 * @brief addEntry a new entry to the ChoiceMenu
		 * @param text to be displayed in the list
		 * @param valuePtr pointer to the associated boolean variable
		 * @param defaultValue default value of the boolean variable
		 */
		void
		addEntry(const char* text,  bool *valuePtr, bool defaultValue = true);

		/**
		 * @brief initialise the ChoiceMenu, should be called right after contructing the instance of ChoiceMenu
		 * TODO this function should be called in the push function of viewStack automatically
		 */
		virtual void
		initialise();

		/**
		 * @brief setTitle set the title displayed on top of the entryList
		 * @param text
		 */
		void
		setTitle(const char* text);

		/**
		 * @brief shortButtonPress handle the button press actions
		 */
		virtual void
		shortButtonPress(modm::MenuButtons::Button button);

		/**
		 * @brief hasChanged returns true if the screen needs to be drawn again
		 */
		virtual bool
		hasChanged();

		/**
		 * @brief draw the screen
		 */
		virtual void
		draw();

		/**
		 * @brief openNextScreen puts the next screen to be displayed on the stack
		 */
		virtual void
		openNextScreen() = 0;

	private:
		modm::ShortDuration display_update_time;

		modm::ShortPeriodicTimer timer;
		bool buttonAction;

		const char* title;

		uint8_t maximalDrawnEntrys;

		uint8_t homePosition; // first entry drawn
		uint8_t position; // selected entry

	protected:

		typedef modm::DoublyLinkedList<ChoiceMenuEntry> EntryList;
		EntryList entries;
	};
}

#include "choice_menu_impl.hpp"

#endif /* CHOICE_MENU_HPP*/
