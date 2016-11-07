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

#include "choice_menu.hpp"


xpcc::ChoiceMenu::ChoiceMenu(xpcc::ViewStack* stack, uint8_t identifier) :
	xpcc::AbstractMenu(stack, identifier),
	display_update_time(500),
	timer(display_update_time),
	buttonAction(false),
	title(""),
	homePosition(0),
	position(0)
{
	this->maximalDrawnEntrys = (getViewStack()->getDisplay().getHeight()- 16) / 8 ;
}

xpcc::ChoiceMenu::ChoiceMenu(xpcc::ViewStack* stack, uint8_t identifier, const char* title) :
	xpcc::AbstractMenu(stack, identifier),
	display_update_time(500),
	timer(display_update_time),
	buttonAction(false),
	title(title),
	homePosition(0),
	position(0)
{
	this->maximalDrawnEntrys = (getViewStack()->getDisplay().getHeight()- 16) / 8 ;
}

void
xpcc::ChoiceMenu::addEntry(const char* text,  bool *valuePtr, bool defaultValue)
{
	static uint16_t availableSpace = (getViewStack()->getDisplay().getWidth()-16)/6-6;
	xpcc::ChoiceMenuEntry entry(text, availableSpace, valuePtr, defaultValue);
	this->entries.append(entry);
}

void
xpcc::ChoiceMenu::initialise()
{
	EntryList::iterator iter = this->entries.begin();
	for(; iter!= this->entries.end(); ++iter){
		(*iter->valuePtr) = iter->defaultValue;
	}
	this->position = 0;
	this->homePosition = 0;
	if (this->entries.begin()->text.needsScrolling())
	{

		this->entries.begin()->text.toogle();
	}
}

void
xpcc::ChoiceMenu::setTitle(const char* text)
{
	this->title = text;
}


void
xpcc::ChoiceMenu::draw()
{
	xpcc::GraphicDisplay* display = &getViewStack()->getDisplay();
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
				(*display) << ">"; // TODO schönes Zeichen nehmen
		}
		else {
				(*display) << " ";
		}

		(*display) << iter->text.getText();
		display->setCursor(display->getWidth()- 8 - 5*6,12+i*8);
		if(*(iter->valuePtr) == true)
		{
			(*display) << "TRUE";
		}
		else{
			(*display) << "FALSE";
		}
		//(*display) << iter->text;
		++iter;
	}

	// TODO wenn möglich pfeil nach oben und nach unten einfügen
}

bool
xpcc::ChoiceMenu::hasChanged()
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
xpcc::ChoiceMenu::shortButtonPress(xpcc::MenuButtons::Button button)
{
	switch(button)
	{
		case xpcc::MenuButtons::DOWN:
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
				this->buttonAction=true;
			}
			break;
		}
		case xpcc::MenuButtons::UP:
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
				this->buttonAction = true;
			}
			break;
		}
		case xpcc::MenuButtons::LEFT:
		{
			this->remove();
			break;
		}
		case xpcc::MenuButtons::OK:
		{
			EntryList::iterator iter = this->entries.begin();

			for (uint8_t j = 0; j < this->position; ++j)
			{
				++iter;
			}

			*(iter->valuePtr) = !(*iter->valuePtr);
			this->buttonAction = true;
			break;
		}
		case xpcc::MenuButtons::RIGHT:
		{
			this->openNextScreen();
		}
	}
}
