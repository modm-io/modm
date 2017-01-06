/*
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "sdl_display.hpp"

modm::SDLDisplay::SDLDisplay(int16_t width, int16_t height):
	width(width), height(height), memory(0), screen(0)
{
	this->memory = new bool[this->width*this->height];
	for(int i = 0; i< this->width*this->height; i++)
	{
		this->memory[i] = false;
	}
	screen = SDL_SetVideoMode( this->width, this->height, 32, SDL_SWSURFACE );
	this->clear();
}

modm::SDLDisplay::~SDLDisplay()
{
	delete this->memory;
}

void
modm::SDLDisplay::update()
{
	for(int i = 0; i < this->width; i++)
	{
		for(int j = 0; j < this->height; j++)
		{
			if(this->getPixel(i, j))
			{
				this->setPixelOnScreen(i, j, true);
			}
			else{
				this->setPixelOnScreen(i,j, false);
			}
		}
	}
	SDL_Flip( this->screen );
}

void
modm::SDLDisplay::setPixel(int16_t x, int16_t y)
{
	this->memory[y*this->width+x] = true;
}

void
modm::SDLDisplay::clearPixel(int16_t x, int16_t y)
{
	this->memory[y*this->width+x] = false;
}

bool
modm::SDLDisplay::getPixel(int16_t x, int16_t y)
{
	return 	this->memory[y*this->width+x];
}

void
modm::SDLDisplay::setPixelOnScreen(int16_t x, int16_t y, bool value)
{
	Uint32 pixel = SDL_MapRGB( this->screen->format, 0, 0, 0 );
	if(!value)
	{
		pixel = SDL_MapRGB( this->screen->format, 0xff, 0xff, 0xff );
	}
	//Convert the pixels to 32 bit Uint32
	Uint32 *pixels = (Uint32 *)this->screen->pixels;
	//Set the pixel
	pixels[ ( y * this->screen->w ) + x ] = pixel;
}

void
modm::SDLDisplay::clearWholeScreen()
{
	Uint32 pixel = SDL_MapRGB( this->screen->format, 0xFF, 0xFF, 0xFF );
	//Convert the pixels to 32 bit Uint32
	Uint32 *pixels = (Uint32 *)this->screen->pixels;
	for(int i = 0; i < this->width; i++)
	{
		for(int j = 0; j < this->height; j++)
		{
			pixels[ ( j * this->screen->w ) + i ] = pixel;
			this->memory[j*this->width+i] = false;
		}
	}
}
