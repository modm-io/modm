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
