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

#include "scrollable_text.hpp"

#include <string.h>

xpcc::ScrollableText::ScrollableText(const char* text, uint16_t space):
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

xpcc::ScrollableText::~ScrollableText()
{
	delete [] this->print;
}

xpcc::ScrollableText::ScrollableText(const ScrollableText& text)
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

xpcc::ScrollableText&
xpcc::ScrollableText::operator=(const ScrollableText& text)
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
xpcc::ScrollableText::getText()
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
xpcc::ScrollableText::setToStart()
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
