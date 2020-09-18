
#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#include <modm/platform/device.hpp>

#define CFG_TUSB_OS OPT_OS_NONE
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

#define CFG_TUSB_MCU OPT_MCU_SAMD21

#define CFG_TUD_ENDOINT0_SIZE 64

#define CFG_TUD_CDC 1

// FIFO size of CDC TX and RX
#define CFG_TUD_CDC_RX_BUFSIZE 64
#define CFG_TUD_CDC_TX_BUFSIZE 64

#endif  // _TUSB_CONFIG_H_