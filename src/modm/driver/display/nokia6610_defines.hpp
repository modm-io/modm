/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NOKIA6610_DEFINES_HPP
#define MODM_NOKIA6610_DEFINES_HPP

// ----------------------------------------------------------------------------
// Defines for the NOKIA6610 display controller.
// ----------------------------------------------------------------------------

namespace modm{
	namespace nokia {
		enum NokiaGE12 {
			NOKIA_GE12_NOP = 0x00, // nop
			NOKIA_GE12_SOFTRST = 0x01, // software reset
			NOKIA_GE12_BOOSTVOFF = 0x02, // booster voltage OFF
			NOKIA_GE12_BOOSTVON = 0x03, // booster voltage ON
			NOKIA_GE12_TESTMODE1 = 0x04, // test mode
			NOKIA_GE12_DISPSTATUS = 0x09, // display status
			NOKIA_GE12_SLEEPIN = 0x10, // sleep in
			NOKIA_GE12_SLEEPOUT = 0x11, // sleep out
			NOKIA_GE12_PARTIAL = 0x12, // partial display mode
			NOKIA_GE12_NORMALMODE = 0x13, // display normal mode
			NOKIA_GE12_INVERSIONOFF = 0x20, // inversion OFF
			NOKIA_GE12_INVERSIONON = 0x21, // inversion ON
			NOKIA_GE12_ALLPIXELOFF = 0x22, // all pixel OFF
			NOKIA_GE12_ALLPIXELON = 0x23, // all pixel ON
			NOKIA_GE12_CONTRAST = 0x25, // write contrast
			NOKIA_GE12_DISPLAYOFF = 0x28, // display OFF
			NOKIA_GE12_DISPLAYON = 0x29,// display ON
			NOKIA_GE12_COLADDRSET = 0x2A, // column address set
			NOKIA_GE12_PAGEADDRSET = 0x2B, // page address set
			NOKIA_GE12_MEMWRITE = 0x2C, // memory write
			NOKIA_GE12_COLORSET = 0x2D, // colour set
			NOKIA_GE12_READRAMDATA = 0x2E, // RAM data read
			NOKIA_GE12_PARTIALAREA = 0x30, // partial area
			NOKIA_GE12_VERTSCROLL = 0x33, // vertical scrolling definition
			NOKIA_GE12_TESTMODE2 = 0x34,// test mode
			NOKIA_GE12_TESTMODE3 = 0x35, // test mode
			NOKIA_GE12_ACCESSCTRL = 0x36, // memory access control
			NOKIA_GE12_VSCRLSADDR = 0x37, // vertical scrolling start address
			NOKIA_GE12_IDLEOFF = 0x38, // idle mode OFF
			NOKIA_GE12_IDLEON = 0x39, // idle mode ON
			NOKIA_GE12_PIXELFORMAT = 0x3A, // interface pixel format
			NOKIA_GE12_TESTMODE4 = 0xDE, // test mode
			NOKIA_GE12_NOP2 = 0xAA, // nop
			NOKIA_GE12_INITESC = 0xC6, // initial escape
			NOKIA_GE12_TESTMODE5 = 0xDA, // test mode
			NOKIA_GE12_TESTMODE6 = 0xDB, // test mode
			NOKIA_GE12_TESTMODE7 = 0xDC, // test mode
			NOKIA_GE12_TESTMODE8 = 0xB2, // test mode
			NOKIA_GE12_GRAYSCALE0 = 0xB3, // gray scale position set 0
			NOKIA_GE12_GRAYSCALE1 = 0xB4, // gray scale position set 1
			NOKIA_GE12_GAMMA = 0xB5, // gamma curve set
			NOKIA_GE12_DISPCTRL = 0xB6, // display control
			NOKIA_GE12_TEMPGRADIENT = 0xB7, // temp gradient set
			NOKIA_GE12_TESTMODE9 = 0xB8, // test mode
			NOKIA_GE12_REFSET = 0xB9, // refresh set
			NOKIA_GE12_VOLTCTRL = 0xBA, // voltage control
			NOKIA_GE12_COMMONDRV = 0xBD, // common driver output select
			NOKIA_GE12_PWRCTRL = 0xBE, // power control
		};

		enum NokiaGE8 {
			NOKIA_GE8_DISON = 0xAF, // Display on
			NOKIA_GE8_DISOFF = 0xAE, // Display off
			NOKIA_GE8_DISNOR = 0xA6, // Normal display
			NOKIA_GE8_DISINV = 0xA7, // Inverse display
			NOKIA_GE8_COMSCN = 0xBB, // Common scan direction
			NOKIA_GE8_DISCTL = 0xCA, // Display control
			//NOKIA_GE8_DISCTL = 0xBA, // Display control
			NOKIA_GE8_SLPIN = 0x95, // Sleep in
			NOKIA_GE8_SLPOUT = 0x94, // Sleep out
			NOKIA_GE8_PASET = 0x75, // Page address set
			NOKIA_GE8_CASET = 0x15, // Column address set
			NOKIA_GE8_DATCTL = 0xBC, // Data scan direction, etc.
			NOKIA_GE8_RGBSET8 = 0xCE, // 256-color position set
			NOKIA_GE8_RAMWR = 0x5C, // Writing to memory
			NOKIA_GE8_RAMRD = 0x5D, // Reading from memory
			NOKIA_GE8_PTLIN = 0xA8, // Partial display in
			NOKIA_GE8_PTLOUT = 0xA9, // Partial display out
			NOKIA_GE8_RMWIN = 0xE0, // Read and modify write
			NOKIA_GE8_RMWOUT = 0xEE, // End
			NOKIA_GE8_ASCSET = 0xAA, // Area scroll set
			NOKIA_GE8_SCSTART = 0xAB, // Scroll start set
			NOKIA_GE8_OSCON = 0xD1, // Internal oscillation on
			NOKIA_GE8_OSCOFF = 0xD2, // Internal oscillation off
			NOKIA_GE8_PWRCTR = 0x20, // Power control
			NOKIA_GE8_VOLCTR = 0x81, // Electronic volume control
			NOKIA_GE8_VOLUP = 0xD6, // Increment electronic control by 1
			NOKIA_GE8_VOLDOWN = 0xD7, // Decrement electronic control by 1
			NOKIA_GE8_TMPGRD = 0x82, // Temperature gradient set
			NOKIA_GE8_EPCTIN = 0xCD, // Control EEPROM
			NOKIA_GE8_EPCOUT = 0xCC, // Cancel EEPROM control
			NOKIA_GE8_EPMWR = 0xFC, // Write into EEPROM
			NOKIA_GE8_EPMRD = 0xFD, // Read from EEPROM
			NOKIA_GE8_EPSRRD1 = 0x7C, // Read register 1
			NOKIA_GE8_EPSRRD2 = 0x7D, // Read register 2
			NOKIA_GE8_NOP = 0x25, // NOP instruction
		};
	}
}

#endif // MODM_NOKIA6610_DEFINES_HPP
