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

#include "descriptors.h"
#include "libmaple.h"
#include "usb_config.h"

const USB_Descriptor_Device usbVcomDescriptor_Device = {
 bLength:              sizeof(USB_Descriptor_Device),
 bDescriptorType:      USB_DESCRIPTOR_TYPE_DEVICE,
 bcdUSB:               0x0200,
 bDeviceClass:         USB_DEVICE_CLASS_CDC,
 bDeviceSubClass:      USB_DEVICE_SUBCLASS_CDC,
 bDeviceProtocol:      0x00,
 bMaxPacketSize0:      0x40,
 idVendor:             VCOM_ID_VENDOR,
 idProduct:            VCOM_ID_PRODUCT,
 bcdDevice:            0x0200,
 iManufacturer:        0x01,
 iProduct:             0x02,
 iSerialNumber:        0x00,
 bNumConfigurations:   0x01
};

const USB_Descriptor_Config usbVcomDescriptor_Config = {
 bLength:                   0x09,//sizeof(USB_Descriptor_Config_Header),
 bDescriptorType:           USB_DESCRIPTOR_TYPE_CONFIGURATION,
 wTotalLength:              0x43,//sizeof(USB_Descriptor_Config),
 bNumInterfaces:            0x02,
 bConfigurationValue:       0x01,
 iConfiguration:            0x00,
 bmAttributes:              (USB_CONFIG_ATTR_BUSPOWERED |
                             USB_CONFIG_ATTR_SELF_POWERED),
 bMaxPower:                 USB_CONFIG_MAX_POWER,

 CCI_Interface:
   {
   bLength:                  0x09,//sizeof(USB_Descriptor_Interface),
   bDescriptorType:          USB_DESCRIPTOR_TYPE_INTERFACE,
   bInterfaceNumber:         0x00,
   bAlternateSetting:        0x00,
   bNumEndpoints:            0x01,
   bInterfaceClass:          0x02,
   bInterfaceSubClass:       0x02,
   bInterfaceProtocol:       0x01,
   iInterface:               0x00
   },

 CDC_Functional_IntHeader:
   {
   bLength:                  0x05,//sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)),
   bDescriptorType:          0x24,
   SubType:                  0x00,
   Data:                     {0x01, 0x10}
   },

 CDC_Functional_CallManagement:
   {
   bLength:                  0x05,//sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)),
   bDescriptorType:          0x24,
   SubType:                  0x01,
   Data:                     {0x03, 0x01}
   },

 CDC_Functional_ACM:
   {
   bLength:                  0x04,//sizeof(CDC_FUNCTIONAL_DESCRIPTOR(1)),
   bDescriptorType:          0x24,
   SubType:                  0x02,
   Data:                     {0x06}
   },

 CDC_Functional_Union:
   {
   bLength:                  0x05,//sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)),
   bDescriptorType:          0x24,
   SubType:                  0x06,
   Data:                     {0x00, 0x01}
   },

 // ManagementEndpoint:
 //  {
 EP1_bLength:                 0x07,//sizeof(USB_Descriptor_Endpoint),
 EP1_bDescriptorType:         USB_DESCRIPTOR_TYPE_ENDPOINT,
 EP1_bEndpointAddress:        (USB_DESCRIPTOR_ENDPOINT_IN | VCOM_NOTIFICATION_EPNUM),
 EP1_bmAttributes:            EP_TYPE_INTERRUPT,
 EP1_wMaxPacketSize0:         VCOM_NOTIFICATION_EPSIZE,
 EP1_wMaxPacketSize1:         0x00,
 EP1_bInterval:               0xFF,
 //  },

 // DCI_Interface:
 //   {
   DCI_bLength:                0x09,//sizeof(USB_Descriptor_Interface),
   DCI_bDescriptorType:        USB_DESCRIPTOR_TYPE_INTERFACE,
   DCI_bInterfaceNumber:       0x01,
   DCI_bAlternateSetting:      0x00,
   DCI_bNumEndpoints:          0x02,
   DCI_bInterfaceClass:        0x0A,
   DCI_bInterfaceSubClass:     0x00,
   DCI_bInterfaceProtocol:     0x00,
   DCI_iInterface:             0x00,
   //   },

 //DataOutEndpoint:
 // {
 //  },
 EP2_bLength:               0x07,//sizeof(USB_Descriptor_Endpoint),
 EP2_bDescriptorType:       USB_DESCRIPTOR_TYPE_ENDPOINT,
 EP2_bEndpointAddress:      (USB_DESCRIPTOR_ENDPOINT_OUT | VCOM_RX_EPNUM),
 EP2_bmAttributes:          EP_TYPE_BULK,
 EP2_wMaxPacketSize0:       VCOM_RX_EPSIZE,
 EP2_wMaxPacketSize1:       0x00,
 EP2_bInterval:             0x00,


 // DataInEndpoint:
 // {
 EP3_bLength:               0x07,//sizeof(USB_Descriptor_Endpoint),
 EP3_bDescriptorType:       USB_DESCRIPTOR_TYPE_ENDPOINT,
 EP3_bEndpointAddress:      (USB_DESCRIPTOR_ENDPOINT_IN | VCOM_TX_EPNUM),
 EP3_bmAttributes:          EP_TYPE_BULK,
 EP3_wMaxPacketSize0:       VCOM_TX_EPSIZE,
 EP3_wMaxPacketSize1:       0x00,
 EP3_bInterval:             0x00

 //  }
};

/*****************************************************************************
 *****************************************************************************
 ***
 *** FIXME FIXME FIXME NOT THE RIGHT THING! MOVE ALL THIS INTO TO WIRISH!
 ***
 *****************************************************************************
 *****************************************************************************/

const uint8 usbVcomDescriptor_LangID[USB_DESCRIPTOR_STRING_LEN(1)] = {
    USB_DESCRIPTOR_STRING_LEN(1),
    USB_DESCRIPTOR_TYPE_STRING,
    0x09,
    0x04
};

const uint8 usbVcomDescriptor_iManufacturer[USB_DESCRIPTOR_STRING_LEN(8)] = {
    USB_DESCRIPTOR_STRING_LEN(8),
    USB_DESCRIPTOR_TYPE_STRING,
    'L', 0, 'e', 0, 'a', 0, 'f', 0,
    'L', 0, 'a', 0, 'b', 0, 's', 0
};

/*
  String Identifiers:

  we may choose to specify any or none of the following string
  identifiers:

  iManufacturer:    LeafLabs
  iProduct:         Maple R3
  iSerialNumber:    NONE
  iConfiguration:   NONE
  iInterface(CCI):  NONE
  iInterface(DCI):  NONE

  additionally we must provide the unicode language identifier,
  which is 0x0409 for US English
*/
const uint8 usbVcomDescriptor_iProduct[USB_DESCRIPTOR_STRING_LEN(8)] = {
    USB_DESCRIPTOR_STRING_LEN(8),
    USB_DESCRIPTOR_TYPE_STRING,
    'M', 0, 'a', 0, 'p', 0, 'l', 0,
    'e', 0, ' ', 0, ' ', 0, ' ', 0
};

/*****************************************************************************
 *****************************************************************************/
