#include "sdl_display.hpp"

xpcc::SDLDisplay::SDLDisplay(int16_t width, int16_t height):
	width(width), height(height), memory(0), screen(0)
{
	this->memory = new uint16_t[this->width*this->height];
	for(int i = 0; i< this->width*this->height; i++)
	{
		this->memory[i] = xpcc::glcd::Color::red().getValue();
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
			this->setColor(xpcc::glcd::Color(this->memory[j*this->width+i]));
			this->setPixelOnScreen(i, j);
		}
	}
	SDL_Flip( this->screen );
}

void
xpcc::SDLDisplay::setPixel(int16_t x, int16_t y)
{
	this->memory[y*this->width+x] = this->foregroundColor.getValue();
}

void
xpcc::SDLDisplay::clearPixel(int16_t x, int16_t y)
{
	this->memory[y*this->width+x] = this->backgroundColor.getValue();
}

bool
xpcc::SDLDisplay::getPixel(int16_t x, int16_t y)
{
	//return this->memory[y*this->width+x];
	return false;
}

void
xpcc::SDLDisplay::setPixelOnScreen(int16_t x, int16_t y)
{
	//XPCC_LOG_DEBUG << "setPixelOnScreen(" << x << ", " << y << ");" << xpcc::endl;
	//XPCC_LOG_DEBUG << "  color: " << this->foregroundColor.getValue() << xpcc::endl;
	//XPCC_LOG_DEBUG << "red: " << colorToRed(this->foregroundColor) << " green: " << colorToGreen(this->foregroundColor) << " blue: " << colorToBlue(this->foregroundColor) << xpcc::endl;

	Uint32 pixel = SDL_MapRGB( this->screen->format, colorToRed(this->foregroundColor), colorToGreen(this->foregroundColor), colorToBlue(this->foregroundColor) );

	//Convert the pixels to 32 bit Uint32
	Uint32 *pixels = (Uint32 *)this->screen->pixels;
	//Set the pixel
	pixels[ ( y * this->screen->w ) + x ] = pixel;
}

void
xpcc::SDLDisplay::clearWholeScreen()
{

	Uint32 pixel = SDL_MapRGB( this->screen->format, colorToRed(this->backgroundColor), colorToGreen(this->backgroundColor), colorToBlue(this->backgroundColor) );
	//Convert the pixels to 32 bit Uint32
	Uint32 *pixels = (Uint32 *)this->screen->pixels;
	for(int i = 0; i < this->width; i++)
	{
		for(int j = 0; j < this->height; j++)
		{
			pixels[ ( j * this->screen->w ) + i ] = pixel;
			this->memory[j*this->width+i] = this->backgroundColor.getValue();
		}
	}
}

inline uint8_t xpcc::SDLDisplay::colorToRed(xpcc::glcd::Color& color)
{
	return color.getValue() >> 8;
}

inline uint8_t xpcc::SDLDisplay::colorToGreen(xpcc::glcd::Color& color)
{
	return static_cast<uint8_t>((color.getValue() << 5) >> 8);
}

inline uint8_t xpcc::SDLDisplay::colorToBlue(xpcc::glcd::Color& color)
{
	return static_cast<uint8_t>((color.getValue() << 11) >> 8);
}
