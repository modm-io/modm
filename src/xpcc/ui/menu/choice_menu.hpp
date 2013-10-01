#ifndef CHOICE_MENU_HPP
#define CHOICE_MENU_HPP
#include "abstract_menu.hpp"
#include "scrollable_text.hpp"
#include "choice_menu_entry.hpp"
#include <xpcc/container/doubly_linked_list.hpp>
#include <xpcc/processing/periodic_timer.hpp>

namespace xpcc{

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
	 *    \ingroup display_menu
	 *
	 */
	class  ChoiceMenu: public AbstractMenu
	{
	public:

		ChoiceMenu(xpcc::ViewStack* stack, uint8_t identifier);

		ChoiceMenu(xpcc::ViewStack* stack, uint8_t identifier, const char* title);

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
		shortButtonPress(xpcc::MenuButtons::Button button);

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
		uint16_t display_update_time;

		xpcc::PeriodicTimer<> timer;
		bool buttonAction;

		const char* title;

		uint8_t maximalDrawnEntrys;

		uint8_t homePosition; // first entry drawn
		uint8_t position; // selected entry

	protected:

		typedef xpcc::DoublyLinkedList<ChoiceMenuEntry> EntryList;
		EntryList entries;
	};
}

#endif /* CHOICE_MENU_HPP*/
