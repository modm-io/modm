/***********************************************************************
 * $Id::                                                               $
 *
 *     Copyright (C) 2010 NXP Semiconductors.
 *
 * Description:
 *     Debug I/O routines for LPCXpresso projects. Uses semihosting to
 *     send printf output to a window in the debugger on the PC. Calls
 *     LGPL printf routine in lib_small_printf_m0 library.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED
#if CONFIG_ENABLE_DRIVER_PRINTF==1
#include <stdint.h>
#include "small_printf.h"

#ifndef DEBUG
#define debug_printf(...)
#define debug_printf_flush()
#define debug_putchar(c)
#define debug_getchar()
#define debug_puts(s)
extern char *non_debug_input_buf[1];
static inline char *_debug_getstr(int *len)
{
    if(len)*len=0;
    non_debug_input_buf[0] = 0;
    return (char *)non_debug_input_buf;
}
#define debug_getstr(len)       _debug_getstr(len)
#define ISDEBUGACTIVE()         0
#else
#define debug_printf(...)       _debug_printf(__VA_ARGS__)
#define debug_printf_flush()    _debug_printf_flush()
#define debug_putchar(c)        _debug_putchar(c)
#define debug_getchar()         __readc(void)
#define debug_getstr(len)       _debug_getstr(len)
#define debug_puts(s)			_debug_puts(s)

// This feature does not work in LPCXpresso on the LPC111x yet
//#define ISDEBUGACTIVE()         (CoreDebug->DHCSR&CoreDebug_DHCSR_C_DEBUGEN_Msk)
#define ISDEBUGACTIVE()				1

void _debug_printf(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
int _debug_printf_flush();
int _debug_putchar (char c);
char *_debug_getstr(int *len);
void _debug_puts(const char *s);

#if CONFIG_DRIVER_PRINTF_REDLIBV2==1
extern int __sys_write(int zero, const char *s, int len);
extern int __sys_readc(void);
#else
extern int __write(int zero, const char *s, int len);
extern int __read(int zero, char *s, int len);
extern int __readc(void);

#define DEBUG_OUTPUT_BUFFER_SIZE 82

#define FLUSH_ON_NL
#endif

#define DEBUG_INPUT_BUFFER_SIZE 22

#endif

#endif
// DEBUG_H_INCLUDED
#endif
