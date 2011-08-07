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

/**
 *  @brief usb-specific hardware setup, NVIC, clocks, and usb activities
 *  in the pre-attached state. includes some of the lower level callbacks
 *  needed by the usb library, like suspend,resume,init,etc
 */

#include "usb.h"
#include "libmaple.h"
#include "usb_lib.h"
#include "gpio.h"
#include "usb_hardware.h"
#include "delay.h"

#include "usb_config.h"
#include "usb_callbacks.h"
#include "usb_lib.h"

/* persistent usb structs */

volatile uint32 bDeviceState = UNCONNECTED;
volatile uint16 wIstr = 0;
volatile uint32 bIntPackSOF = 0;

DEVICE Device_Table =
    {NUM_ENDPTS,
     1};

DEVICE_PROP Device_Property =
    {usbInit,
     usbReset,
     usbStatusIn,
     usbStatusOut,
     usbDataSetup,
     usbNoDataSetup,
     usbGetInterfaceSetting,
     usbGetDeviceDescriptor,
     usbGetConfigDescriptor,
     usbGetStringDescriptor,
     0,
     bMaxPacketSize};

USER_STANDARD_REQUESTS User_Standard_Requests =
    {NOP_Process,
     usbSetConfiguration,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     usbSetDeviceAddress};

void (*pEpInt_IN[7])(void) =
    {vcomDataTxCb,
     vcomManagementCb,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process};

void (*pEpInt_OUT[7])(void) =
    {NOP_Process,
     NOP_Process,
     vcomDataRxCb,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process};

struct {
  volatile RESUME_STATE eState;
  volatile uint8 bESOFcnt;
} ResumeS;

void setupUSB (void) {
  gpio_set_mode(USB_DISC_DEV, USB_DISC_PIN, GPIO_OUTPUT_PP);

  /* setup the apb1 clock for USB */
  pRCC->APB1ENR |= 0x00800000;

  /* initialize the usb application */
  gpio_write_bit(USB_DISC_DEV, USB_DISC_PIN, 0); // presents us to the host
  USB_Init();  // low level init routine provided by the ST library
}

void disableUSB (void) {
  // These are just guesses about how to do this
  // TODO: real disable function
  usbDsbISR();
  gpio_write_bit(USB_DISC_DEV, USB_DISC_PIN, 1);
}

void usbSuspend(void) {
  u16 wCNTR;
  wCNTR = _GetCNTR();
  wCNTR |= CNTR_FSUSP | CNTR_LPMODE;
  _SetCNTR(wCNTR);

  /* run any power reduction handlers */
  bDeviceState = SUSPENDED;
}

void usbResumeInit(void) {
  u16 wCNTR;

  /* restart any clocks that had been stopped */

  wCNTR = _GetCNTR();
  wCNTR &= (~CNTR_LPMODE);
  _SetCNTR(wCNTR);

  /* undo power reduction handlers here */
  _SetCNTR(ISR_MSK);

}

void usbResume(RESUME_STATE eResumeSetVal) {
  u16 wCNTR;

  if (eResumeSetVal != RESUME_ESOF)
    ResumeS.eState = eResumeSetVal;

  switch (ResumeS.eState)
    {
    case RESUME_EXTERNAL:
      usbResumeInit();
      ResumeS.eState = RESUME_OFF;
      break;
    case RESUME_INTERNAL:
      usbResumeInit();
      ResumeS.eState = RESUME_START;
      break;
    case RESUME_LATER:
      ResumeS.bESOFcnt = 2;
      ResumeS.eState = RESUME_WAIT;
      break;
    case RESUME_WAIT:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
        ResumeS.eState = RESUME_START;
      break;
    case RESUME_START:
      wCNTR = _GetCNTR();
      wCNTR |= CNTR_RESUME;
      _SetCNTR(wCNTR);
      ResumeS.eState = RESUME_ON;
      ResumeS.bESOFcnt = 10;
      break;
    case RESUME_ON:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0) {
          wCNTR = _GetCNTR();
          wCNTR &= (~CNTR_RESUME);
          _SetCNTR(wCNTR);
          ResumeS.eState = RESUME_OFF;
      }
      break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
      ResumeS.eState = RESUME_OFF;
      break;
    }
}

RESULT usbPowerOn(void) {
  u16 wRegVal;

  wRegVal = CNTR_FRES;
  _SetCNTR(wRegVal);

  wInterrupt_Mask = 0;
  _SetCNTR(wInterrupt_Mask);
  _SetISTR(0);
  wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM; // the bare minimum
  _SetCNTR(wInterrupt_Mask);

  return USB_SUCCESS;
}

RESULT usbPowerOff(void) {
  _SetCNTR(CNTR_FRES);
  _SetISTR(0);
  _SetCNTR(CNTR_FRES + CNTR_PDWN);

  /* note that all weve done here is powerdown the
     usb peripheral. we have no disabled the clocks,
     pulled the usb_disc pin back up, or reset the
     application state machines */

  return USB_SUCCESS;
}


// These two functions (usbEnbISR/usbDsbISR) are implementented in ST style,
// and at least the DsbISR doesn't seem to work?
void usbEnbISR(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = TRUE;
  nvicInit(&NVIC_InitStructure);
}

void usbDsbISR(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = FALSE;
  nvicInit(&NVIC_InitStructure);
}

/* overloaded ISR routine, this is the main usb ISR */
void __irq_usb_lp_can_rx0(void) {
wIstr = _GetISTR();

/* go nuts with the preproc switches since this is an ISTR and must be FAST */
#if (ISR_MSK & ISTR_RESET)
if (wIstr & ISTR_RESET & wInterrupt_Mask)
  {
    _SetISTR((u16)CLR_RESET);
    Device_Property.Reset();
  }
#endif


#if (ISR_MSK & ISTR_DOVR)
if (wIstr & ISTR_DOVR & wInterrupt_Mask)
  {
    _SetISTR((u16)CLR_DOVR);
  }
#endif


#if (ISR_MSK & ISTR_ERR)
if (wIstr & ISTR_ERR & wInterrupt_Mask)
  {
    _SetISTR((u16)CLR_ERR);
  }
#endif


#if (ISR_MSK & ISTR_WKUP)
if (wIstr & ISTR_WKUP & wInterrupt_Mask) {
    _SetISTR((u16)CLR_WKUP);
    usbResume(RESUME_EXTERNAL);
}
#endif

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (ISR_MSK & ISTR_SUSP)
if (wIstr & ISTR_SUSP & wInterrupt_Mask) {
    /* check if SUSPEND is possible */
    if (F_SUSPEND_ENABLED) {
        usbSuspend();
    } else {
        /* if not possible then resume after xx ms */
        usbResume(RESUME_LATER);
    }
    /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
    _SetISTR((u16)CLR_SUSP);
}
#endif


#if (ISR_MSK & ISTR_SOF)
if (wIstr & ISTR_SOF & wInterrupt_Mask) {
    _SetISTR((u16)CLR_SOF);
    bIntPackSOF++;
 }
#endif


#if (ISR_MSK & ISTR_ESOF)
if (wIstr & ISTR_ESOF & wInterrupt_Mask) {
    _SetISTR((u16)CLR_ESOF);
    /* resume handling timing is made with ESOFs */
    usbResume(RESUME_ESOF); /* request without change of the machine state */
 }
#endif

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (ISR_MSK & ISTR_CTR)
if (wIstr & ISTR_CTR & wInterrupt_Mask) {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    CTR_LP(); /* low priority ISR defined in the usb core lib */
 }
#endif

}

void usbWaitReset(void) {
  delay_us(RESET_DELAY);
  systemHardReset();
}

/* This low-level send bytes function is NON-BLOCKING; blocking behavior, with
 * a timeout, is implemented in usercode (or in the Wirish C++ high level
 * implementation).
 *
 * This function will quickly copy up to 64 bytes of data (out of an
 * arbitrarily large buffer) into the USB peripheral TX buffer and return the
 * number placed in that buffer. It is up to usercode to divide larger packets
 * into 64-byte chunks to guarantee delivery.
 *
 *
 */
void usbBlockingSendByte(char ch) {
    while (countTx);
    UserToPMABufferCopy((uint8*)&ch,VCOM_TX_ADDR,1);
    _SetEPTxCount(VCOM_TX_ENDP,1);
    _SetEPTxValid(VCOM_TX_ENDP);
    countTx = 1;
    while (countTx);
}

uint32 usbSendBytes(const uint8* sendBuf, uint32 len) {
  /* Last transmission hasn't finished, abort */
  if (countTx) {
    return 0;
  }

  // We can only put VCOM_TX_EPSIZE bytes in the buffer
  if (len > VCOM_TX_EPSIZE / 2) {
    len = VCOM_TX_EPSIZE / 2;
  }

  // Try to load some bytes if we can
  if (len) {
    UserToPMABufferCopy(sendBuf, VCOM_TX_ADDR, len);
    _SetEPTxCount(VCOM_TX_ENDP, len);
    countTx += len;
    _SetEPTxValid(VCOM_TX_ENDP);
  }

  return len;
}

/* returns the number of available bytes are in the recv FIFO */
uint32 usbBytesAvailable(void) {
  return newBytes;
}

/* copies len bytes from the local recieve FIFO (not
   usb packet buffer) into recvBuf and deq's the fifo.
   will only copy the minimum of len or the available
   bytes. returns the number of bytes copied */
uint32 usbReceiveBytes(uint8* recvBuf, uint32 len) {
  if (len > newBytes) {
      len = newBytes;
  }

  int i;
  for (i=0;i<len;i++) {
    recvBuf[i] = (uint8)(vcomBufferRx[i]);
  }

  newBytes -= len;

  /* re-enable the rx endpoint which we had set to receive 0 bytes */
  if (newBytes == 0) {
    SetEPRxCount(VCOM_RX_ENDP,VCOM_RX_EPSIZE);
    SetEPRxStatus(VCOM_RX_ENDP,EP_RX_VALID);
  }

  return len;
}

void usbSendHello(void) {
  uint8 bufin = 48 + recvBufIn;;
  uint8 bufout = 48 + recvBufOut;
  uint8 avail  = 48 + usbBytesAvailable();

  char *line = "\r\n";
  while(usbSendBytes(&bufin,1) == 0);
  while(usbSendBytes(&bufout,1) == 0);
  while(usbSendBytes(&avail,1) == 0);
  while(usbSendBytes((uint8*)line,2) == 0);

  uint8 recv[64];
  usbReceiveBytes(&recv[0],1);
}

uint8 usbGetDTR() {
  return ((line_dtr_rts & CONTROL_LINE_DTR) != 0);
}

uint8 usbGetRTS() {
  return ((line_dtr_rts & CONTROL_LINE_RTS) != 0);
}

uint8 usbIsConfigured() {
  return (bDeviceState == CONFIGURED);
}

uint8 usbIsConnected() {
  return (bDeviceState != UNCONNECTED);
}

uint16 usbGetPending() {
  return countTx;
}

