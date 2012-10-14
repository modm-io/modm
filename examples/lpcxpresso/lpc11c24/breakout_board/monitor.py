#!/usr/bin/env python

#OS interface
import sys
import argparse

# Good math and plotting
import matplotlib
import pylab

# IO
import time
import serial

# Parsing data
import struct

def unpackPacket(buffer, format):
    '''Parses a packet from the logger'''
    packet = struct.unpack(format, buffer)
    
    return packet

class SerialDummy:
    '''Dummy class for emulating data from the serial port for development and debugging.'''
    ii = 0
    
    def read(self, len):
        self.ii = self.ii + 1
        return struct.pack('hh', self.ii, self.ii * 2)
    
    def inWaiting(self):
        return 100
    
def createCanvas():
    global fig, line1, ax, manager
    xAxis = pylab.arange(0, 2000, 1)
    yAxis = pylab.array([0] * 2000)

    fig = pylab.figure(1)
    ax = fig.add_subplot(111)
    ax.grid(True)
    ax.set_title("Realtime Waveform Plot")
    ax.set_xlabel("Time")
    ax.set_ylabel("Amplitude")
    ax.axis([0, 2000, -500, 500])
    line1 = ax.plot(xAxis, yAxis, '-')
    
    manager = pylab.get_current_fig_manager()

def timerStart():
    global fig
    timerPlotter = fig.canvas.new_timer(interval = 50)
    timerPlotter.add_callback(realtimePlotter, ())
    
    timerSerial = fig.canvas.new_timer(interval = 10)
    timerSerial.add_callback(serialThread, ())
    
    timerPlotter.start()
    timerSerial.start()


def realtimePlotter(arg):
    '''Thread (?) that updates the plot.'''
    global values, line1, ax, manager
    
    CurrentXAxis = pylab.arange(len(values) - 2000, len(values), 1)
    line1[0].set_data(CurrentXAxis, pylab.array(values[-2000:]))
    
    # Set limits of plot
    ax.axis([CurrentXAxis.min(), CurrentXAxis.max(), -1000, 1000])
    
    manager.canvas.draw()

def serialThread(arg):
    '''Thread (?) that is called from timer'''
    global ser, synchronised
    
    if synchronised:
        while ser.inWaiting() >= packetLen:
            sBuf = ser.read(packetLen)
            
            try:
                packet = unpackPacket(sBuf, packetFormat)
            except (struct.error):
                synchronised = False
                if args.debug:
                    print "Synchronisation lost."
            
            print packet
            values.append(packet[0])
    else:
        if ser.inWaiting() >= 1:
            sBuf = ser.read(1)
            if sBuf == '#':
                print "Go!"
                synchronised = True

def main():
    global values, ser, synchronised
    
    synchronised = False
    
    values = []
    values = [0 for x in range(2000)]
    
    parser = argparse.ArgumentParser(description='Display debug data from XPCC Logger')
    parser.add_argument('-v', '--verbose', action='store_true', help='Be more verbose while running.')
    parser.add_argument('-d', '--debug', action='store_true', help='Run the program in debug mode without external hardware.')
    parser.add_argument('-p', '--port', action='store', default='/dev/ttyUSB0', help='The hardware port to open.')
    parser.add_argument('-b', '--baudrate', action='store', help='The baudrate of the hardware interface.', default=3000000)
    
    args = parser.parse_args()

    if args.debug and args.verbose:
        print "Running in debug mode."
    
    if args.debug:
        if args.verbose:
            print "Opening dummy source."
            
        ser = SerialDummy()
            
    else:
        if args.verbose:
            print "Opening port %s at %s baud." % (args.port, args.baudrate)
            
        ser = serial.Serial(args.port, args.baudrate, timeout=None)

    global packetLen, packetFormat

    packetLen = 4
    packetFormat = 'hh'
        
    synchronised = False
    
#    if args.debug:
        # skip synchronisation
#        synchronised = True
        
    createCanvas()
    timerStart()
    
    # Main loop
    pylab.show()
    
if __name__ == "__main__":
    main()
