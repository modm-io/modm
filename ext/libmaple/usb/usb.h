/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#ifndef _USB_H_
#define _USB_H_

#include "usb_lib.h"
#include "../libmaple.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RESUME_EXTERNAL,
    RESUME_INTERNAL,
    RESUME_LATER,
    RESUME_WAIT,
    RESUME_START,
    RESUME_ON,
    RESUME_OFF,
    RESUME_ESOF
} RESUME_STATE;

typedef enum {
    UNCONNECTED,
    ATTACHED,
    POWERED,
    SUSPENDED,
    ADDRESSED,
    CONFIGURED
} DEVICE_STATE;

extern volatile uint32 bDeviceState;

void setupUSB(void);
void disableUSB(void);
void usbSuspend(void);
void usbResumeInit(void);
void usbResume(RESUME_STATE);

RESULT usbPowerOn(void);
RESULT usbPowerOff(void);

void usbDsbISR(void);
void usbEnbISR(void);

/* overloaded ISR routine, this is the main usb ISR */
void __irq_usb_lp_can_rx0(void);
void usbWaitReset(void);

/* blocking functions for send/receive */
void   usbBlockingSendByte(char ch);
uint32 usbSendBytes(const uint8* sendBuf,uint32 len);
uint32 usbBytesAvailable(void);
uint32 usbReceiveBytes(uint8* recvBuf, uint32 len);
uint8 usbGetDTR(void);
uint8 usbGetRTS(void);
uint8 usbIsConnected(void);
uint8 usbIsConfigured(void);
uint16 usbGetPending(void);

void usbSendHello(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _USB_H_
