#! /usr/bin/env python

# -*- coding: iso-8859-1 -*-

##########################################################################
# USPP Library (Universal Serial Port Python Library)
#
# Copyright (C) 2006 Isaac Barona <ibarona@gmail.com>
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
##########################################################################

#-------------------------------------------------------------------------
# Project:   USPP Library (Universal Serial Port Python Library)
# Name:      uspp.py
# Purpose:   Main module. Imports the correct module for the platform
#            in which it is running.
#
# Author:    Isaac Barona Martinez <ibarona@gmail.com>
# Copyright: (c) 2006 by Isaac Barona Martinez
# Licence:   LGPL
#
# Created:   26 June 2001
# History:
#            05/08/2001: Release version 0.1.
#            24/02/2006: Final version 1.0.
#
#-------------------------------------------------------------------------


"""
USPP - Universal Serial Port Python Library

This module exports a SerialPort class to access serial ports in
different platforms (currently W9X and Linux) with the same code.

When the library user import the uspp module,  it automatically 
loads the correct class implementation for the platform in which
is running.

The public classes, exceptions and methods are the following:

SerialPortException
===================
Exception raised in the SerialPort methods.


SerialPort
==========
Class that encapsulate methods to access serial ports. It has the
following public methods:

* __init__(self, dev, timeout=None, speed=None, mode='232', params=None)

  Create an object to access serial port 'dev'

  Arguments:

    dev: String that indicate the name of the port. Possible values are:
        In Linux: '/dev/ttyS0', '/dev/ttyS1', ..., '/dev/ttySX' or
                  '/dev/cua0', '/dev/cua1', ..., '/dev/cuaX'
        In W9X: 'COM1', 'COM2', ..., 'COMX'
        
    timeout: specifies in milliseconds the inter-byte timeout. Possible
        values are:
        * None: For blocking readings. Time-outs are not used for reading
          operations.
        * 0: For non-blocking readings. The reading operation is to return 
          inmediately with the characters that have already been received, 
          even if no characters have been received.
        * >0: For time-out readings. A character must be read in less than
          this value.

    speed: integer that specifies the input and output baud rate to
        use. Possible values are: 110, 300, 600, 1200, 2400, 4800, 9600,
        19200, 38400, 57600 and 115200.
        If None a default speed of 9600 bps is selected.

    mode: string that specifies RS-232 or RS-485 mode. The RS-485 mode
        is half duplex and use the RTS signal to indicate the
        direction of the communication (transmit or recive).
        Posible values are: '232' or '485'. Default to RS232 mode (the
        only implemented just now).

    params: list that specifies properties of the serial communication.
        If params=None it uses default values for the number of bits
        per byte (8), the parity (NOPARITY) and the number of stop bits (1)
        else:
            * In W9X: params must be a list with three items setting up the 
              these values in this order.
            * In Linux: params must is the termios package mode array to use 
              for initialization. 



* __del__(self):

  Destroy the SerialPort object and close the serial port. It is a good
  idea that when you finish working with the serial port you explicity
  do: del tty (where tty is a SerialPort object).

* fileno(self):

  Returns the file descriptor of the serial port. This information is
  sometimes necessary for example if you want to use the select function.

* read(self, num=1):

  Read num bytes from the serial port. Depending the timeout value used in
  the inicialitation this operation can be:

    - Blocking (if timeout=None): The operation blocks until num bytes 
      arrive the serial port.
    - Non-blocking (if timeout=0): The operation returns inmediatelly
      with as many as num bytes that were waiting in the serial port to be read.
    - Time-out (if timeout>0): A byte must arrive in less milliseconds than
      the specified. If the number of read bytes is less than num a
      SerialPortException is raised because a time-out has happened.

* write(self, s):
  
  Write the string s to the serial port.

* inWaiting(self):

  Returns the number of bytes waiting to be read.


* flush(self):
  
  Discards all characters from the output or input buffer.



NOTE ON CHARACTERS AND BYTES
============================

The write and read methods of the SerialPort class expect data 
in string buffers. Do not think this library only works for 
ASCII communications. To interpret string elements as bytes (integer number)
you only have to use the built-in ord() function. To convert a byte
into a string element, use chr().

Example: Suppose you want to transmit the following three bytes:
0x02, 0x10, 0x30. You only have to do:

packet = ''
packet = packet + chr(0x02) + chr(0x10) + chr(0x30)
tty.write(packet)

So, you can see the bytes you send or receive as integers or as characters
depending the situation.



"""



__author__="Isaac Barona Martinez <ibarona@tid.es>"

__copyright__="""
Copyright (C) 2001 Isaac Barona Martinez <ibarona@tid.es>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 dated
June, 1991.
"""

__version__="0.1"


import sys

if sys.platform=='win32':
    from SerialPort_win import *
elif sys.platform=='linux2':
    from SerialPort_linux import *
else:
    sys.exit('Sorry, no implemented for this platform yet')
