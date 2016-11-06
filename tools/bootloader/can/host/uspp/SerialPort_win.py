# -*- coding: utf-8 -*-
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
# Name:      SerialPort_win.py
# Purpose:   Handle low level access to serial port in windows.
#
# Author:    Isaac Barona Martinez <ibarona@gmail.com>
# Copyright: (c) 2001 by Isaac Barona Martínez
# Licence:   LGPL
#
# Created:   26 June 2001
# History:
# 20 January 2002 : Damien Géranton <dgeranton@voila.fr>
#  Bug fix for Win2000, the file must not be open with
#  FILE_FLAG_OVERLAPPED
#
# 29 March 2006: Version 1.1
#   * Correct an undefined simbol in readline method: Matt Kraai
#   <kraai@ftbfs.org>
#   * COM ports with two digits are allowed: Brian Cain <brian.cain@gmail.com>
# 
#
#-------------------------------------------------------------------------

"""
SerialPort_win.py - Handle low level access to serial port in windows.

See also uspp module docstring.

"""


from win32file import *
from win32event import *
import win32con
import exceptions

class SerialPortException(exceptions.Exception):
    """Exception raise in the SerialPort methods"""
    def __init__(self, args=None):
        self.args=args


class SerialPort:
    """Encapsulate methods for accesing to a serial port."""

    BaudRatesDic={110: CBR_110,
                  300: CBR_300,
                  600: CBR_600,
                  1200: CBR_1200,
                  2400: CBR_2400,
                  4800: CBR_4800, 
                  9600: CBR_9600,
                  19200: CBR_19200,
                  38400: CBR_38400,
                  57600: CBR_57600,
                  115200: CBR_115200,
                  }

    def __init__(self, dev, timeout=None, speed=None, mode='232', params=None):
        """Open the serial port named by the string 'dev'

        'dev' can be any of the following strings: 'COM1', 'COM2', ... 'COMX'
        
        'timeout' specifies the inter-byte timeout or first byte timeout
        (in miliseconds) for all subsequent reads on SerialPort.
        If we specify None time-outs are not used for reading operations
        (blocking reading).
        If 'timeout' is 0 then reading operations are non-blocking. It
        specifies that the reading operation is to return inmediately
        with the bytes that have already been received, even if
        no bytes have been received.
        
        'speed' is an integer that specifies the input and output baud rate to
        use. Possible values are: 110, 300, 600, 1200, 2400, 4800, 9600,
        19200, 38400, 57600 and 115200.
        If None a default speed of 9600 bps is selected.
        
        'mode' specifies if we are using RS-232 or RS-485. The RS-485 mode
        is half duplex and use the RTS signal to indicate the
        direction of the communication (transmit or recive).
        Default to RS232 mode (at moment, only the RS-232 mode is
        implemented).

        'params' is a list that specifies properties of the serial 
        communication.
        If params=None it uses default values for the number of bits
        per byte (8), the parity (NOPARITY) and the number of stop bits (1)
        else params must be a list with three items setting up the 
        these values in this order.

        """

        dev = "\\\\.\\" + dev # to allow to digits ports 
        self.__devName, self.__timeout, self.__speed=dev, timeout, speed
        self.__mode=mode
        self.__params=params
        try:
            self.__handle=CreateFile (dev,
                                  win32con.GENERIC_READ|win32con.GENERIC_WRITE,
                                  0, # exclusive access
                                  None, # no security
                                  win32con.OPEN_EXISTING,
                                  win32con.FILE_ATTRIBUTE_NORMAL,
                                  None)
        except:
            raise SerialPortException('Unable to open port')

        self.__configure()

    def __del__(self):
        """Close the serial port
        
        To close the serial port we have to do explicity: del s
        (where s is an instance of SerialPort)
        """
        try:
            CloseHandle(self.__handle)
        except IOError:
            raise SerialPortException('Unable to close port')


    def __configure(self):
        """Configure the serial port.

        Private method called in the class constructor that configure the 
        serial port with the characteristics given in the constructor.
        """
        if not self.__speed:
            self.__speed=9600
        # Tell the port we want a notification on each char
        SetCommMask(self.__handle, EV_RXCHAR)
        # Setup a 4k buffer
        SetupComm(self.__handle, 4096, 4096)
        # Remove anything that was there
        PurgeComm(self.__handle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|
                  PURGE_RXCLEAR)

        # Setup the timeouts parameters for the port
        # timeouts is a tuple with the following items:
        # [0] int : ReadIntervalTimeout
        # [1] int : ReadTotalTimeoutMultiplier
        # [2] int : ReadTotalTimeoutConstant
        # [3] int : WriteTotalTimeoutMultiplier
        # [4] int : WriteTotalTimeoutConstant

        if self.__timeout==None:
            timeouts= 0, 0, 0, 0, 0
        elif self.__timeout==0:
            timeouts = win32con.MAXDWORD, 0, 0, 0, 1000
        else:
            timeouts= self.__timeout, 0, self.__timeout, 0 , 1000
        SetCommTimeouts(self.__handle, timeouts)

        # Setup the connection info
        dcb=GetCommState(self.__handle)
        dcb.BaudRate=SerialPort.BaudRatesDic[self.__speed]
        if not self.__params:
            dcb.ByteSize=8
            dcb.Parity=NOPARITY
            dcb.StopBits=ONESTOPBIT
        else:
            dcb.ByteSize, dcb.Parity, dcb.StopBits=self.__params
        SetCommState(self.__handle, dcb)

    def timeout(self, timeout):
        self.__timeout = timeout
        self.__configure()

    def fileno(self):
        """Return the file descriptor for opened device.

        This information can be used for example with the 
        select function.
        """
        return self.__handle


    def read(self, num=1):
        """Read num bytes from the serial port.

        If self.__timeout!=0 and != None and the number of read bytes is less
        than num an exception is generated because a timeout has expired.
        If self.__timeout==0 read is non-blocking and inmediately returns
        up to num bytes that have previously been received.
        """

        (hr, buff) = ReadFile(self.__handle, num)
        if len(buff)<>num and self.__timeout!=0: # Time-out  
            raise SerialPortException('Timeout')
        else:
            return buff


    def readline(self):
        """Read a line from the serial port.  Returns input once a '\n'
        character is found.
        
        """

        s = ''
        while not '\n' in s:
            s = s+SerialPort.read(self,1)

        return s 


    def write(self, s):
        """Write the string s to the serial port"""
        overlapped=OVERLAPPED()
        overlapped.hEvent=CreateEvent(None, 0,0, None)
        WriteFile(self.__handle, s, overlapped)
        # Wait for the write to complete
        WaitForSingleObject(overlapped.hEvent, INFINITE)

    def inWaiting(self):
        """Returns the number of bytes waiting to be read"""
        flags, comstat = ClearCommError(self.__handle)
        return comstat.cbInQue

    def flush(self):
        """Discards all bytes from the output or input buffer"""
        PurgeComm(self.__handle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|
                  PURGE_RXCLEAR)



        

