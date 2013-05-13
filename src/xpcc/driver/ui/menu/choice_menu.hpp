#ifndef CHOICE_MENU_HPP
#define CHOICE_MENU_HPP
#include "abstract_menu.hpp"
#include "scrollable_text.hpp"
#include "choice_menu_entry.hpp"
#include <xpcc/container/doubly_linked_list.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

#define DISPLAY_UPDATE_TIME 500 //after this time the display is redrawn

namespace xpcc{
	class  ChoiceMenu: public AbstractMenu
	{
	public:

		ChoiceMenu(xpcc::ViewStack* stack, uint8_t identifier);

		virtual ~ChoiceMenu() = 0;

		ChoiceMenu(xpcc::ViewStack* stack, uint8_t identifier, const char* title);

		void
		addEntry(const char* text,  bool *valuePtr, bool defaultValue = true);

		virtual void
		initialise();

		void
		setTitle(const char* text);

		virtual void
		shortButtonPress(xpcc::MenuButtons::Button button);

		virtual bool
		hasChanged();

		virtual void
		draw();

		virtual void
		openNextScreen() = 0;

	private:

		xpcc::PeriodicTimer<> timer;
		bool buttonAction;
		bool newOnStack;

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
