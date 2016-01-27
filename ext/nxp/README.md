## LPC device headers

This folder includes the CMSIS device headers for all LPC devices.
The files included here are part of the CMSIS libraries for the particular chip.
Only the device header and system init header from the `inc` folder are copied.

Unfortunately the device headers are scattered in a lot of libaries and example code that can be found
when searching for the device family on [the download page of lpcware.com](https://www.lpcware.com/downloads).

**BE AWARE THAT NXP HAS APPLIED THE FOLLOWING NON-STANDARD LICENSE TO THEIR HEADER FILES!**
```
Software that is described herein is for illustrative purposes only
which provides customers with programming information regarding the
products. This software is supplied "AS IS" without any warranties.
NXP Semiconductors assumes no responsibility or liability for the
use of the software, conveys no license or title under any patent,
copyright, or mask work right to the product. NXP Semiconductors
reserves the right to make changes in the software without
notification. NXP Semiconductors also make no representation or
warranty that such application will be suitable for the specified
use without further testing or modification.

Permission to use, copy, modify, and distribute this software and its
documentation is hereby granted, under NXP Semiconductors'
relevant copyright in the software, without fee, provided that it
is used in conjunction with NXP Semiconductors microcontrollers.  This
copyright, permission, and disclaimer notice must appear in all copies of
this code.
```
