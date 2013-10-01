# XML Device Files

The concept of XML Device Files makes it possible to generate a device
specific driver library for every microcontorller supported by xpcc.

## Goals of XML Device File architecture
* Make cpp/hpp files resulting from templates more legible by removing
  almost all of the preprocessor ifdefs
* Only use jinja2 if structures in template files, this makes the code easier
  to read because no longer you need to differentiate between checks executed
  when the template is converted and executed during compile time
* Stronger compile time checks with a device specific library
* More powerful mixing and matching of peripheral and core drivers

## Content of a XML Device File
* Name of device: 
* Family: stm32, lpc, atmega etc.

## Doxygen
Since there will be no cpp/hpp in the xpcc architcture/platform sources
anymore, we need a doxygen XML Device File to generate a generic documentation
from. Probably one from every family or maybe even seperate ones for
STM32F1/F2/F3/F4 etc. would be necessary.
