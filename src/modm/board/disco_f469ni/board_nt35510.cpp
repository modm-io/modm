
#include <modm/board.hpp>

static void dsi_write_command(uint32_t count, uint8_t const * const p)
{
	/* Wait for Command FIFO Empty */
	for (int t = 1'024; not (DSI->GPSR & DSI_GPSR_CMDFE) and t; t--) {
		modm::delay_ms(1);
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

		for (int t = 1'024; not (DSI->GPSR & DSI_GPSR_CMDFE) and t; t--) {
			modm::delay_ms(1);
		}

		while(counter < count)
		{
			DSI->GPDR = p[counter] |
						(p[counter + 1] << 8) |
						(p[counter + 2] << 16) |
						(p[counter + 3] << 24);
			counter += 4;

			for (int t = 1'024; not (DSI->GPSR & DSI_GPSR_CMDFE) and t; t--) {
				modm::delay_ms(1);
			}
		}

		/* Configure the packet to send a long DCS command */
		DSI->GHCR = (0x00000039 | (0 << 6) | (((count + 1) & 0xFF) << 8) | (((count + 1) & 0xFF00) << 8));
	}
}



/**
  ******************************************************************************
  * @file    nt35510.c
  * @author  MCD Application Team
  * @brief   This file provides the LCD Driver for Frida Techshine 3K138 (WVGA)
  *          DSI LCD Display NT35510.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

 /**
 *  @brief LCD_OrientationTypeDef
 *  Possible values of Display Orientation
 */
constexpr uint32_t NT35510_ORIENTATION_PORTRAIT    = ((uint32_t)0x00); /* Portrait orientation choice of LCD screen  */
constexpr uint32_t NT35510_ORIENTATION_LANDSCAPE   = ((uint32_t)0x01); /* Landscape orientation choice of LCD screen */

/**
 *  @brief  Possible values of
 *  pixel data format (ie color coding) transmitted on DSI Data lane in DSI packets
 */
constexpr uint32_t NT35510_FORMAT_RGB888            = ((uint32_t)0x00); /* Pixel format chosen is RGB888 : 24 bpp */
constexpr uint32_t NT35510_FORMAT_RBG565            = ((uint32_t)0x02); /* Pixel format chosen is RGB565 : 16 bpp */

/**
  * @brief  nt35510_480x800 Size
  */

/* Width and Height in Portrait mode */
constexpr uint16_t  NT35510_480X800_WIDTH             = ((uint16_t)480);     /* LCD PIXEL WIDTH   */
constexpr uint16_t  NT35510_480X800_HEIGHT            = ((uint16_t)800);     /* LCD PIXEL HEIGHT  */

/* Width and Height in Landscape mode */
constexpr uint16_t  NT35510_800X480_WIDTH              = ((uint16_t)800);     /* LCD PIXEL WIDTH   */
constexpr uint16_t  NT35510_800X480_HEIGHT             = ((uint16_t)480);     /* LCD PIXEL HEIGHT  */

/**
  * @brief  NT35510_480X800 Timing parameters for Portrait orientation mode
  */
constexpr uint16_t  NT35510_480X800_HSYNC             = ((uint16_t)2);      /* Horizontal synchronization */
constexpr uint16_t  NT35510_480X800_HBP               = ((uint16_t)34);     /* Horizontal back porch      */
constexpr uint16_t  NT35510_480X800_HFP               = ((uint16_t)34);     /* Horizontal front porch     */
constexpr uint16_t  NT35510_480X800_VSYNC             = ((uint16_t)120);      /* Vertical synchronization   */
constexpr uint16_t  NT35510_480X800_VBP               = ((uint16_t)150);     /* Vertical back porch        */
constexpr uint16_t  NT35510_480X800_VFP               = ((uint16_t)150);     /* Vertical front porch       */

/**
  * @brief  NT35510_800X480 Timing parameters for Landscape orientation mode
  *         Same values as for Portrait mode in fact.
  */
constexpr uint16_t  NT35510_800X480_HSYNC             = NT35510_480X800_VSYNC;  /* Horizontal synchronization */
constexpr uint16_t  NT35510_800X480_HBP               = NT35510_480X800_VBP;    /* Horizontal back porch      */
constexpr uint16_t  NT35510_800X480_HFP               = NT35510_480X800_VFP;    /* Horizontal front porch     */
constexpr uint16_t  NT35510_800X480_VSYNC             = NT35510_480X800_HSYNC;  /* Vertical synchronization   */
constexpr uint16_t  NT35510_800X480_VBP               = NT35510_480X800_HBP;    /* Vertical back porch        */
constexpr uint16_t  NT35510_800X480_VFP               = NT35510_480X800_HFP;    /* Vertical front porch       */


/* List of NT35510 used commands                                  */
/* Detailed in NT35510 Data Sheet v0.80                           */
/* Version of 10/28/2011                                          */
/* Command, NumberOfArguments                                     */

constexpr uint8_t  NT35510_CMD_NOP                   = 0x00;  /* NOP */
constexpr uint8_t  NT35510_CMD_SWRESET               = 0x01;  /* SW reset */
constexpr uint8_t  NT35510_CMD_RDDID                 = 0x04;  /* Read display ID */
constexpr uint8_t  NT35510_CMD_RDNUMED               = 0x05;  /* Read number of errors on DSI */
constexpr uint8_t  NT35510_CMD_RDDPM                 = 0x0A;  /* Read display power mode */
constexpr uint8_t  NT35510_CMD_RDDMADCTL             = 0x0B;  /* Read display MADCTL */
constexpr uint8_t  NT35510_CMD_RDDCOLMOD             = 0x0C;  /* Read display pixel format */
constexpr uint8_t  NT35510_CMD_RDDIM                 = 0x0D;  /* Read display image mode */
constexpr uint8_t  NT35510_CMD_RDDSM                 = 0x0E;  /* Read display signal mode */
constexpr uint8_t  NT35510_CMD_RDDSDR                = 0x0F;  /* Read display self-diagnostics result */
constexpr uint8_t  NT35510_CMD_SLPIN                 = 0x10;  /* Sleep in */
constexpr uint8_t  NT35510_CMD_SLPOUT                = 0x11;  /* Sleep out */
constexpr uint8_t  NT35510_CMD_PTLON                 = 0x12;  /* Partial mode on  */
constexpr uint8_t  NT35510_CMD_NORON                 = 0x13;  /* Normal display mode on */
constexpr uint8_t  NT35510_CMD_INVOFF                = 0x20;  /* Display inversion off */
constexpr uint8_t  NT35510_CMD_INVON                 = 0x21;  /* Display inversion on */
constexpr uint8_t  NT35510_CMD_ALLPOFF               = 0x22;  /* All pixel off */
constexpr uint8_t  NT35510_CMD_ALLPON                = 0x23;  /* All pixel on */
constexpr uint8_t  NT35510_CMD_GAMSET                = 0x26;  /* Gamma set */
constexpr uint8_t  NT35510_CMD_DISPOFF               = 0x28;  /* Display off */
constexpr uint8_t  NT35510_CMD_DISPON                = 0x29;  /* Display on */
constexpr uint8_t  NT35510_CMD_CASET                 = 0x2A;  /* Column address set */
constexpr uint8_t  NT35510_CMD_RASET                 = 0x2B;  /* Row address set */
constexpr uint8_t  NT35510_CMD_RAMWR                 = 0x2C;  /* Memory write */
constexpr uint8_t  NT35510_CMD_RAMRD                 = 0x2E;  /* Memory read  */
constexpr uint8_t  NT35510_CMD_PLTAR                 = 0x30;  /* Partial area */
constexpr uint8_t  NT35510_CMD_TOPC                  = 0x32;  /* Turn On Peripheral Command */
constexpr uint8_t  NT35510_CMD_TEOFF                 = 0x34;  /* Tearing effect line off */
constexpr uint8_t  NT35510_CMD_TEEON                 = 0x35;  /* Tearing effect line on */
constexpr uint8_t  NT35510_CMD_MADCTL                = 0x36;  /* Memory data access control */
constexpr uint8_t  NT35510_CMD_IDMOFF                = 0x38;  /* Idle mode off */
constexpr uint8_t  NT35510_CMD_IDMON                 = 0x39;  /* Idle mode on */
constexpr uint8_t  NT35510_CMD_COLMOD                = 0x3A;  /* Interface pixel format */
constexpr uint8_t  NT35510_CMD_RAMWRC                = 0x3C;  /* Memory write continue */
constexpr uint8_t  NT35510_CMD_RAMRDC                = 0x3E;  /* Memory read continue */
constexpr uint8_t  NT35510_CMD_STESL                 = 0x44;  /* Set tearing effect scan line */
constexpr uint8_t  NT35510_CMD_GSL                   = 0x45;  /* Get scan line */
constexpr uint8_t  NT35510_CMD_DSTBON                = 0x4F;  /* Deep standby mode on */
constexpr uint8_t  NT35510_CMD_WRPFD                 = 0x50;  /* Write profile value for display */
constexpr uint8_t  NT35510_CMD_WRDISBV               = 0x51;  /* Write display brightness */
constexpr uint8_t  NT35510_CMD_RDDISBV               = 0x52;  /* Read display brightness */
constexpr uint8_t  NT35510_CMD_WRCTRLD               = 0x53;  /* Write CTRL display */
constexpr uint8_t  NT35510_CMD_RDCTRLD               = 0x54;  /* Read CTRL display value */
constexpr uint8_t  NT35510_CMD_WRCABC                = 0x55;  /* Write content adaptative brightness control */
constexpr uint8_t  NT35510_CMD_RDCABC                = 0x56;  /* Read content adaptive brightness control */
constexpr uint8_t  NT35510_CMD_WRHYSTE               = 0x57;  /* Write hysteresis */
constexpr uint8_t  NT35510_CMD_WRGAMMSET             = 0x58;  /* Write gamme setting */
constexpr uint8_t  NT35510_CMD_RDFSVM                = 0x5A;  /* Read FS value MSBs */
constexpr uint8_t  NT35510_CMD_RDFSVL                = 0x5B;  /* Read FS value LSBs */
constexpr uint8_t  NT35510_CMD_RDMFFSVM              = 0x5C;  /* Read median filter FS value MSBs */
constexpr uint8_t  NT35510_CMD_RDMFFSVL              = 0x5D;  /* Read median filter FS value LSBs */
constexpr uint8_t  NT35510_CMD_WRCABCMB              = 0x5E;  /* Write CABC minimum brightness */
constexpr uint8_t  NT35510_CMD_RDCABCMB              = 0x5F;  /* Read CABC minimum brightness */
constexpr uint8_t  NT35510_CMD_WRLSCC                = 0x65;  /* Write light sensor compensation coefficient value */
constexpr uint8_t  NT35510_CMD_RDLSCCM               = 0x66;  /* Read light sensor compensation coefficient value MSBs */
constexpr uint8_t  NT35510_CMD_RDLSCCL               = 0x67;  /* Read light sensor compensation coefficient value LSBs */
constexpr uint8_t  NT35510_CMD_RDBWLB                = 0x70;  /* Read black/white low bits */
constexpr uint8_t  NT35510_CMD_RDBKX                 = 0x71;  /* Read Bkx */
constexpr uint8_t  NT35510_CMD_RDBKY                 = 0x72;  /* Read Bky */
constexpr uint8_t  NT35510_CMD_RDWX                  = 0x73;  /* Read Wx */
constexpr uint8_t  NT35510_CMD_RDWY                  = 0x74;  /* Read Wy */
constexpr uint8_t  NT35510_CMD_RDRGLB                = 0x75;  /* Read red/green low bits */
constexpr uint8_t  NT35510_CMD_RDRX                  = 0x76;  /* Read Rx */
constexpr uint8_t  NT35510_CMD_RDRY                  = 0x77;  /* Read Ry */
constexpr uint8_t  NT35510_CMD_RDGX                  = 0x78;  /* Read Gx */
constexpr uint8_t  NT35510_CMD_RDGY                  = 0x79;  /* Read Gy */
constexpr uint8_t  NT35510_CMD_RDBALB                = 0x7A;  /* Read blue/acolor low bits */
constexpr uint8_t  NT35510_CMD_RDBX                  = 0x7B;  /* Read Bx */
constexpr uint8_t  NT35510_CMD_RDBY                  = 0x7C;  /* Read By */
constexpr uint8_t  NT35510_CMD_RDAX                  = 0x7D;  /* Read Ax */
constexpr uint8_t  NT35510_CMD_RDAY                  = 0x7E;  /* Read Ay */
constexpr uint8_t  NT35510_CMD_RDDDBS                = 0xA1;  /* Read DDB start */
constexpr uint8_t  NT35510_CMD_RDDDBC                = 0xA8;  /* Read DDB continue */
constexpr uint8_t  NT35510_CMD_RDDCS                 = 0xAA;  /* Read first checksum */
constexpr uint8_t  NT35510_CMD_RDCCS                 = 0xAF;  /* Read continue checksum */
constexpr uint8_t  NT35510_CMD_RDID1                 = 0xDA;  /* Read ID1 value */
constexpr uint8_t  NT35510_CMD_RDID2                 = 0xDB;  /* Read ID2 value */
constexpr uint8_t  NT35510_CMD_RDID3                 = 0xDC;  /* Read ID3 value */

/* Parameter TELOM : Tearing Effect Line Output Mode : possible values */
constexpr uint8_t NT35510_TEEON_TELOM_VBLANKING_INFO_ONLY            = 0x00;
constexpr uint8_t NT35510_TEEON_TELOM_VBLANKING_AND_HBLANKING_INFO   = 0x01;

/* Possible used values of MADCTR */
constexpr uint8_t NT35510_MADCTR_MODE_PORTRAIT       = 0x00;
constexpr uint8_t NT35510_MADCTR_MODE_LANDSCAPE      = 0x60;  /* MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0 */

/* Possible values of COLMOD parameter corresponding to used pixel formats */
constexpr uint8_t NT35510_COLMOD_RGB565             = 0x55;
constexpr uint8_t NT35510_COLMOD_RGB888             = 0x77;

/**
  * @brief  NT35510_480X800 frequency divider
  */
constexpr uint8_t NT35510_480X800_FREQUENCY_DIVIDER  = 2;   /* LCD Frequency divider      */


/* ************************************************************************** */
/* Proprietary Initialization                                                 */
/* ************************************************************************** */
const uint8_t nt35510_reg[]   = {0x55, 0xAA, 0x52, 0x08, 0x01, 0xF0};
const uint8_t nt35510_reg1[]  = {0x03, 0x03, 0x03, 0xB0};
const uint8_t nt35510_reg2[]  = {0x46, 0x46, 0x46, 0xB6};
const uint8_t nt35510_reg3[]  = {0x03, 0x03, 0x03, 0xB1};
const uint8_t nt35510_reg4[]  = {0x36, 0x36, 0x36, 0xB7};
const uint8_t nt35510_reg5[]  = {0x00, 0x00, 0x02, 0xB2};
const uint8_t nt35510_reg6[]  = {0x26, 0x26, 0x26, 0xB8};
const uint8_t nt35510_reg7[]  = {0xBF, 0x01};
const uint8_t nt35510_reg8[]  = {0x09, 0x09, 0x09, 0xB3};
const uint8_t nt35510_reg9[]  = {0x36, 0x36, 0x36, 0xB9};
const uint8_t nt35510_reg10[] = {0x08, 0x08, 0x08, 0xB5};
const uint8_t nt35510_reg12[] = {0x26, 0x26, 0x26, 0xBA};
const uint8_t nt35510_reg13[] = {0x00, 0x80, 0x00, 0xBC};
const uint8_t nt35510_reg14[] = {0x00, 0x80, 0x00, 0xBD};
const uint8_t nt35510_reg15[] = {0x00, 0x50, 0xBE};
const uint8_t nt35510_reg16[] = {0x55, 0xAA, 0x52, 0x08, 0x00, 0xF0};
const uint8_t nt35510_reg17[] = {0xFC, 0x00, 0xB1};
const uint8_t nt35510_reg18[] = {0xB6, 0x03};
const uint8_t nt35510_reg19[] = {0xB5, 0x51};
const uint8_t nt35510_reg20[] = {0x00, 0x00, 0xB7};
const uint8_t nt35510_reg21[] = {0x01, 0x02, 0x02, 0x02, 0xB8};
const uint8_t nt35510_reg22[] = {0x00, 0x00, 0x00, 0xBC};
const uint8_t nt35510_reg23[] = {0x03, 0x00, 0x00, 0xCC};
const uint8_t nt35510_reg24[] = {0xBA, 0x01};
const uint8_t nt35510_madctl_portrait[] = {NT35510_CMD_MADCTL ,0x00};
const uint8_t nt35510_caset_portrait[] = {0x00, 0x00, 0x01, 0xDF ,NT35510_CMD_CASET};
const uint8_t nt35510_raset_portrait[] = {0x00, 0x00, 0x03, 0x1F ,NT35510_CMD_RASET};
const uint8_t nt35510_madctl_landscape[] = {NT35510_CMD_MADCTL, 0x60};
const uint8_t nt35510_caset_landscape[] = {0x00, 0x00, 0x03, 0x1F ,NT35510_CMD_CASET};
const uint8_t nt35510_raset_landscape[] = {0x00, 0x00, 0x01, 0xDF ,NT35510_CMD_RASET};
const uint8_t nt35510_reg26[] = {NT35510_CMD_TEEON, 0x00};  /* Tear on */
const uint8_t nt35510_reg27[] = {NT35510_CMD_SLPOUT, 0x00}; /* Sleep out */
const uint8_t nt35510_reg30[] = {NT35510_CMD_DISPON, 0x00};

const uint8_t nt35510_reg31[] = {NT35510_CMD_WRDISBV, 0x7F};
const uint8_t nt35510_reg32[] = {NT35510_CMD_WRCTRLD, 0x2C};
const uint8_t nt35510_reg33[] = {NT35510_CMD_WRCABC, 0x02};
const uint8_t nt35510_reg34[] = {NT35510_CMD_WRCABCMB, 0xFF};
const uint8_t nt35510_reg35[] = {NT35510_CMD_RAMWR, 0x00};
const uint8_t nt35510_reg36[] = {NT35510_CMD_COLMOD, NT35510_COLMOD_RGB565};
const uint8_t nt35510_reg37[] = {NT35510_CMD_COLMOD, NT35510_COLMOD_RGB888};

uint8_t nt35510_init(uint8_t ColorCoding)
{
  modm::delay_ms(120);

  dsi_write_command(5, nt35510_reg); /* LV2:  Page 1 enable */
  dsi_write_command(3, nt35510_reg1);/* AVDD: 5.2V */
  dsi_write_command(3, nt35510_reg2);/* AVDD: Ratio */
  dsi_write_command(3, nt35510_reg3);/* AVEE: -5.2V */
  dsi_write_command(3, nt35510_reg4);/* AVEE: Ratio */
  dsi_write_command(3, nt35510_reg5);/* VCL: -2.5V */
  dsi_write_command(3, nt35510_reg6);/* VCL: Ratio */
  dsi_write_command(1, nt35510_reg7);/* VGH: 15V (Free Pump) */
  dsi_write_command(3, nt35510_reg8);
  dsi_write_command(3, nt35510_reg9);/* VGH: Ratio */
  dsi_write_command(3, nt35510_reg10);/* VGL_REG: -10V */
  dsi_write_command(3, nt35510_reg12);/* VGLX: Ratio */
  dsi_write_command(3, nt35510_reg13);/* VGMP/VGSP: 4.5V/0V */
  dsi_write_command(3, nt35510_reg14);/* VGMN/VGSN:-4.5V/0V */
  dsi_write_command(2, nt35510_reg15);/* VCOM: -1.325V */

/* ************************************************************************** */
/* Proprietary DCS Initialization                                             */
/* ************************************************************************** */
  dsi_write_command(5, nt35510_reg16);/* LV2: Page 0 enable */
  dsi_write_command(2, nt35510_reg17);/* Display control */
  dsi_write_command(1, nt35510_reg18);/* Src hold time */
  dsi_write_command(1, nt35510_reg19);
  dsi_write_command(2, nt35510_reg20);/* Gate EQ control */
  dsi_write_command(4, nt35510_reg21);/* Src EQ control(Mode2) */
  dsi_write_command(3, nt35510_reg22);/* Inv. mode(2-dot) */
  dsi_write_command(3, nt35510_reg23);
  dsi_write_command(1, nt35510_reg24);
  /* Tear on */
  dsi_write_command(0, nt35510_reg26);
  /* Set Pixel color format to RGB888 */
  dsi_write_command(0, nt35510_reg37);

/* ************************************************************************** */
/* Standard DCS Initialization                                                */
/* ************************************************************************** */

  /* Add a delay, otherwise MADCTL not taken */
  modm::delay_ms(200);

  /* Configure orientation */
  // if(orientation == NT35510_ORIENTATION_PORTRAIT)
  // {
  //   dsi_write_command(1, nt35510_madctl_portrait);
  //   dsi_write_command(4, nt35510_caset_portrait);
  //   dsi_write_command(4, nt35510_raset_portrait);
  // }
  // else
  // {
  dsi_write_command(1, (uint8_t *)nt35510_madctl_landscape);
  dsi_write_command(4, (uint8_t *)nt35510_caset_landscape);
  dsi_write_command(4, (uint8_t *)nt35510_raset_landscape);
  // }

  dsi_write_command(0, (uint8_t *)nt35510_reg27);
  /* Wait for sleep out exit */
  modm::delay_ms(120);

  switch(ColorCoding)
  {
    case NT35510_FORMAT_RBG565 :
      /* Set Pixel color format to RGB565 */
      dsi_write_command(1, nt35510_reg36);
      break;
    case NT35510_FORMAT_RGB888 :
      /* Set Pixel color format to RGB888 */
      dsi_write_command(1, nt35510_reg37);
      break;
    default :
      /* Set Pixel color format to RGB888 */
      dsi_write_command(1, nt35510_reg37);
      break;
  }

  /** CABC : Content Adaptive Backlight Control section start >> */
  /* Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value */
  dsi_write_command(1, nt35510_reg31);
  /* defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
  dsi_write_command(1, nt35510_reg32);
  /* defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] */
  dsi_write_command(1, nt35510_reg33);
  /* defaut is 0 (lowest Brightness), 0xFF is highest Brightness */
  dsi_write_command(1, nt35510_reg34);
  /** CABC : Content Adaptive Backlight Control section end << */

  /* Display on */
  dsi_write_command(0, nt35510_reg30);

  /* Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by */
  /* DSI host from LTDC incoming pixels in video mode */
  dsi_write_command(0, nt35510_reg35);

  return 0;
}