/*
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

#include "scrollable_text.hpp"

#include <string.h>

modm::ScrollableText::ScrollableText(const char* text, uint16_t space):
paused(true), text(text), startPosition(0), length(strlen(text)), space(space)
{
	this->print = new char[space+1];
	if(this->needsScrolling())
	{
		for(uint8_t i=0; i<space; ++i)
		{
			this->print[i]=this->text[i];
		}
		this->print[space]='\0';
	}
}

modm::ScrollableText::~ScrollableText()
{
	delete [] this->print;
}

modm::ScrollableText::ScrollableText(const ScrollableText& text)
{
	this->paused = true;
	this->space = text.space;
	this->startPosition = 0;
	this->paused = text.paused;
	this->text = text.text;
	this->length= text.length;
	this->print = new char[this->space+1];

	if(this->needsScrolling())
	{
		for(uint8_t i=0; i<space; ++i)
		{
			this->print[i]=this->text[i];
		}
		this->print[space]='\0';
	}
}

modm::ScrollableText&
modm::ScrollableText::operator=(const ScrollableText& text)
{
	if(&text == this) return *this;

	delete [] this->print;
	this->space = text.space;
	this->startPosition = text.startPosition;
	this->paused = text.paused;
	this->text = text.text;
	this->length= text.length;
	this->print = new char[this->space+1];

	if(this->needsScrolling())
	{
		for(uint8_t i=0; i<space; ++i)
		{
			this->print[i]=this->text[i];
		}
		this->print[space]='\0';
	}

	return *this;
}

const char*
modm::ScrollableText::getText()
{
	if(this->needsScrolling())
	{
		if(!this->isPaused())
		{
			for(uint16_t i = 0; i<this->space; ++i)
			{
				if( (i+this->startPosition) < this->length)
					this->print[i] = this->text[i+this->startPosition];
				else if( i+this->startPosition == this->length)
				{
					this->print[i] =' ';
				}
				else
				{
					this->print[i] = this->text[i+ this->startPosition - (this->length+1)];
				}
			}
			this->print[space] = '\0';
			++this->startPosition;
			if(startPosition > this->length)
			{
				this->startPosition = 0;
			}
		}
		return this->print;
	}
	else
	{
		return this->text;
	}
}


void
modm::ScrollableText::setToStart()
{
	this->startPosition = 0;
	if(this->needsScrolling())
	{
		for(uint8_t i=0; i<space; ++i)
		{
			this->print[i]=this->text[i];
		}
		this->print[space]='\0';
	}
}
