/*********************************************************************************************************
 *
 * File                : LCD.c
 * Hardware Environment: 
 * Build Environment   : RealView MDK-ARM  Version: 4.20
 * Version             : V1.0
 * By                  : 
 *
 *                                  (c) Copyright 2005-2011, WaveShare
 *                                       http://www.waveshare.net
 *                                          All Rights Reserved
 *
 *********************************************************************************************************/

#include <xpcc/architecture.hpp>

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_fsmc.h"

using namespace xpcc::stm32;

__inline void LCD_WriteIndex(uint16_t index);
__inline void LCD_WriteData(uint16_t data);
__inline uint16_t LCD_ReadData(void);
__inline uint16_t LCD_ReadReg(uint16_t LCD_Reg);
__inline void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
static void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void delay_ms(uint16_t ms);

/* Private define ------------------------------------------------------------*/

#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60020000)) /* RS = 1 */

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */	   
#define  HX8347D    10 /* 0x0047 */
#define  HX8347A    11 /* 0x0047 */	
#define  LGDP4535   12 /* 0x4535 */  
#define  SSD2119    13 /* 3.5 LCD 0x9919 */

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code = SSD1289;

/*******************************************************************************
 * Function Name  : delay_ms
 * Description    : Delay Time
 * Input          : - cnt: Delay Time
 * Output         : None
 * Return         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void delay_ms(uint16_t ms)
{
	uint16_t i, j;
	for (i = 0; i < ms; i++)
	{
		for (j = 0; j < 1141; j++)
			;
	}
}

/*******************************************************************************
 * Function Name  : LCD_Initializtion
 * Description    : SSD1963 Resets
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
uint16_t DeviceCode = SSD1289;
uint16_t i;

/*******************************************************************************
 * Function Name  : LCD_Clear
 * Description    : 
 * Input          : - Color: Screen Color
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index = 0;

	if (LCD_Code == HX8347D || LCD_Code == HX8347A)
	{
		LCD_WriteReg(0x02, 0x00);
		LCD_WriteReg(0x03, 0x00);

		LCD_WriteReg(0x04, 0x00);
		LCD_WriteReg(0x05, 0xEF);

		LCD_WriteReg(0x06, 0x00);
		LCD_WriteReg(0x07, 0x00);

		LCD_WriteReg(0x08, 0x01);
		LCD_WriteReg(0x09, 0x3F);
	}
	else
	{
		LCD_SetCursor(0, 0);
	}

	LCD_WriteIndex(0x0022);
	for (index = 0; index < MAX_X * MAX_Y; index++)
	{
		LCD_WriteData(Color);
	}

}

static void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

	uint16_t temp;
	Ypos = (MAX_Y - 1) - Ypos;
	temp = Ypos;
	Ypos = Xpos;
	Xpos = temp;

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

	Ypos = ( MAX_Y - 1 ) - Ypos;

#endif

	switch (LCD_Code)
	{
	default: /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
		LCD_WriteReg(0x0020, Xpos);
		LCD_WriteReg(0x0021, Ypos);
		break;

	case SSD1298: /* 0x8999 */
	case SSD1289: /* 0x8989 */
		LCD_WriteReg(0x004e, Xpos);
		LCD_WriteReg(0x004f, Ypos);
		break;

	case HX8347A: /* 0x0047 */
	case HX8347D: /* 0x0047 */
		LCD_WriteReg(0x02, Xpos >> 8);
		LCD_WriteReg(0x03, Xpos);

		LCD_WriteReg(0x06, Ypos >> 8);
		LCD_WriteReg(0x07, Ypos);

		break;
	case SSD2119: /* 3.5 LCD 0x9919 */
		break;
	}
}

/******************************************************************************
 * Function Name  : LCD_BGR2RGB
 * Description    : RRRRRGGGGGGBBBBB convert to BBBBBGGGGGGRRRRR
 * Input          : RGB color
 * Output         : None
 * Return         : RGB color
 * Attention		 :
 *******************************************************************************/
static uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t r, g, b, rgb;

	b = (color >> 0) & 0x1f;
	g = (color >> 5) & 0x3f;
	r = (color >> 11) & 0x1f;

	rgb = (b << 11) + (g << 5) + (r << 0);

	return (rgb);
}

/******************************************************************************
 * Function Name  : LCD_GetPoint
 * Description    : Get color of the point
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate 
 * Output         : None
 * Return         : Screen Color
 * Attention		 : None
 *******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos, uint16_t Ypos)
{
	uint16_t dummy;

	LCD_SetCursor(Xpos, Ypos);

	LCD_WriteIndex(0x0022);

	switch (LCD_Code)
	{
	case ST7781:
	case LGDP4531:
	case LGDP4535:
	case SSD1289:
	case SSD1298:
		dummy = LCD_ReadData();
		dummy = LCD_ReadData();

		return dummy;
	case HX8347A:
	case HX8347D:
	{
		uint8_t red, green, blue;
		red = LCD_ReadData() >> 3;
		green = LCD_ReadData() >> 3;
		blue = LCD_ReadData() >> 2;
		dummy = (green << 11) | (blue << 5) | red;
	}
		return dummy;
	default: /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
		dummy = LCD_ReadData();
		dummy = LCD_ReadData();
		return LCD_BGR2RGB(dummy);
	}
}

/******************************************************************************
 * Function Name  : LCD_SetPoint
 * Description    : 
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate 
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t point)
{
	if (Xpos >= MAX_X || Ypos >= MAX_Y)
	{
		return;
	}
	LCD_SetCursor(Xpos, Ypos);
	LCD_WriteReg(0x0022, point);
}

/******************************************************************************
 * Function Name  : LCD_DrawLine
 * Description    : Bresenham's line algorithm
 * Input          : - x0:
 *                  - y0:
 *       				   - x1:
 *       		       - y1:
 *                  - color:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t color)
{
	short dx, dy;
	short temp;

	if (x0 > x1)
	{
		temp = x1;
		x1 = x0;
		x0 = temp;
	}
	if (y0 > y1)
	{
		temp = y1;
		y1 = y0;
		y0 = temp;
	}

	dx = x1 - x0;
	dy = y1 - y0;

	if (dx == 0)
	{
		do
		{
			LCD_SetPoint(x0, y0, color);
			y0++;
		} while (y1 >= y0);
		return;
	}
	if (dy == 0)
	{
		do
		{
			LCD_SetPoint(x0, y0, color);
			x0++;
		} while (x1 >= x0);
		return;
	}

	/* Bresenham's line algorithm  */
	if (dx > dy)
	{
		temp = 2 * dy - dx;
		while (x0 != x1)
		{
			LCD_SetPoint(x0, y0, color);
			x0++;
			if (temp > 0)
			{
				y0++;
				temp += 2 * dy - 2 * dx;
			}
			else
			{
				temp += 2 * dy;
			}
		}
		LCD_SetPoint(x0, y0, color);
	}
	else
	{
		temp = 2 * dx - dy;
		while (y0 != y1)
		{
			LCD_SetPoint(x0, y0, color);
			y0++;
			if (temp > 0)
			{
				x0++;
				temp += 2 * dy - 2 * dx;
			}
			else
			{
				temp += 2 * dy;
			}
		}
		LCD_SetPoint(x0, y0, color);
	}
}

/******************************************************************************
 * Function Name  : PutChar
 * Description    : 
 * Input          : - Xpos:
 *                  - Ypos:
 *				           - ASCI:
 *				           - charColor:
 *				           - bkColor:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void PutChar(uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor,
		uint16_t bkColor)
{
	uint16_t i, j;
	uint8_t buffer[16], tmp_char;
	GetASCIICode(buffer, ASCI);
	for (i = 0; i < 16; i++)
	{
		tmp_char = buffer[i];
		for (j = 0; j < 8; j++)
		{
			if (((tmp_char >> (7 - j)) & 0x01) == 0x01)
			{
				LCD_SetPoint(Xpos + j, Ypos + i, charColor);
			}
			else
			{
				LCD_SetPoint(Xpos + j, Ypos + i, bkColor);
			}
		}
	}
}

/******************************************************************************
 * Function Name  : GUI_Text
 * Description    : 
 * Input          : - Xpos: 
 *                  - Ypos: 
 *				           - str:
 *				           - charColor:
 *				           - bkColor:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, const char *str, uint16_t Color,
		uint16_t bkColor)
{
	uint8_t TempChar;
	do
	{
		TempChar = *str++;
		PutChar(Xpos, Ypos, TempChar, Color, bkColor);
		if (Xpos < (MAX_X - 8))
		{
			Xpos += 8;
		}
		else if (Ypos < (MAX_Y - 16))
		{
			Xpos = 0;
			Ypos += 16;
		}
		else
		{
			Xpos = 0;
			Ypos = 0;
		}
	} while (*str != 0);
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : 
 * Input          : - index:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
__inline void LCD_WriteIndex(uint16_t index)
{
	//GPIO_ResetBits(GPIOD , GPIO_Pin_7);		 //CS=0;
	LCD_REG = index;
	//GPIO_SetBits(GPIOD , GPIO_Pin_7);		 //CS=1;
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : 
 * Input          : - index:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
__inline void LCD_WriteData(uint16_t data)
{
	//GPIO_ResetBits(GPIOD , GPIO_Pin_7);		 //CS=0;
	LCD_RAM = data;
	//GPIO_SetBits(GPIOD , GPIO_Pin_7);		 //CS=1;
}

/*******************************************************************************
 * Function Name  : LCD_ReadData
 * Description    : 
 * Input          : None
 * Output         : None
 * Return         : 
 * Attention		 : None
 *******************************************************************************/
__inline uint16_t LCD_ReadData(void)
{
	//uint32_t tmp;
	//GPIO_ResetBits(GPIOD , GPIO_Pin_7);		 //CS=0;
	//tmp = LCD_RAM;
	//GPIO_SetBits(GPIOD , GPIO_Pin_7);		 //CS=1;
	return LCD_RAM;
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Writes to the selected LCD register.
 * Input          : - LCD_Reg: address of the selected register.
 *                  - LCD_RegValue: value to write to the selected register.
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
__inline void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	LCD_WriteIndex(LCD_Reg);
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Reads the selected LCD Register.
 * Input          : None
 * Output         : None
 * Return         : LCD Register Value.
 * Attention		 : None
 *******************************************************************************/
__inline uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	/* Write 16-bit Index (then Read Reg) */
	LCD_WriteIndex(LCD_Reg);

	/* Read 16-bit Reg */
	return LCD_ReadData();
}

/*********************************************************************************************************
 END FILE
 *********************************************************************************************************/
