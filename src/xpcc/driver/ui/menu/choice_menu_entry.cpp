#include "choice_menu_entry.hpp"


xpcc::ChoiceMenuEntry::ChoiceMenuEntry(const char* text, uint16_t space, bool *valuePtr, bool defaultValue):
	text(text, space), valuePtr(valuePtr), defaultValue(defaultValue)
{

}
