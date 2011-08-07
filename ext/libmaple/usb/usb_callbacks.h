/* insert license */

#ifndef __USB_CALLBACKS
#define __USB_CALLBACKS

#include "../libmaple.h"
#include "usb_lib/usb_lib.h"
#include "usb_config.h"

#define SET_LINE_CODING        0x20
#define GET_LINE_CODING        0x21
#define SET_COMM_FEATURE       0x02
#define SET_CONTROL_LINE_STATE 0x22
#define CONTROL_LINE_DTR       (0x01)
#define CONTROL_LINE_RTS       (0x02)

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
  uint32 bitrate;
  uint8  format;
  uint8  paritytype;
  uint8  datatype;
} USB_Line_Coding;

typedef enum {
  DTR_UNSET,
  DTR_HIGH,
  DTR_NEGEDGE,
  DTR_LOW  
} RESET_STATE;

extern RESET_STATE reset_state;  /* tracks DTR/RTS */
extern uint8       line_dtr_rts;  
extern volatile uint32 countTx;
extern uint8 vcomBufferRx[VCOM_RX_BUFLEN];  /* no reason this has to be VCOM_RX_EPSIZE, could be bigger */
extern volatile uint32 recvBufIn;   /* the FIFO in index to the recvbuffer */
extern volatile uint32 recvBufOut;  /* the FIFO out index to the recvbuffer */
extern volatile uint32 maxNewBytes;
extern volatile uint32 newBytes;

void vcomDataTxCb(void);
void vcomDataRxCb(void);
void vcomManagementCb(void);

uint8* vcomGetSetLineCoding(uint16 length);
void vcomSetLineSate(void);

void usbInit(void);
/* internal functions (as per the usb_core pProperty structure) */
void usbInit(void);
void usbReset(void);
void usbStatusIn(void);
void usbStatusOut(void);

RESULT usbDataSetup(uint8 request);
RESULT usbNoDataSetup(uint8 request);
RESULT usbGetInterfaceSetting(uint8,uint8);

uint8* usbGetDeviceDescriptor(uint16 length);
uint8* usbGetConfigDescriptor(uint16 length);
uint8* usbGetStringDescriptor(uint16 length);

/* internal callbacks to respond to standard requests */
void usbSetConfiguration(void);
void usbSetDeviceAddress(void);

#if defined(__cplusplus)
}
#endif


#endif
