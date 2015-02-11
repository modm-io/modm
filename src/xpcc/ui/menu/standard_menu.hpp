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

#ifndef XPCC__STANDARD_MENU_HPP
#define XPCC__STANDARD_MENU_HPP

#include "abstract_menu.hpp"
#include "scrollable_text.hpp"
#include "menu_entry_callback.hpp"
#include <xpcc/container/doubly_linked_list.hpp>
#include <xpcc/processing/periodic_timer.hpp>


namespace xpcc
{

	/**
 	* \brief Menu Entry of Standard Menu
	*
	* \ingroup	display_menu
	* \author	Thorsten Lajewski
	*/
	struct MenuEntry
	{
		/**
		 * @brief MenuEntry constructor
		 * @param text, name to be displayed as menu entry
		 * @param space, available to display menu entry in number of letters
		 * @param func  callback, which is called when entry is chosen
		 */
		MenuEntry(const char* text, uint16_t space, MenuEntryCallback func);
		
		ScrollableText text;
		MenuEntryCallback callback;
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
	* \ingroup	display_menu
	* \author	Thorsten Lajewski
	*/

	class StandardMenu : public AbstractMenu
	{
	public:

		StandardMenu(xpcc::ViewStack* stack, uint8_t identifier);
		
		virtual ~StandardMenu() = 0;

		StandardMenu(xpcc::ViewStack* stack, uint8_t identifier, const char* title);

		/**
		 * @brief addEntry adds a new option to the displayed list
		 */
		void
		addEntry(const char* text, MenuEntryCallback func);

		/**
		 * @brief setTitle set the title of the menu displayed on top of the list
		 */
		void
		setTitle(const char* text);

		/**
		 * @brief shortButtonPress handles normal button actions
		 */
		virtual void
		shortButtonPress(xpcc::MenuButtons::Button button);

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
			this->timer.restart(ms);
		}

	private:
		uint16_t display_update_time; //after this time the display is redrawn,
									  // important for scrolling

		xpcc::ShortPeriodicTimer timer;
		bool buttonAction;

		const char* title;

		uint8_t maximalDrawnEntrys;

		uint8_t homePosition; // first entry drawn
		uint8_t position; // selected entry


	protected:

		typedef xpcc::DoublyLinkedList<MenuEntry> EntryList;
		EntryList entries;

	};
}

#endif // XPCC__STANDARD_MENU_HPP
