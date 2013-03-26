// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#include "touchscreen_calibrator.hpp"


#include <xpcc/architecture.hpp>
#include <xpcc/driver/ui/touchscreen/ads7843.hpp>

#include "lcd.h"

using namespace xpcc::stm32;

GPIO__OUTPUT(Cs, C, 4);
GPIO__INPUT(Int, C, 5);

xpcc::Ads7843<SpiMaster2, Cs, Int> ads7843;

/* Private define ------------------------------------------------------------*/

#define CHX 	0x90
#define CHY 	0xd0

/* Private variables ---------------------------------------------------------*/
Matrix matrix;
Coordinate display;

Coordinate ScreenSample[3];

Coordinate DisplaySample[3] = { { 45, 45 }, { 270, 90 }, { 100, 190 } };

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2

/*******************************************************************************
 * Function Name  : TP_Init
 * Description    : 
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void TP_Init(void)
{
	Cs::setOutput();
	Cs::set();
	
	Int::setInput(PULLUP);

	SpiMaster2::initialize(SpiMaster2::MODE_0, SpiMaster2::PRESCALER_64);
	SpiMaster2::configurePins(SpiMaster2::REMAP_PB13_PB14_PB15);
	
	ads7843.initialize();
}

/*******************************************************************************
 * Function Name  : TP_GetAdXY
 * Description    : Read ADS7843
 * Input          : None
 * Output         : None
 * Return         : 
 * Attention		 : None
 *******************************************************************************/
void TP_GetAdXY(int *x, int *y)
{
	int adx, ady;
	adx = ads7843.readX();
	xpcc::delay_us(1);
	ady = ads7843.readY();
	*x = adx;
	*y = ady;
}

/*******************************************************************************
 * Function Name  : TP_DrawPoint
 * Description    : 
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate 
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void TP_DrawPoint(uint16_t Xpos, uint16_t Ypos)
{
	LCD_SetPoint(Xpos, Ypos, Blue); /* Center point */
	LCD_SetPoint(Xpos + 1, Ypos, Blue);
	LCD_SetPoint(Xpos, Ypos + 1, Blue);
	LCD_SetPoint(Xpos + 1, Ypos + 1, Blue);
}

/*******************************************************************************
 * Function Name  : DrawCross
 * Description    : 
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate 
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void DrawCross(uint16_t Xpos, uint16_t Ypos)
{
	LCD_DrawLine(Xpos - 15, Ypos, Xpos - 2, Ypos, 0xffff);
	LCD_DrawLine(Xpos + 2, Ypos, Xpos + 15, Ypos, 0xffff);
	LCD_DrawLine(Xpos, Ypos - 15, Xpos, Ypos - 2, 0xffff);
	LCD_DrawLine(Xpos, Ypos + 2, Xpos, Ypos + 15, 0xffff);

	LCD_DrawLine(Xpos - 15, Ypos + 15, Xpos - 7, Ypos + 15,
			RGB565CONVERT(184,158,131));
	LCD_DrawLine(Xpos - 15, Ypos + 7, Xpos - 15, Ypos + 15,
			RGB565CONVERT(184,158,131));

	LCD_DrawLine(Xpos - 15, Ypos - 15, Xpos - 7, Ypos - 15,
			RGB565CONVERT(184,158,131));
	LCD_DrawLine(Xpos - 15, Ypos - 7, Xpos - 15, Ypos - 15,
			RGB565CONVERT(184,158,131));

	LCD_DrawLine(Xpos + 7, Ypos + 15, Xpos + 15, Ypos + 15,
			RGB565CONVERT(184,158,131));
	LCD_DrawLine(Xpos + 15, Ypos + 7, Xpos + 15, Ypos + 15,
			RGB565CONVERT(184,158,131));

	LCD_DrawLine(Xpos + 7, Ypos - 15, Xpos + 15, Ypos - 15,
			RGB565CONVERT(184,158,131));
	LCD_DrawLine(Xpos + 15, Ypos - 15, Xpos + 15, Ypos - 7,
			RGB565CONVERT(184,158,131));
}

/*******************************************************************************
 * Function Name  : Read_Ads7846
 * Description    : Get TouchPanel X Y
 * Input          : None
 * Output         : None
 * Return         : Coordinate *
 * Attention		 : None
 *******************************************************************************/
Coordinate *Read_Ads7846(void)
{
	static Coordinate screen;
	
	int m0, m1, m2, TP_X[1], TP_Y[1], temp[3];
	uint8_t count = 0;
	int buffer[2][9] = { { 0 }, { 0 } };

	do
	{
		TP_GetAdXY(TP_X, TP_Y);
		
		buffer[0][count] = TP_X[0];
		buffer[1][count] = TP_Y[0];
		count++;
	} while (!Int::read() && count < 9);
	
	if (count == 9) /* Average X Y  */
	{
		/* Average X  */
		temp[0] = (buffer[0][0] + buffer[0][1] + buffer[0][2]) / 3;
		temp[1] = (buffer[0][3] + buffer[0][4] + buffer[0][5]) / 3;
		temp[2] = (buffer[0][6] + buffer[0][7] + buffer[0][8]) / 3;

		m0 = temp[0] - temp[1];
		m1 = temp[1] - temp[2];
		m2 = temp[2] - temp[0];

		m0 = m0 > 0 ? m0 : (-m0);
		m1 = m1 > 0 ? m1 : (-m1);
		m2 = m2 > 0 ? m2 : (-m2);

		if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD)
			return 0;

		if (m0 < m1)
		{
			if (m2 < m0)
				screen.x = (temp[0] + temp[2]) / 2;
			else
				screen.x = (temp[0] + temp[1]) / 2;
		}
		else if (m2 < m1)
			screen.x = (temp[0] + temp[2]) / 2;
		else
			screen.x = (temp[1] + temp[2]) / 2;

		/* Average Y  */
		temp[0] = (buffer[1][0] + buffer[1][1] + buffer[1][2]) / 3;
		temp[1] = (buffer[1][3] + buffer[1][4] + buffer[1][5]) / 3;
		temp[2] = (buffer[1][6] + buffer[1][7] + buffer[1][8]) / 3;
		m0 = temp[0] - temp[1];
		m1 = temp[1] - temp[2];
		m2 = temp[2] - temp[0];
		m0 = m0 > 0 ? m0 : (-m0);
		m1 = m1 > 0 ? m1 : (-m1);
		m2 = m2 > 0 ? m2 : (-m2);
		if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD)
			return 0;

		if (m0 < m1)
		{
			if (m2 < m0)
				screen.y = (temp[0] + temp[2]) / 2;
			else
				screen.y = (temp[0] + temp[1]) / 2;
		}
		else if (m2 < m1)
			screen.y = (temp[0] + temp[2]) / 2;
		else
			screen.y = (temp[1] + temp[2]) / 2;

		return &screen;
	}
	return 0;
}

/*******************************************************************************
 * Function Name  : setCalibrationMatrix
 * Description    : Calculate K A B C D E F
 * Input          : None
 * Output         : None
 * Return         : 
 * Attention		 : None
 *******************************************************************************/
FunctionalState setCalibrationMatrix(Coordinate * display,
		Coordinate * screen, Matrix * matrix)
{

	FunctionalState retTHRESHOLD = ENABLE;
	/* K��(X0��X2) (Y1��Y2)��(X1��X2) (Y0��Y2) */
	matrix->scale = 
			((screen[0].x - screen[2].x) * (screen[1].y - screen[2].y)) -
			((screen[1].x - screen[2].x) * (screen[0].y - screen[2].y));
	
	if (matrix->scale == 0)
	{
		retTHRESHOLD = DISABLE;
	}
	else
	{
		/* A��((XD0��XD2) (Y1��Y2)��(XD1��XD2) (Y0��Y2))��K	*/
		matrix->An =
				((display[0].x - display[2].x) * (screen[1].y - screen[2].y)) -
				((display[1].x - display[2].x) * (screen[0].y - screen[2].y));
		/* B��((X0��X2) (XD1��XD2)��(XD0��XD2) (X1��X2))��K	*/
		matrix->Bn = 
				((screen[0].x - screen[2].x) * (display[1].x - display[2].x)) -
				((display[0].x - display[2].x) * (screen[1].x - screen[2].x));
		/* C��(Y0(X2XD1��X1XD2)+Y1(X0XD2��X2XD0)+Y2(X1XD0��X0XD1))��K */
		matrix->Cn = 
				(screen[2].x * display[1].x - screen[1].x * display[2].x) * screen[0].y +
				(screen[0].x * display[2].x - screen[2].x * display[0].x) * screen[1].y +
				(screen[1].x * display[0].x - screen[0].x * display[1].x) * screen[2].y;
		
		/* D��((YD0��YD2) (Y1��Y2)��(YD1��YD2) (Y0��Y2))��K	*/
		matrix->Dn =
				((display[0].y - display[2].y) * (screen[1].y - screen[2].y)) -
				((display[1].y - display[2].y) * (screen[0].y - screen[2].y));
		/* E��((X0��X2) (YD1��YD2)��(YD0��YD2) (X1��X2))��K	*/
		matrix->En =
				((screen[0].x - screen[2].x) * (display[1].y - display[2].y)) -
				((display[0].y - display[2].y) * (screen[1].x - screen[2].x));
		/* F��(Y0(X2YD1��X1YD2)+Y1(X0YD2��X2YD0)+Y2(X1YD0��X0YD1))��K */
		matrix->Fn =
				(screen[2].x * display[1].y - screen[1].x * display[2].y) * screen[0].y +
				(screen[0].x * display[2].y - screen[2].x * display[0].y) * screen[1].y +
				(screen[1].x * display[0].y - screen[0].x * display[1].y) * screen[2].y;
	}
	return (retTHRESHOLD);
}

/*******************************************************************************
 * Function Name  : getDisplayPoint
 * Description    : Touch panel X Y to display X Y
 * Input          : None
 * Output         : None
 * Return         : 
 * Attention		 : None
 *******************************************************************************/
FunctionalState getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr,
		Matrix * matrixPtr)
{
	FunctionalState retTHRESHOLD = ENABLE;

	if (matrixPtr->scale != 0)
	{
		/* XD = AX+BY+C */
		displayPtr->x =
				((matrixPtr->An * screenPtr->x) +
				 (matrixPtr->Bn * screenPtr->y) + matrixPtr->Cn) / matrixPtr->scale;
		/* YD = DX+EY+F */
		displayPtr->y =
				((matrixPtr->Dn * screenPtr->x)	+
				 (matrixPtr->En * screenPtr->y) + matrixPtr->Fn) / matrixPtr->scale;
	}
	else
	{
		retTHRESHOLD = DISABLE;
	}
	return (retTHRESHOLD);
}

void
TouchPanel_Calibrate(void)
{
	uint8_t i;
	Coordinate * Ptr;

	for (i = 0; i < 3; i++)
	{
		LCD_Clear(Black);
		GUI_Text(44, 10, "Touch crosshair to calibrate", 0xffff, Black);
		xpcc::delay_ms(500);
		DrawCross(DisplaySample[i].x, DisplaySample[i].y);
		do
		{
			Ptr = Read_Ads7846();
		} while (Ptr == 0);
		ScreenSample[i].x = Ptr->x;
		ScreenSample[i].y = Ptr->y;
	}
	setCalibrationMatrix(&DisplaySample[0], &ScreenSample[0], &matrix);
	LCD_Clear(Black);
}
