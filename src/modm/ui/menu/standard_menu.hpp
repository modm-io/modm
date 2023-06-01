/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2011, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
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

#ifndef MODM_STANDARD_MENU_HPP
#define MODM_STANDARD_MENU_HPP

#include "abstract_menu.hpp"
#include "scrollable_text.hpp"
#include "menu_entry_callback.hpp"
#include <modm/container/doubly_linked_list.hpp>
#include <modm/processing/timer.hpp>


namespace modm
{

	/**
 	* \brief Menu Entry of Standard Menu
	*
	* \ingroup	modm_ui_menu
	* \author	Thorsten Lajewski
	*/
	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	struct MenuEntry
	{
		/**
		 * @brief MenuEntry constructor
		 * @param text, name to be displayed as menu entry
		 * @param space, available to display menu entry in number of letters
		 * @param func  callback, which is called when entry is chosen
		 */
		MenuEntry(const char* text, uint16_t space, MenuEntryCallback<Allocator> func);

		ScrollableText text;
		MenuEntryCallback<Allocator> callback;
	};

	/**
	* \brief StandardMenu provides a vertical list of entry.
	*
	*Controls:
	*  Down - next Entry
	*  Up  - previous Entry
	*  Left - remove Standard Menu from stack
	*  Right - select chosen entry.
	*
	* \ingroup	modm_ui_menu
	* \author	Thorsten Lajewski
	*/

	template<typename Allocator = allocator::Dynamic<IAbstractView> >
	class StandardMenu : public AbstractMenu<Allocator>
	{
	public:

		StandardMenu(modm::ViewStack<Allocator>* stack, uint8_t identifier);

		virtual ~StandardMenu() {}

		StandardMenu(modm::ViewStack<Allocator>* stack, uint8_t identifier, const char* title);

		/**
		 * @brief addEntry adds a new option to the displayed list
		 */
		void
		addEntry(const char* text, MenuEntryCallback<Allocator> func);

		/**
		 * @brief setTitle set the title of the menu displayed on top of the list
		 */
		void
		setTitle(const char* text);

		/**
		 * @brief shortButtonPress handles normal button actions
		 */
		virtual void
		shortButtonPress(modm::MenuButtons::Button button);

		/**
		 * @brief hasChanged, returns true if the screen has to be redrawn
		 */
		virtual bool
		hasChanged();

		/**
		 * @brief draw the screen
		 */
		virtual void
		draw();

		/**
		 * @brief selectedEntryFunction can be reimplemented to allow side effects each time one Entry
		 *        selected. This function always gets called after Button UP or Button DOWN was pressed.
		 * @param selected
		 */
		virtual void
		selectedEntryFunction(uint8_t selected);

		/**
		 * @brief setUpdateTime changes the time for redrawing the screen, this is important for scrolling texts, because it determines the scroll speed
		 * @param ms
		 */
		inline void setUpdateTime(uint16_t ms)
		{
			this->display_update_time = ms;
			this->timer.restart(std::chrono::milliseconds(ms));
		}

	private:
		uint16_t display_update_time; //after this time the display is redrawn,
									  // important for scrolling

		modm::ShortPeriodicTimer timer;
		bool buttonAction;

		const char* title;

		uint8_t maximalDrawnEntrys;

		uint8_t homePosition; // first entry drawn
		uint8_t position; // selected entry


	protected:

		template<typename T>
		using EntryList = modm::DoublyLinkedList<MenuEntry<T> >;

		EntryList<Allocator> entries;

	};
}

#include "standard_menu_impl.hpp"

#endif // MODM_STANDARD_MENU_HPP
