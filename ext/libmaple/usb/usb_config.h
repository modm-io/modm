/* insert license */

#ifndef __USB_CONFIG_H
#define __USB_CONFIG_H

#include "usb_lib/usb_lib.h"
#include "../gpio.h"

/******************************************************************************
 ******************************************************************************
 ***
 ***   HACK ALERT
 ***
 ***   FIXME FIXME FIXME FIXME
 ***
 ***   A bunch of board-specific #defines that are only used by the
 ***   USB routines got put into libmaple.h for what appear to be
 ***   historical reasons.  I'm [mbolivar] putting them in here for
 ***   now, so that we can treat the usb/ directory as a black box,
 ***   freeing the rest of libmaple/ to be implemented as a
 ***   general-purpose STM32 library. All of this REALLY needs to get
 ***   moved into wirish when we get a chance to redo the USB stack.
 ***
 ******************************************************************************
 *****************************************************************************/

#define VCOM_ID_VENDOR            0x1EAF
#define RESET_DELAY               (100000)
#define USB_CONFIG_MAX_POWER      (100 >> 1)

#if defined(BOARD_maple) || defined(BOARD_maple_RET6)

    /* USB Identifier numbers */
    #define VCOM_ID_PRODUCT      0x0004
    #define USB_DISC_DEV         GPIOC
    #define USB_DISC_PIN         12

#elif defined(BOARD_maple_mini)

    #define VCOM_ID_PRODUCT      0x0004
    #define USB_DISC_DEV         GPIOB
    #define USB_DISC_PIN         9

#elif defined(BOARD_maple_native)

    #define VCOM_ID_PRODUCT      0x0004
    #define USB_DISC_DEV         GPIOB
    #define USB_DISC_PIN         8

#else

#error ("Sorry! the USB stack relies on LeafLabs board-specific "       \
        "configuration right now.  If you want, you can pretend you're one " \
        "of our boards; i.e., #define BOARD_maple, BOARD_maple_mini, or " \
        "BOARD_maple_native according to what matches your MCU best. "  \
        "You should also take a look at libmaple/usb/descriptors.c; we make " \
        "some assumptions there that you probably won't like.")

#endif

/******************************************************************************
 ******************************************************************************
 ***
 ***   END HACK
 ***
 ******************************************************************************
 *****************************************************************************/


/* choose addresses to give endpoints the max 64 byte buffers */
#define USB_BTABLE_ADDRESS        0x00
#define VCOM_CTRL_EPNUM           0x00
#define VCOM_CTRL_RX_ADDR         0x40
#define VCOM_CTRL_TX_ADDR         0x80
#define VCOM_CTRL_EPSIZE          0x40

#define VCOM_TX_ENDP              ENDP1
#define VCOM_TX_EPNUM             0x01
#define VCOM_TX_ADDR              0xC0
#define VCOM_TX_EPSIZE            0x40

#define VCOM_NOTIFICATION_ENDP    ENDP2
#define VCOM_NOTIFICATION_EPNUM   0x02
#define VCOM_NOTIFICATION_ADDR    0x100
#define VCOM_NOTIFICATION_EPSIZE  0x40

#define VCOM_RX_ENDP              ENDP3
#define VCOM_RX_EPNUM             0x03
#define VCOM_RX_ADDR              0x110
#define VCOM_RX_EPSIZE            0x40
#define VCOM_RX_BUFLEN            (VCOM_RX_EPSIZE*3)

#define bMaxPacketSize            0x40  /* 64B, maximum for USB FS Devices */

#define NUM_ENDPTS                0x04

/* handle all usb interrupts */
#define ISR_MSK (CNTR_CTRM    |                                         \
                 CNTR_WKUPM   |                                         \
                 CNTR_SUSPM   |                                         \
                 CNTR_ERRM    |                                         \
                 CNTR_SOFM    |                                         \
                 CNTR_ESOFM   |                                         \
                 CNTR_RESETM)

#define F_SUSPEND_ENABLED 1


#endif
