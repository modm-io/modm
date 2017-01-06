/*
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "standard_menu.hpp"

modm::MenuEntry::MenuEntry(const char* text, uint16_t space, MenuEntryCallback func):
	text(text, space),
	callback(func)
{
}

modm::StandardMenu::StandardMenu(modm::ViewStack* stack, uint8_t identifier) :
	modm::AbstractMenu(stack, identifier),
	display_update_time(500),
	timer(display_update_time),
	buttonAction(false),
	title(""),
	homePosition(0),
	position(0)
{
	this->maximalDrawnEntrys = (getViewStack()->getDisplay().getHeight()- 16) / 8 ;
}

modm::StandardMenu::StandardMenu(modm::ViewStack* stack, uint8_t identifier, const char* title) :
	modm::AbstractMenu(stack, identifier),
	display_update_time(500),
	timer(display_update_time),
	buttonAction(false),
	title(title),
	homePosition(0),
	position(0)
{
	this->maximalDrawnEntrys = (getViewStack()->getDisplay().getHeight()- 16) / 8 ;
}

modm::StandardMenu::~StandardMenu()
{
}

void
modm::StandardMenu::addEntry(const char* text, MenuEntryCallback func)
{
	modm::MenuEntry entry(text, (getViewStack()->getDisplay().getWidth()-16)/6, func);
	this->entries.append(entry);
}

void
modm::StandardMenu::setTitle(const char* text)
{
	this->title = text;
}


void
modm::StandardMenu::draw()
{
	modm::GraphicDisplay* display = &getViewStack()->getDisplay();
	display->clear();
	display->setCursor(0,2);
	(*display) << this->title;
	display->drawLine(0, 10, display->getWidth(), 10);

	uint8_t i, count = this->entries.getSize();
	EntryList::iterator iter = this->entries.begin();

	for(uint8_t j=0; j<this->homePosition; ++j)
	{
		++iter;
	}

	for (i = 0; i < this->maximalDrawnEntrys; i++)
	{
		if (this->homePosition + i >= count)
			break;

		display->setCursor(4, 12+i*8);
		if (this->position - this->homePosition == i) {
				(*display) << ">"; // TODO add nicer symbol
		}
		else {
				(*display) << " ";
		}
		
		(*display) << iter->text.getText();
		++iter;
	}

	// todo add file up and / or down if some entrys are not displayed on screen
}

bool
modm::StandardMenu::hasChanged()
{
	if (timer.execute() || this->buttonAction)
	{
		if (this->buttonAction) this->buttonAction=false;

		return true;
	}
	else
	{
		return false;
	}
}

void
modm::StandardMenu::selectedEntryFunction(uint8_t /*selected*/)
{

}

void
modm::StandardMenu::shortButtonPress(modm::MenuButtons::Button button)
{
	switch(button)
	{
		case modm::MenuButtons::DOWN:
		{
			if (this->position + 1U < this->entries.getSize())
			{
				EntryList::iterator iter = this->entries.begin();

				for (uint8_t j=0; j<this->position;  ++j)
				{
					++iter;
				}
				iter->text.toogle();
				iter->text.setToStart();

				if (this->position - this->homePosition == this->maximalDrawnEntrys - 1)
				{
					this->homePosition += 1;
					this->position += 1;
					++iter;
					iter->text.toogle();
				}
				else
				{
					this->position += 1;
					++iter;
					iter->text.toogle();
				}
				this->selectedEntryFunction(this->position);
				this->buttonAction=true;
			}
			break;
		}
		case modm::MenuButtons::UP:
		{
			if (this->position > 0)
			{
				EntryList::iterator iter = this->entries.begin();

				for (uint8_t j = 0; j < this->position; ++j)
				{
					++iter;
				}

				iter->text.toogle();
				iter->text.setToStart();

				if (this->position == this->homePosition)
				{
					this->homePosition -= 1;
					this->position -= 1;
					--iter;
					iter->text.toogle();
				}
				else
				{
					this->position -= 1;
					--iter;
					iter->text.toogle();
				}
				this->selectedEntryFunction(this->position);
				this->buttonAction = true;
			}
			break;
		}
		case modm::MenuButtons::LEFT:
		{
			this->remove();
			break;
		}
		case modm::MenuButtons::OK:
		{
			break;
		}
		case modm::MenuButtons::RIGHT:
		{
			EntryList::iterator iter = this->entries.begin();

			for (uint8_t j = 0; j < this->position; ++j)
			{
				++iter;
			}

			iter->callback.call();
			break;
		}
	}
}
