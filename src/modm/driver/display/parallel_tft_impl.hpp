/*
 * Copyright (c) 2013, Fabian Greif
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PARALLEL_TFT_HPP
#	error	"Don't include this file directly, use 'parallel_tft.hpp' instead!"
#endif

//#define DISP_ORIENTATION					0
//#define DISP_ORIENTATION					90
//#define DISP_ORIENTATION					180
#define DISP_ORIENTATION					270

/* Private define ------------------------------------------------------------*/

#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

#define  MAX_X  320
#define  MAX_Y  240

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

#define  MAX_X  240
#define  MAX_Y  320

#endif

template <typename INTERFACE>
modm::ParallelTft<INTERFACE>::ParallelTft(INTERFACE& interface) :
	interface(interface)
{
}

template <typename INTERFACE>
void
modm::ParallelTft<INTERFACE>::initialize()
{
	// Read LCD ID
	//uint16_t code = interface.readRegister(0x0000);

	// TODO
	deviceCode = Device::SSD1289;

	interface.writeRegister(0x0000, 0x0001);
	/* Enable LCD Oscillator */
	interface.writeRegister(0x0003, 0xA8A4);
	interface.writeRegister(0x000C, 0x0000);
	interface.writeRegister(0x000D, 0x080C);
	interface.writeRegister(0x000E, 0x2B00);
	interface.writeRegister(0x001E, 0x00B0);
	interface.writeRegister(0x0001, 0x2B3F);
	/* 320*240 0x2B3F */
	interface.writeRegister(0x0002, 0x0600);
	interface.writeRegister(0x0010, 0x0000);
	interface.writeRegister(0x0011, 0x6070);
	interface.writeRegister(0x0005, 0x0000);
	interface.writeRegister(0x0006, 0x0000);
	interface.writeRegister(0x0016, 0xEF1C);
	interface.writeRegister(0x0017, 0x0003);
	interface.writeRegister(0x0007, 0x0133);
	interface.writeRegister(0x000B, 0x0000);
	interface.writeRegister(0x000F, 0x0000);
	interface.writeRegister(0x0041, 0x0000);
	interface.writeRegister(0x0042, 0x0000);
	interface.writeRegister(0x0048, 0x0000);
	interface.writeRegister(0x0049, 0x013F);
	interface.writeRegister(0x004A, 0x0000);
	interface.writeRegister(0x004B, 0x0000);
	interface.writeRegister(0x0044, 0xEF00);
	interface.writeRegister(0x0045, 0x0000);
	interface.writeRegister(0x0046, 0x013F);
	interface.writeRegister(0x0030, 0x0707);
	interface.writeRegister(0x0031, 0x0204);
	interface.writeRegister(0x0032, 0x0204);
	interface.writeRegister(0x0033, 0x0502);
	interface.writeRegister(0x0034, 0x0507);
	interface.writeRegister(0x0035, 0x0204);
	interface.writeRegister(0x0036, 0x0204);
	interface.writeRegister(0x0037, 0x0502);
	interface.writeRegister(0x003A, 0x0302);
	interface.writeRegister(0x003B, 0x0302);
	interface.writeRegister(0x0023, 0x0000);
	interface.writeRegister(0x0024, 0x0000);
	interface.writeRegister(0x0025, 0x8000);

	interface.writeRegister(0x004f, 0);
	interface.writeRegister(0x004e, 0);

	interface.writeRegister(0x0011, 0x6068);

	clear();
}

template <typename INTERFACE>
void
modm::ParallelTft<INTERFACE>::clear()
{
	writeCursor(0, 0);

	interface.writeIndex(0x0022);
	for (uint32_t i = 0; i < MAX_X * MAX_Y; i++)
	{
		interface.writeData(backgroundColor.value());
	}
}

template <typename INTERFACE>
void
modm::ParallelTft<INTERFACE>::setPixel(int16_t x, int16_t y)
{
	if (x >= MAX_X || y >= MAX_Y) {
		return;
	}

	writeCursor(x, y);
	interface.writeRegister(0x0022, foregroundColor.value());
}

template <typename INTERFACE>
void
modm::ParallelTft<INTERFACE>::clearPixel(int16_t x, int16_t y)
{
	(void) x;
	(void) y;

//	if (x >= MAX_X || y >= MAX_Y) {
//		return;
//	}
//
//	writeCursor(x, y);
//	interface.writeRegister(0x0022, color.value());
}

template <typename INTERFACE>
modm::color::Rgb565
modm::ParallelTft<INTERFACE>::getPixel(int16_t x, int16_t y) const
{
	(void) x;
	(void) y;

	return modm::color::Rgb565();
}

// ----------------------------------------------------------------------------
template <typename INTERFACE>
void
modm::ParallelTft<INTERFACE>::writeCursor(uint16_t x, uint16_t y)
{

#if ( DISP_ORIENTATION == 90 )

	uint16_t temp;
	y = (MAX_Y - 1) - y;
	temp = y;
	y = x;
	x = temp;

#elif ( DISP_ORIENTATION == 270 )

	uint16_t temp;
	x = (MAX_X -1) - x;
	temp = y;
	y = x;
	x = temp;

#elif ( DISP_ORIENTATION == 0 )


#elif ( DISP_ORIENTATION == 180 )

	y = ( MAX_Y - 1 ) - y;
	x = (MAX_X -1) - x;

#endif


	switch (deviceCode)
	{
	default:
		interface.writeRegister(0x0020, x);
		interface.writeRegister(0x0021, y);
		break;

	case Device::SSD1298:
	case Device::SSD1289:
		interface.writeRegister(0x004e, x);
		interface.writeRegister(0x004f, y);
		break;
	}
}
