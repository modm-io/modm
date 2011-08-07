/* insert license */

#include "usb_callbacks.h"
#include "usb_lib/usb_lib.h"
#include "descriptors.h"
#include "usb_config.h"
#include "usb.h"
#include "usb_hardware.h"

ONE_DESCRIPTOR Device_Descriptor = {
    (uint8*)&usbVcomDescriptor_Device,
    sizeof(USB_Descriptor_Device)
};

ONE_DESCRIPTOR Config_Descriptor = {
    (uint8*)&usbVcomDescriptor_Config,
    0x43//sizeof(USB_Descriptor_Config)
};

ONE_DESCRIPTOR String_Descriptor[3] = {
    {(uint8*)&usbVcomDescriptor_LangID,       USB_DESCRIPTOR_STRING_LEN(1)},
    {(uint8*)&usbVcomDescriptor_iManufacturer,USB_DESCRIPTOR_STRING_LEN(8)},
    {(uint8*)&usbVcomDescriptor_iProduct,     USB_DESCRIPTOR_STRING_LEN(8)}
};

uint8 last_request = 0;

USB_Line_Coding line_coding = {
 bitrate:     115200,
 format:      0x00, /* stop bits-1 */
 paritytype:  0x00,
 datatype:    0x08
};

uint8 vcomBufferRx[VCOM_RX_BUFLEN];
volatile uint32 countTx    = 0;
volatile uint32 recvBufIn  = 0;
volatile uint32 recvBufOut = 0;
volatile uint32 maxNewBytes   = VCOM_RX_BUFLEN;
volatile uint32 newBytes = 0;
RESET_STATE reset_state = DTR_UNSET;
uint8       line_dtr_rts = 0;

void vcomDataTxCb(void) {
    /* do whatever after data has been sent to host */

    /* allows usbSendBytes to stop blocking */

    /* assumes tx transactions are atomic 64 bytes (nearly certain they are) */
    countTx = 0;
}

/* we could get arbitrarily complicated here for speed purposes
   however, the simple scheme here is to implement a receive fifo
   and always set the maximum to new bytes to the space remaining
   in the fifo. this number will be reincremented after calls
   to usbReceiveBytes */
void vcomDataRxCb(void) {
  /* do whatever after data has been received from host */

  /* setEPRxCount on the previous cycle should garuntee
     we havnt received more bytes than we can fit */
  newBytes = GetEPRxCount(VCOM_RX_ENDP);
  SetEPRxStatus(VCOM_RX_ENDP,EP_RX_NAK);

  /* todo, not checking very carefully for edge cases. USUALLY,
     if we emit the reset pulse and send 4 bytes, then newBytes
     should be 4. But its POSSIBLE that this would be violated
     in some cases */

  /* magic number, {0x31, 0x45, 0x41, 0x46} is "1EAF" */
  uint8 chkBuf[4];
  uint8 cmpBuf[4] = {0x31, 0x45, 0x41, 0x46};
  if (reset_state == DTR_NEGEDGE) {
    reset_state = DTR_LOW;

    if  (newBytes >= 4) {
      unsigned int target = (unsigned int)usbWaitReset | 0x1;

      PMAToUserBufferCopy(chkBuf,VCOM_RX_ADDR,4);

      int i;
      USB_Bool cmpMatch = TRUE;
      for (i=0; i<4; i++) {
          if (chkBuf[i] != cmpBuf[i]) {
              cmpMatch = FALSE;
          }
      }

      if (cmpMatch) {
          asm volatile("mov r0, %[stack_top]      \n\t"             // Reset the stack
                       "mov sp, r0                \n\t"
                       "mov r0, #1                \n\t"
                       "mov r1, %[target_addr]    \n\t"
                       "mov r2, %[cpsr]           \n\t"
                       "push {r2}                 \n\t"             // Fake xPSR
                       "push {r1}                 \n\t"             // Target address for PC
                       "push {r0}                 \n\t"             // Fake LR
                       "push {r0}                 \n\t"             // Fake R12
                       "push {r0}                 \n\t"             // Fake R3
                       "push {r0}                 \n\t"             // Fake R2
                       "push {r0}                 \n\t"             // Fake R1
                       "push {r0}                 \n\t"             // Fake R0
                       "mov lr, %[exc_return]     \n\t"
                       "bx lr"
                       :
                       : [stack_top] "r" (STACK_TOP),
                         [target_addr] "r" (target),
                         [exc_return] "r" (EXC_RETURN),
                         [cpsr] "r" (DEFAULT_CPSR)
                       : "r0", "r1", "r2");
          /* should never get here */
      }
    }
  }

  PMAToUserBufferCopy(&vcomBufferRx[0],VCOM_RX_ADDR,newBytes);
}

void vcomManagementCb(void) {
    /* unused. This enpoint would callback if we had sent a linestate
       changed notification */
}

u8* vcomGetSetLineCoding(uint16 length) {
    if (length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_Line_Coding);
    }
    return (uint8*)&line_coding;
}

void vcomSetLineState(void) {
}

void usbInit(void) {
    pInformation->Current_Configuration = 0;
    usbPowerOn();

    _SetISTR(0);
    wInterrupt_Mask = ISR_MSK;
    _SetCNTR(wInterrupt_Mask);

    usbEnbISR();
    bDeviceState = UNCONNECTED;
}

void usbReset(void) {
    pInformation->Current_Configuration = 0;

    /* current feature is current bmAttributes */
    pInformation->Current_Feature = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELF_POWERED);

    _SetBTABLE(USB_BTABLE_ADDRESS);

    /* setup control endpoint 0 */
    _SetEPType(ENDP0, EP_CONTROL);
    _SetEPTxStatus(ENDP0, EP_TX_STALL);
    _SetEPRxAddr(ENDP0,VCOM_CTRL_RX_ADDR);
    _SetEPTxAddr(ENDP0,VCOM_CTRL_TX_ADDR);
    Clear_Status_Out(ENDP0);

    SetEPRxCount(ENDP0, pProperty->MaxPacketSize);
    SetEPRxValid(ENDP0);

    /* setup management endpoint 1  */
    SetEPType     (VCOM_NOTIFICATION_ENDP,     EP_INTERRUPT);
    SetEPTxAddr   (VCOM_NOTIFICATION_ENDP,     VCOM_NOTIFICATION_ADDR);
    SetEPTxStatus (VCOM_NOTIFICATION_ENDP,     EP_TX_NAK);
    SetEPRxStatus (VCOM_NOTIFICATION_ENDP,     EP_RX_DIS);

    /* setup data endpoint OUT (rx) */
    /*   SetEPType     (VCOM_RX_ENDP, EP_BULK); */
    /*   SetEPRxAddr   (VCOM_RX_ENDP, VCOM_RX_ADDR); */
    /*   SetEPRxCount  (VCOM_RX_ENDP, VCOM_RX_EPSIZE); */
    /*   // SetEPTxStatus (VCOM_RX_ENDP, EP_TX_DIS); */
    /*   SetEPRxStatus (VCOM_RX_ENDP, EP_RX_VALID); */

    SetEPType     (3, EP_BULK);
    SetEPRxAddr   (3, 0x110);
    SetEPRxCount  (3,64);
    // SetEPTxStatus (VCOM_RX_ENDP, EP_TX_DIS);
    SetEPRxStatus (3, EP_RX_VALID);

    /* setup data endpoint IN (tx)  */
    SetEPType     (VCOM_TX_ENDP, EP_BULK);
    SetEPTxAddr   (VCOM_TX_ENDP, VCOM_TX_ADDR);
    SetEPTxStatus (VCOM_TX_ENDP, EP_TX_NAK);
    SetEPRxStatus (VCOM_TX_ENDP, EP_RX_DIS);

    bDeviceState = ATTACHED;
    SetDeviceAddress(0);

    /* reset the rx fifo */
    recvBufIn   = 0;
    recvBufOut  = 0;
    maxNewBytes = VCOM_RX_EPSIZE;
    countTx     = 0;
}


void usbStatusIn(void) {
    /* adjust the usart line coding
       if we wish to couple the CDC line coding
       with the real usart port */
}

void usbStatusOut(void) {
}

RESULT usbDataSetup(uint8 request) {
    uint8 *(*CopyRoutine)(uint16);
    CopyRoutine = NULL;

    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        switch (request) {
        case (GET_LINE_CODING):
            CopyRoutine = vcomGetSetLineCoding;
            last_request = GET_LINE_CODING;
            break;
        case (SET_LINE_CODING):
            CopyRoutine = vcomGetSetLineCoding;
            last_request = SET_LINE_CODING;
            break;
        default: break;
        }
    }

    if (CopyRoutine == NULL) {
        return USB_UNSUPPORT;
    }

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return USB_SUCCESS;
}

RESULT usbNoDataSetup(u8 request) {
    uint8 new_signal;

    /* we support set com feature but dont handle it */
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {

        switch (request) {
        case (SET_COMM_FEATURE):
            return USB_SUCCESS;
        case (SET_CONTROL_LINE_STATE):
            /* to reset the board, pull both dtr and rts low
               then pulse dtr by itself */
            new_signal = pInformation->USBwValues.bw.bb0 & (CONTROL_LINE_DTR | CONTROL_LINE_RTS);
            line_dtr_rts = new_signal & 0x03;

            switch (reset_state) {
                /* no default, covered enum */
            case DTR_UNSET:
                if ((new_signal & CONTROL_LINE_DTR) == 0 ) {
                    reset_state = DTR_LOW;
                } else {
                    reset_state = DTR_HIGH;
                }
                break;

            case DTR_HIGH:
                if ((new_signal & CONTROL_LINE_DTR) == 0 ) {
                    reset_state = DTR_NEGEDGE;
                } else {
                    reset_state = DTR_HIGH;
                }
                break;

            case DTR_NEGEDGE:
                if ((new_signal & CONTROL_LINE_DTR) == 0 ) {
                    reset_state = DTR_LOW;
                } else {
                    reset_state = DTR_HIGH;
                }
                break;

            case DTR_LOW:
                if ((new_signal & CONTROL_LINE_DTR) == 0 ) {
                    reset_state = DTR_LOW;
                } else {
                    reset_state = DTR_HIGH;
                }
                break;
            }

            return USB_SUCCESS;
        }
    }
    return USB_UNSUPPORT;
}

RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting) {
    if (alt_setting > 0) {
        return USB_UNSUPPORT;
    } else if (interface > 1) {
        return USB_UNSUPPORT;
    }

    return USB_SUCCESS;
}


u8* usbGetDeviceDescriptor(u16 length) {
    return Standard_GetDescriptorData(length, &Device_Descriptor);
}

u8* usbGetConfigDescriptor(u16 length) {
    return Standard_GetDescriptorData(length, &Config_Descriptor);
}

u8* usbGetStringDescriptor(u16 length) {
    uint8 wValue0 = pInformation->USBwValue0;

    if (wValue0 > 2) {
        return NULL;
    }
    return Standard_GetDescriptorData(length, &String_Descriptor[wValue0]);
}

/* internal callbacks to respond to standard requests */
void usbSetConfiguration(void) {
    if (pInformation->Current_Configuration != 0) {
        bDeviceState = CONFIGURED;
    }
}

void usbSetDeviceAddress(void) {
    bDeviceState = ADDRESSED;
}

