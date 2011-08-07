/* *****************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * ****************************************************************************/

#ifndef __DESCRIPTORS_H
#define __DESCRIPTORS_H


#include "../libmaple.h"
#include "usb_lib.h"

#define USB_DESCRIPTOR_TYPE_DEVICE        0x01
#define USB_DESCRIPTOR_TYPE_CONFIGURATION 0x02
#define USB_DESCRIPTOR_TYPE_STRING        0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE     0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT      0x05

#define USB_DEVICE_CLASS_CDC              0x02
#define USB_DEVICE_SUBCLASS_CDC           0x00

#define USB_CONFIG_ATTR_BUSPOWERED        0b10000000
#define USB_CONFIG_ATTR_SELF_POWERED      0b11000000

#define EP_TYPE_INTERRUPT                 0x03
#define EP_TYPE_BULK                      0x02

#define USB_DESCRIPTOR_ENDPOINT_IN        0x80
#define USB_DESCRIPTOR_ENDPOINT_OUT       0x00

#define USB_DESCRIPTOR_STRING_LEN(x) (2 + (x << 1))


#if defined(__cplusplus)
extern "C" {
#endif

#define USB_DESCRIPTOR_STRING(len)		\
  struct {					\
    uint8 bLength;				\
    uint8 bDescriptorType;			\
    uint16 bString[len];			\
  }

#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)	\
  struct					\
  {						\
    uint8 bLength;				\
    uint8 bDescriptorType;			\
    uint8 SubType;				\
    uint8 Data[DataSize];			\
  }

typedef struct {
  uint8                 bLength;
  uint8                 bDescriptorType;
  uint16                bcdUSB;
  uint8                 bDeviceClass;
  uint8                 bDeviceSubClass;
  uint8                 bDeviceProtocol;
  uint8                 bMaxPacketSize0;
  uint16                idVendor;
  uint16                idProduct;
  uint16                bcdDevice;
  uint8                 iManufacturer;
  uint8                 iProduct;
  uint8                 iSerialNumber;
  uint8                 bNumConfigurations;
} USB_Descriptor_Device;

typedef struct {
  uint8                 bLength;
  uint8                 bDescriptorType;
  uint16                wTotalLength;
  uint8                 bNumInterfaces;
  uint8                 bConfigurationValue;
  uint8                 iConfiguration;
  uint8                 bmAttributes;
  uint8                 bMaxPower;
} USB_Descriptor_Config_Header;

typedef struct {
  uint8                 bLength;
  uint8                 bDescriptorType;
  uint8                 bInterfaceNumber;
  uint8                 bAlternateSetting;
  uint8                 bNumEndpoints;
  uint8                 bInterfaceClass;
  uint8                 bInterfaceSubClass;
  uint8                 bInterfaceProtocol;
  uint8                 iInterface;
} USB_Descriptor_Interface;

typedef struct {
  uint8                 bLength;
  uint8                 bDescriptorType;
  uint8                 bEndpointAddress;
  uint8                 bmAttributes;
  uint16                wMaxPacketSize;
  uint8                 bInterval;
} USB_Descriptor_Endpoint;

typedef struct {
  /* config header */
  uint8                 bLength;
  uint8                 bDescriptorType;
  uint16                wTotalLength;
  uint8                 bNumInterfaces;
  uint8                 bConfigurationValue;
  uint8                 iConfiguration;
  uint8                 bmAttributes;
  uint8                 bMaxPower;

  USB_Descriptor_Interface            CCI_Interface;
  struct {						
    uint8 bLength;				
    uint8 bDescriptorType;			
    uint8 SubType;				
    uint8 Data[2];			
  }                                   CDC_Functional_IntHeader;
  struct {						
    uint8 bLength;				
    uint8 bDescriptorType;			
    uint8 SubType;				
    uint8 Data[2];			
  }                                   CDC_Functional_CallManagement;
  struct {						
    uint8 bLength;				
    uint8 bDescriptorType;			
    uint8 SubType;				
    uint8 Data[1];			
  }                                   CDC_Functional_ACM;
  struct {						
    uint8 bLength;				
    uint8 bDescriptorType;			
    uint8 SubType;				
    uint8 Data[2];			
  }                                   CDC_Functional_Union;

  /*
  USB_Descriptor_Endpoint             ManagementEndpoint;
  */
  uint8                 EP1_bLength;
  uint8                 EP1_bDescriptorType;
  uint8                 EP1_bEndpointAddress;
  uint8                 EP1_bmAttributes;
  uint8                 EP1_wMaxPacketSize0;
  uint8                 EP1_wMaxPacketSize1;
  uint8                 EP1_bInterval;

  /*
  USB_Descriptor_Interface            DCI_Interface;
  */

  uint8                 DCI_bLength;
  uint8                 DCI_bDescriptorType;
  uint8                 DCI_bInterfaceNumber;
  uint8                 DCI_bAlternateSetting;
  uint8                 DCI_bNumEndpoints;
  uint8                 DCI_bInterfaceClass;
  uint8                 DCI_bInterfaceSubClass;
  uint8                 DCI_bInterfaceProtocol;
  uint8                 DCI_iInterface;

  /*
  USB_Descriptor_Endpoint             DataOutEndpoint;
  USB_Descriptor_Endpoint             DataInEndpoint;
  */

  uint8                 EP2_bLength;
  uint8                 EP2_bDescriptorType;
  uint8                 EP2_bEndpointAddress;
  uint8                 EP2_bmAttributes;
  uint8                 EP2_wMaxPacketSize0;
  uint8                 EP2_wMaxPacketSize1;
  uint8                 EP2_bInterval;

  uint8                 EP3_bLength;
  uint8                 EP3_bDescriptorType;
  uint8                 EP3_bEndpointAddress;
  uint8                 EP3_bmAttributes;
  uint8                 EP3_wMaxPacketSize0;
  uint8                 EP3_wMaxPacketSize1;
  uint8                 EP3_bInterval;

  
}USB_Descriptor_Config;
 
  typedef struct {
    uint8          bLength;
    uint8          bDescriptorType;
    uint16         bString[];
  } USB_Descriptor_String;

extern const USB_Descriptor_Device usbVcomDescriptor_Device;
extern const USB_Descriptor_Config usbVcomDescriptor_Config;

extern const uint8 usbVcomDescriptor_LangID[USB_DESCRIPTOR_STRING_LEN(1)];
extern const uint8 usbVcomDescriptor_iManufacturer[USB_DESCRIPTOR_STRING_LEN(8)];
extern const uint8 usbVcomDescriptor_iProduct[USB_DESCRIPTOR_STRING_LEN(8)];

#if defined(__cplusplus)
  }
#endif

#endif  // __DESCRIPTORS_H
