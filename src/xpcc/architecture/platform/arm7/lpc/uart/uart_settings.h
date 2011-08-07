
#ifndef UART_SETTINGS_H
#define UART_SETTINGS_H

#include "lpc24xx_registers.h"

/** Setup for UART-Interface ********************/
/* Settings for 250000 Baud (at 72 MHz)
 * UxPCLK			setCCLKD4
 * UxDLLVAL  		0x03
 * UxDLMVAL  		0x00
 * UxDIVMUL  		0x21
 * 
 * Settings for 115200 Baud (at 72 MHz):
 * UxPCLK			setCCLKD4
 * UxDLLVAL  		0x08
 * UxDLMVAL  		0x00
 * UxDIVMUL  		0x92
 * 
 * Settings for 57600 Baud (at 72 MHz):
 * UxPCLK			setCCLKD4
 * UxDLLVAL  		0x10
 * UxDLMVAL  		0x00
 * UxDIVMUL  		0x92
 * 
 * Settings for 9600 Baud (at 72 MHz):
 * UxPCLK			setCCLKD4
 * UxDLLVAL  		0x5F
 * UxDLMVAL  		0x00
 * UxDIVMUL  		0xD3
 * 
 * General (high error):
 * U0DLL =  ((CCLK * 1000000 / (BAUD*16))/(1+(2/9))) & 0xFF;
 * UxDLM = (((CCLK * 1000000 / (BAUD*16))/(1+(2/9))) & 0xFF00)>>8;
 * UxDIVMUL  		0x92
 */
/** Setup for UART0 *****************************/
// => Debug interface (115200 Baud)
#define U0PCLK		setCCLKD4	// pclk = CCLK/4 = 18 MHz
#define U0DIVMUL	0x92 		// mulval & divaddval (Frac=0.22 s.b.)
#define U0DLLVAL	8			// Untere 8 Bit des Teilers
#define U0DLMVAL	0			// Obere 8 Bit des Teilers
#define U0MODE		3			// 8N1

/** Setup for UART1 *****************************/
// => MWS (250 kBaud)
#define U1PCLK		setCCLKD4	// pclk = CCLK/4 = 18 MHz
#define U1DIVMUL	0x21 		// mulval & divaddval (Frac=0.22 s.b.)
#define U1DLLVAL	3			// Untere 8 Bit des Teilers
#define U1DLMVAL	0			// Obere 8 Bit des Teilers
#define U1MODE		3			// 8N1

/** Setup for UART2 *****************************/
// => MWS (250 kBaud)
#define U2PCLK		setCCLKD4	// pclk = CCLK/4 = 18 MHz
#define U2DIVMUL	0x21 		// mulval & divaddval (Frac=0.22 s.b.)
#define U2DLLVAL	3			// Untere 8 Bit des Teilers
#define U2DLMVAL	0			// Obere 8 Bit des Teilers
#define U2MODE		3			// 8N1

/** Setup for UART3 *****************************/
// not used
#define U3PCLK		setCCLKD4	// pclk = CCLK/4 = 18 MHz
#define U3DIVMUL	0x92 		// mulval & divaddval (Frac=0.22 s.b.)
#define U3DLLVAL	8			// Untere 8 Bit des Teilers
#define U3DLMVAL	0			// Obere 8 Bit des Teilers
#define U3MODE		3			// 8N1

#endif // UART_SETTINGS_H
