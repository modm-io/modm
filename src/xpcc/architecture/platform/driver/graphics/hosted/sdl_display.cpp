#include "sdl_display.hpp"

xpcc::SDLDisplay::SDLDisplay(int16_t width, int16_t height):
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

xpcc::SDLDisplay::~SDLDisplay()
{
	delete this->memory;
}

void
xpcc::SDLDisplay::update()
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
xpcc::SDLDisplay::setPixel(int16_t x, int16_t y)
{
	this->memory[y*this->width+x] = true;
}

void
xpcc::SDLDisplay::clearPixel(int16_t x, int16_t y)
{
	this->memory[y*this->width+x] = false;
}

bool
xpcc::SDLDisplay::getPixel(int16_t x, int16_t y)
{
	return 	this->memory[y*this->width+x];
}

void
xpcc::SDLDisplay::setPixelOnScreen(int16_t x, int16_t y, bool value)
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
xpcc::SDLDisplay::clearWholeScreen()
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
