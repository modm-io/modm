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

///*******************************************************************************
// * Function Name  : setCalibrationMatrix
// * Description    : Calculate K A B C D E F
// * Input          : None
// * Output         : None
// * Return         : 
// * Attention		 : None
// *******************************************************************************/
//FunctionalState setCalibrationMatrix(Coordinate * display,
//		Coordinate * screen, Matrix * matrix)
//{
//
//	FunctionalState retTHRESHOLD = ENABLE;
//	/* K��(X0��X2) (Y1��Y2)��(X1��X2) (Y0��Y2) */
//	matrix->scale = 
//			((screen[0].x - screen[2].x) * (screen[1].y - screen[2].y)) -
//			((screen[1].x - screen[2].x) * (screen[0].y - screen[2].y));
//	
//	if (matrix->scale == 0)
//	{
//		retTHRESHOLD = DISABLE;
//	}
//	else
//	{
//		/* A��((XD0��XD2) (Y1��Y2)��(XD1��XD2) (Y0��Y2))��K	*/
//		matrix->An =
//				((display[0].x - display[2].x) * (screen[1].y - screen[2].y)) -
//				((display[1].x - display[2].x) * (screen[0].y - screen[2].y));
//		/* B��((X0��X2) (XD1��XD2)��(XD0��XD2) (X1��X2))��K	*/
//		matrix->Bn = 
//				((screen[0].x - screen[2].x) * (display[1].x - display[2].x)) -
//				((display[0].x - display[2].x) * (screen[1].x - screen[2].x));
//		/* C��(Y0(X2XD1��X1XD2)+Y1(X0XD2��X2XD0)+Y2(X1XD0��X0XD1))��K */
//		matrix->Cn = 
//				(screen[2].x * display[1].x - screen[1].x * display[2].x) * screen[0].y +
//				(screen[0].x * display[2].x - screen[2].x * display[0].x) * screen[1].y +
//				(screen[1].x * display[0].x - screen[0].x * display[1].x) * screen[2].y;
//		
//		/* D��((YD0��YD2) (Y1��Y2)��(YD1��YD2) (Y0��Y2))��K	*/
//		matrix->Dn =
//				((display[0].y - display[2].y) * (screen[1].y - screen[2].y)) -
//				((display[1].y - display[2].y) * (screen[0].y - screen[2].y));
//		/* E��((X0��X2) (YD1��YD2)��(YD0��YD2) (X1��X2))��K	*/
//		matrix->En =
//				((screen[0].x - screen[2].x) * (display[1].y - display[2].y)) -
//				((display[0].y - display[2].y) * (screen[1].x - screen[2].x));
//		/* F��(Y0(X2YD1��X1YD2)+Y1(X0YD2��X2YD0)+Y2(X1YD0��X0YD1))��K */
//		matrix->Fn =
//				(screen[2].x * display[1].y - screen[1].x * display[2].y) * screen[0].y +
//				(screen[0].x * display[2].y - screen[2].x * display[0].y) * screen[1].y +
//				(screen[1].x * display[0].y - screen[0].x * display[1].y) * screen[2].y;
//	}
//	return (retTHRESHOLD);
//}
//
///*******************************************************************************
// * Function Name  : getDisplayPoint
// * Description    : Touch panel X Y to display X Y
// * Input          : None
// * Output         : None
// * Return         : 
// * Attention		 : None
// *******************************************************************************/
//FunctionalState getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr,
//		Matrix * matrixPtr)
//{
//	FunctionalState retTHRESHOLD = ENABLE;
//
//	if (matrixPtr->scale != 0)
//	{
//		/* XD = AX+BY+C */
//		displayPtr->x =
//				((matrixPtr->An * screenPtr->x) +
//				 (matrixPtr->Bn * screenPtr->y) + matrixPtr->Cn) / matrixPtr->scale;
//		/* YD = DX+EY+F */
//		displayPtr->y =
//				((matrixPtr->Dn * screenPtr->x)	+
//				 (matrixPtr->En * screenPtr->y) + matrixPtr->Fn) / matrixPtr->scale;
//	}
//	else
//	{
//		retTHRESHOLD = DISABLE;
//	}
//	return (retTHRESHOLD);
//}
