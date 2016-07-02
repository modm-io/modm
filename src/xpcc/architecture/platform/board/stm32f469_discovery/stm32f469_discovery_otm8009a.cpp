// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

//
// STM32F469I-DISCO
// Discovery kit for STM32F469 line
// http://www.st.com/web/catalog/tools/FM116/CL1620/SC959/SS1532/LN1848/PF262395
//

#include "stm32f469_discovery.hpp"

static void dsi_write_command(uint32_t count, uint8_t *p)
{
	/* Wait for Command FIFO Empty */
	for (int t = 1024; not (DSI->GPSR & DSI_GPSR_CMDFE) and t; t--) {
		xpcc::delayMilliseconds(1);
	}

	if(count <= 1)
	{
		DSI->GHCR = (0x00000015 | (0 << 6) | (p[0] << 8) | (p[1] << 16));
	}
	else
	{
		DSI->GPDR = p[count] |
					(p[0] << 8) |
					(p[1] << 16) |
					(p[2] << 24);
		uint16_t counter = 3;

		for (int t = 1024; not (DSI->GPSR & DSI_GPSR_CMDFE) and t; t--) {
			xpcc::delayMilliseconds(1);
		}

		while(counter < count)
		{
			DSI->GPDR = p[counter] |
						(p[counter + 1] << 8) |
						(p[counter + 2] << 16) |
						(p[counter + 3] << 24);
			counter += 4;

			for (int t = 1024; not (DSI->GPSR & DSI_GPSR_CMDFE) and t; t--) {
				xpcc::delayMilliseconds(1);
			}
		}

		/* Configure the packet to send a long DCS command */
		DSI->GHCR = (0x00000039 | (0 << 6) | (((count + 1) & 0xFF) << 8) | (((count + 1) & 0xFF00) << 8));
	}
}


/**
  ******************************************************************************
  * @file    otm8009a.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    03-August-2015
  * @brief   This file provides the LCD Driver for KoD KM-040TMP-02-0621 (WVGA)
  *          DSI LCD Display OTM8009A.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

// This code is slightly modified!

/* List of OTM8009A used commands                                  */
/* Detailed in OTM8009A Data Sheet 'DATA_SHEET_OTM8009A_V0 92.pdf' */
/* Version of 14 June 2012                                         */
constexpr uint8_t OTM8009A_CMD_NOP                   = 0x00;  /* NOP command      */
constexpr uint8_t OTM8009A_CMD_SWRESET               = 0x01;  /* Sw reset command */
constexpr uint8_t OTM8009A_CMD_RDDMADCTL             = 0x0B;  /* Read Display MADCTR command : read memory display access ctrl */
constexpr uint8_t OTM8009A_CMD_RDDCOLMOD             = 0x0C;  /* Read Display pixel format */
constexpr uint8_t OTM8009A_CMD_SLPIN                 = 0x10;  /* Sleep In command */
constexpr uint8_t OTM8009A_CMD_SLPOUT                = 0x11;  /* Sleep Out command */
constexpr uint8_t OTM8009A_CMD_PTLON                 = 0x12;  /* Partial mode On command */
constexpr uint8_t OTM8009A_CMD_DISPOFF               = 0x28;  /* Display Off command */
constexpr uint8_t OTM8009A_CMD_DISPON                = 0x29;  /* Display On command */
constexpr uint8_t OTM8009A_CMD_CASET                 = 0x2A;  /* Column address set command */
constexpr uint8_t OTM8009A_CMD_PASET                 = 0x2B;  /* Page address set command */
constexpr uint8_t OTM8009A_CMD_RAMWR                 = 0x2C;  /* Memory (GRAM) write command */
constexpr uint8_t OTM8009A_CMD_RAMRD                 = 0x2E;  /* Memory (GRAM) read command  */
constexpr uint8_t OTM8009A_CMD_PLTAR                 = 0x30;  /* Partial area command (4 parameters) */
constexpr uint8_t OTM8009A_CMD_TEOFF                 = 0x34;  /* Tearing Effect Line Off command : command with no parameter */
constexpr uint8_t OTM8009A_CMD_TEEON                 = 0x35;  /* Tearing Effect Line On command : command with 1 parameter 'TELOM' */
constexpr uint8_t OTM8009A_CMD_MADCTR                = 0x36;  /* Memory Access write control command  */
constexpr uint8_t OTM8009A_CMD_IDMOFF                = 0x38;  /* Idle mode Off command */
constexpr uint8_t OTM8009A_CMD_IDMON                 = 0x39;  /* Idle mode On command  */
constexpr uint8_t OTM8009A_CMD_COLMOD                = 0x3A;  /* Interface Pixel format command */
constexpr uint8_t OTM8009A_CMD_RAMWRC                = 0x3C;  /* Memory write continue command */
constexpr uint8_t OTM8009A_CMD_RAMRDC                = 0x3E;  /* Memory read continue command  */
constexpr uint8_t OTM8009A_CMD_WRTESCN               = 0x44;  /* Write Tearing Effect Scan line command */
constexpr uint8_t OTM8009A_CMD_RDSCNL                = 0x45;  /* Read  Tearing Effect Scan line command */
/* CABC Management : ie : Content Adaptive Back light Control in IC OTM8009a */
constexpr uint8_t OTM8009A_CMD_WRDISBV               = 0x51;  /* Write Display Brightness command          */
constexpr uint8_t OTM8009A_CMD_WRCTRLD               = 0x53;  /* Write CTRL Display command                */
constexpr uint8_t OTM8009A_CMD_WRCABC                = 0x55;  /* Write Content Adaptive Brightness command */
constexpr uint8_t OTM8009A_CMD_WRCABCMB              = 0x5E;  /* Write CABC Minimum Brightness command     */

/* Possible used values of MADCTR */
constexpr uint8_t OTM8009A_MADCTR_MODE_PORTRAIT      = 0x00;
constexpr uint8_t OTM8009A_MADCTR_MODE_LANDSCAPE     = 0x60;  /* MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0 */
/* Parameter TELOM : Tearing Effect Line Output Mode : possible values */
constexpr uint8_t OTM8009A_TEEON_TELOM_VBLANKING_INFO_ONLY            = 0x00;
constexpr uint8_t OTM8009A_TEEON_TELOM_VBLANKING_AND_HBLANKING_INFO   = 0x01;
/* Possible values of COLMOD parameter corresponding to used pixel formats */
constexpr uint8_t OTM8009A_COLMOD_RGB565             = 0x55;
constexpr uint8_t OTM8009A_COLMOD_RGB888             = 0x77;

constexpr uint8_t OTM8009A_480X800_FREQUENCY_DIVIDER  = 2;   /* LCD Frequency divider      */

const uint8_t lcdRegData1[]  = {0x80,0x09,0x01,0xFF};
const uint8_t lcdRegData2[]  = {0x80,0x09,0xFF};
const uint8_t lcdRegData3[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE1};
const uint8_t lcdRegData4[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE2};
const uint8_t lcdRegData5[]  = {0x79,0x79,0xD8};
const uint8_t lcdRegData6[]  = {0x00,0x01,0xB3};
const uint8_t lcdRegData7[]  = {0x85,0x01,0x00,0x84,0x01,0x00,0xCE};
const uint8_t lcdRegData8[]  = {0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3A,0x00,0x00,0x00,0xCE};
const uint8_t lcdRegData9[]  = {0x18,0x02,0x03,0x3B,0x00,0x00,0x00,0x18,0x01,0x03,0x3C,0x00,0x00,0x00,0xCE};
const uint8_t lcdRegData10[] = {0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0xCF};
const uint8_t lcdRegData11[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData12[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData13[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData14[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData15[] = {0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData16[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData17[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData18[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCB};
const uint8_t lcdRegData19[] = {0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData20[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02,0xCC};
const uint8_t lcdRegData21[] = {0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData22[] = {0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData23[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01,0xCC};
const uint8_t lcdRegData24[] = {0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData25[] = {0xFF,0xFF,0xFF,0xFF};
/*
  * CASET value (Column Address Set) : X direction LCD GRAM boundaries
  * depending on LCD orientation mode and PASET value (Page Address Set) : Y direction
  * LCD GRAM boundaries depending on LCD orientation mode
  * XS[15:0] = 0x000 = 0, XE[15:0] = 0x31F = 799 for landscape mode : apply to CASET
  * YS[15:0] = 0x000 = 0, YE[15:0] = 0x31F = 799 for portrait mode : : apply to PASET
  */
const uint8_t lcdRegData27[] = {0x00, 0x00, 0x03, 0x1F, OTM8009A_CMD_CASET};
/*
  * XS[15:0] = 0x000 = 0, XE[15:0] = 0x1DF = 479 for portrait mode : apply to CASET
  * YS[15:0] = 0x000 = 0, YE[15:0] = 0x1DF = 479 for landscape mode : apply to PASET
 */
const uint8_t lcdRegData28[] = {0x00, 0x00, 0x01, 0xDF, OTM8009A_CMD_PASET};


const uint8_t ShortRegData1[]  = {OTM8009A_CMD_NOP, 0x00};
const uint8_t ShortRegData2[]  = {OTM8009A_CMD_NOP, 0x80};
const uint8_t ShortRegData3[]  = {0xC4, 0x30};
const uint8_t ShortRegData4[]  = {OTM8009A_CMD_NOP, 0x8A};
const uint8_t ShortRegData5[]  = {0xC4, 0x40};
const uint8_t ShortRegData6[]  = {OTM8009A_CMD_NOP, 0xB1};
const uint8_t ShortRegData7[]  = {0xC5, 0xA9};
const uint8_t ShortRegData8[]  = {OTM8009A_CMD_NOP, 0x91};
const uint8_t ShortRegData9[]  = {0xC5, 0x34};
const uint8_t ShortRegData10[] = {OTM8009A_CMD_NOP, 0xB4};
const uint8_t ShortRegData11[] = {0xC0, 0x50};
const uint8_t ShortRegData12[] = {0xD9, 0x4E};
const uint8_t ShortRegData13[] = {OTM8009A_CMD_NOP, 0x81};
const uint8_t ShortRegData14[] = {0xC1, 0x66};
const uint8_t ShortRegData15[] = {OTM8009A_CMD_NOP, 0xA1};
const uint8_t ShortRegData16[] = {0xC1, 0x08};
const uint8_t ShortRegData17[] = {OTM8009A_CMD_NOP, 0x92};
const uint8_t ShortRegData18[] = {0xC5, 0x01};
const uint8_t ShortRegData19[] = {OTM8009A_CMD_NOP, 0x95};
const uint8_t ShortRegData20[] = {OTM8009A_CMD_NOP, 0x94};
const uint8_t ShortRegData21[] = {0xC5, 0x33};
const uint8_t ShortRegData22[] = {OTM8009A_CMD_NOP, 0xA3};
const uint8_t ShortRegData23[] = {0xC0, 0x1B};
const uint8_t ShortRegData24[] = {OTM8009A_CMD_NOP, 0x82};
const uint8_t ShortRegData25[] = {0xC5, 0x83};
const uint8_t ShortRegData26[] = {0xC4, 0x83};
const uint8_t ShortRegData27[] = {0xC1, 0x0E};
const uint8_t ShortRegData28[] = {OTM8009A_CMD_NOP, 0xA6};
const uint8_t ShortRegData29[] = {OTM8009A_CMD_NOP, 0xA0};
const uint8_t ShortRegData30[] = {OTM8009A_CMD_NOP, 0xB0};
const uint8_t ShortRegData31[] = {OTM8009A_CMD_NOP, 0xC0};
const uint8_t ShortRegData32[] = {OTM8009A_CMD_NOP, 0xD0};
const uint8_t ShortRegData33[] = {OTM8009A_CMD_NOP, 0x90};
const uint8_t ShortRegData34[] = {OTM8009A_CMD_NOP, 0xE0};
const uint8_t ShortRegData35[] = {OTM8009A_CMD_NOP, 0xF0};
const uint8_t ShortRegData36[] = {OTM8009A_CMD_SLPOUT, 0x00};
const uint8_t ShortRegData37[] = {OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB565};
const uint8_t ShortRegData38[] = {OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB888};
const uint8_t ShortRegData39[] = {OTM8009A_CMD_MADCTR, OTM8009A_MADCTR_MODE_LANDSCAPE};
const uint8_t ShortRegData40[] = {OTM8009A_CMD_WRDISBV, 0x7F};
const uint8_t ShortRegData41[] = {OTM8009A_CMD_WRCTRLD, 0x2C};
const uint8_t ShortRegData42[] = {OTM8009A_CMD_WRCABC, 0x02};
const uint8_t ShortRegData43[] = {OTM8009A_CMD_WRCABCMB, 0xFF};
const uint8_t ShortRegData44[] = {OTM8009A_CMD_DISPON, 0x00};
const uint8_t ShortRegData45[] = {OTM8009A_CMD_RAMWR, 0x00};
const uint8_t ShortRegData46[] = {0xCF, 0x00};
const uint8_t ShortRegData47[] = {0xC5, 0x66};
const uint8_t ShortRegData48[] = {OTM8009A_CMD_NOP, 0xB6};
const uint8_t ShortRegData49[] = {0xF5, 0x06};

void otm8009a_init(uint8_t ColorCoding)
{
	/* Enable CMD2 to access vendor specific commands                               */
	/* Enter in command 2 mode and set EXTC to enable address shift function (0x00) */
	dsi_write_command(0, (uint8_t *)ShortRegData1);
	dsi_write_command( 3, (uint8_t *)lcdRegData1);

	/* Enter ORISE Command 2 */
	dsi_write_command(0, (uint8_t *)ShortRegData2); /* Shift address to 0x80 */
	dsi_write_command( 2, (uint8_t *)lcdRegData2);

	/////////////////////////////////////////////////////////////////////
	/* SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          */
	/* Set SD_PT                                                       */
	/* -> Source output level during porch and non-display area to GND */
	dsi_write_command(0, (uint8_t *)ShortRegData2);
	dsi_write_command(0, (uint8_t *)ShortRegData3);
	xpcc::delayMilliseconds(10);
	/* Not documented */
	dsi_write_command(0, (uint8_t *)ShortRegData4);
	dsi_write_command(0, (uint8_t *)ShortRegData5);
	xpcc::delayMilliseconds(10);
	/////////////////////////////////////////////////////////////////////

	/* PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8 */
	/* Set gvdd_en_test                                     */
	/* -> enable GVDD test mode !!!                         */
	dsi_write_command(0, (uint8_t *)ShortRegData6);
	dsi_write_command(0, (uint8_t *)ShortRegData7);

	/* PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79      */
	/* Set pump 4 vgh voltage                                    */
	/* -> from 15.0v down to 13.0v                               */
	/* Set pump 5 vgh voltage                                    */
	/* -> from -12.0v downto -9.0v                               */
	dsi_write_command(0, (uint8_t *)ShortRegData8);
	dsi_write_command(0, (uint8_t *)ShortRegData9);

	/* P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 */
	/* -> Column inversion                                */
	dsi_write_command(0, (uint8_t *)ShortRegData10);
	dsi_write_command(0, (uint8_t *)ShortRegData11);

	/* VCOMDC - 0xD900h - 1st parameter - Default 0x39h */
	/* VCOM Voltage settings                            */
	/* -> from -1.0000v downto -1.2625v                 */
	dsi_write_command(0, (uint8_t *)ShortRegData1);
	dsi_write_command(0, (uint8_t *)ShortRegData12);

	/* Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) */
	dsi_write_command(0, (uint8_t *)ShortRegData13);
	dsi_write_command(0, (uint8_t *)ShortRegData14);

	/* Video mode internal */
	dsi_write_command(0, (uint8_t *)ShortRegData15);
	dsi_write_command(0, (uint8_t *)ShortRegData16);

	/* PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 */
	/* Set pump 4&5 x6                                     */
	/* -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           */
	dsi_write_command(0, (uint8_t *)ShortRegData17);
	dsi_write_command(0, (uint8_t *)ShortRegData18);

	/* PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h */
	/* Change pump4 clock ratio                              */
	/* -> from 1 line to 1/2 line                            */
	dsi_write_command(0, (uint8_t *)ShortRegData19);
	dsi_write_command(0, (uint8_t *)ShortRegData9);

	/* GVDD/NGVDD settings */
	dsi_write_command(0, (uint8_t *)ShortRegData1);
	dsi_write_command( 2, (uint8_t *)lcdRegData5);

	/* PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h */
	/* Rewrite the default value !                           */
	dsi_write_command(0, (uint8_t *)ShortRegData20);
	dsi_write_command(0, (uint8_t *)ShortRegData21);

	/* Panel display timing Setting 3 */
	dsi_write_command(0, (uint8_t *)ShortRegData22);
	dsi_write_command(0, (uint8_t *)ShortRegData23);

	/* Power control 1 */
	dsi_write_command(0, (uint8_t *)ShortRegData24);
	dsi_write_command(0, (uint8_t *)ShortRegData25);

	/* Source driver precharge */
	dsi_write_command(0, (uint8_t *)ShortRegData13);
	dsi_write_command(0, (uint8_t *)ShortRegData26);

	dsi_write_command(0, (uint8_t *)ShortRegData15);
	dsi_write_command(0, (uint8_t *)ShortRegData27);

	dsi_write_command(0, (uint8_t *)ShortRegData28);
	dsi_write_command( 2, (uint8_t *)lcdRegData6);

	/* GOAVST */
	dsi_write_command(0, (uint8_t *)ShortRegData2);
	dsi_write_command( 6, (uint8_t *)lcdRegData7);

	dsi_write_command(0, (uint8_t *)ShortRegData29);
	dsi_write_command( 14, (uint8_t *)lcdRegData8);

	dsi_write_command(0, (uint8_t *)ShortRegData30);
	dsi_write_command( 14, (uint8_t *)lcdRegData9);

	dsi_write_command(0, (uint8_t *)ShortRegData31);
	dsi_write_command( 10, (uint8_t *)lcdRegData10);

	dsi_write_command(0, (uint8_t *)ShortRegData32);
	dsi_write_command(0, (uint8_t *)ShortRegData46);

	dsi_write_command(0, (uint8_t *)ShortRegData2);
	dsi_write_command( 10, (uint8_t *)lcdRegData11);

	dsi_write_command(0, (uint8_t *)ShortRegData33);
	dsi_write_command( 15, (uint8_t *)lcdRegData12);

	dsi_write_command(0, (uint8_t *)ShortRegData29);
	dsi_write_command( 15, (uint8_t *)lcdRegData13);

	dsi_write_command(0, (uint8_t *)ShortRegData30);
	dsi_write_command( 10, (uint8_t *)lcdRegData14);

	dsi_write_command(0, (uint8_t *)ShortRegData31);
	dsi_write_command( 15, (uint8_t *)lcdRegData15);

	dsi_write_command(0, (uint8_t *)ShortRegData32);
	dsi_write_command( 15, (uint8_t *)lcdRegData16);

	dsi_write_command(0, (uint8_t *)ShortRegData34);
	dsi_write_command( 10, (uint8_t *)lcdRegData17);

	dsi_write_command(0, (uint8_t *)ShortRegData35);
	dsi_write_command( 10, (uint8_t *)lcdRegData18);

	dsi_write_command(0, (uint8_t *)ShortRegData2);
	dsi_write_command( 10, (uint8_t *)lcdRegData19);

	dsi_write_command(0, (uint8_t *)ShortRegData33);
	dsi_write_command( 15, (uint8_t *)lcdRegData20);

	dsi_write_command(0, (uint8_t *)ShortRegData29);
	dsi_write_command( 15, (uint8_t *)lcdRegData21);

	dsi_write_command(0, (uint8_t *)ShortRegData30);
	dsi_write_command( 10, (uint8_t *)lcdRegData22);

	dsi_write_command(0, (uint8_t *)ShortRegData31);
	dsi_write_command( 15, (uint8_t *)lcdRegData23);

	dsi_write_command(0, (uint8_t *)ShortRegData32);
	dsi_write_command( 15, (uint8_t *)lcdRegData24);

	/////////////////////////////////////////////////////////////////////////////
	/* PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 */
	/* Pump 1 min and max DM                                */
	dsi_write_command(0, (uint8_t *)ShortRegData13);
	dsi_write_command(0, (uint8_t *)ShortRegData47);
	dsi_write_command(0, (uint8_t *)ShortRegData48);
	dsi_write_command(0, (uint8_t *)ShortRegData49);
	/////////////////////////////////////////////////////////////////////////////

	/* Exit CMD2 mode */
	dsi_write_command(0, (uint8_t *)ShortRegData1);
	dsi_write_command( 3, (uint8_t *)lcdRegData25);

	/*************************************************************************** */
	/* Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   */
	/*************************************************************************** */

	/* NOP - goes back to DCS std command ? */
	dsi_write_command(0, (uint8_t *)ShortRegData1);

	/* Gamma correction 2.2+ table (HSDT possible) */
	dsi_write_command(0, (uint8_t *)ShortRegData1);
	dsi_write_command( 16, (uint8_t *)lcdRegData3);

	/* Gamma correction 2.2- table (HSDT possible) */
	dsi_write_command(0, (uint8_t *)ShortRegData1);
	dsi_write_command( 16, (uint8_t *)lcdRegData4);

	/* Send Sleep Out command to display : no parameter */
	dsi_write_command(0, (uint8_t *)ShortRegData36);

	/* Wait for sleep out exit */
	xpcc::delayMilliseconds(120);

	if (ColorCoding == 0) {
		dsi_write_command(0, (uint8_t *)ShortRegData38);
	} else {
		dsi_write_command(0, (uint8_t *)ShortRegData37);
	}

	/* Send command to configure display in landscape orientation mode. By default
	  the orientation mode is portrait  */
	dsi_write_command(0, (uint8_t *)ShortRegData39);
	dsi_write_command( 4, (uint8_t *)lcdRegData27);
	dsi_write_command( 4, (uint8_t *)lcdRegData28);

	/** CABC : Content Adaptive Backlight Control section start >> */
	/* Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value */
	dsi_write_command(0, (uint8_t *)ShortRegData40);

	/* defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
	dsi_write_command(0, (uint8_t *)ShortRegData41);

	/* defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] */
	dsi_write_command(0, (uint8_t *)ShortRegData42);

	/* defaut is 0 (lowest Brightness), 0xFF is highest Brightness */
	dsi_write_command(0, (uint8_t *)ShortRegData43);

	/** CABC : Content Adaptive Backlight Control section end << */

	/* Send Command Display On */
	dsi_write_command(0, (uint8_t *)ShortRegData44);

	/* NOP command */
	dsi_write_command(0, (uint8_t *)ShortRegData1);

	/* Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by */
	/* DSI host from LTDC incoming pixels in video mode */
	dsi_write_command(0, (uint8_t *)ShortRegData45);
}
