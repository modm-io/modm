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

class Monitor(object):
    """ 
    """
    
    def __init__(self, args, format):
        self.args = args
        self.struct = struct.Struct(format)
        
        self.displayWidth = 1200
        
        self.handler = SerialHandler(self.args, self.struct.size)
        
        # Allocate memory for every channel. 
        # Unpack an arbitrary string to see how many channels the struct has
        self.channels = len(self.struct.unpack_from("                  "))
        self.values = pylab.zeros([self.displayWidth, self.channels])
        
        self.createCanvas()
        self.startTimers()

    def unpackSample(self, buffer):
        ''' Unpacks a (packed) sample. 
        '''
        sample = self.struct.unpack(buffer)
        return sample

    def createCanvas(self):
        xAxis = pylab.arange(0, self.displayWidth, 1)
        yAxis = pylab.array([0] * self.displayWidth)
    
        self.fig = pylab.figure(1)
        self.ax = self.fig.add_subplot(111)
        self.ax.grid(True)
        self.ax.set_title("Realtime Waveform Plot")
        self.ax.set_xlabel("Time")
        self.ax.set_ylabel("Amplitude")
        self.ax.axis([0, self.displayWidth, 0, 1000])
        
#        self.lines = []
#        for style in ['r-', 'b-']:
        self.lines = self.ax.plot(xAxis, yAxis, 'r-', xAxis, yAxis, 'b-')
        
        self.manager = pylab.get_current_fig_manager()

    def realtimePlotter(self, arg):
        ''' Thread (?) that updates the plot.'''
        
        newestIndex = pylab.shape(self.values)[0]
        
        CurrentXAxis = pylab.arange(newestIndex - self.displayWidth, newestIndex, 1)
        self.lines[0].set_data(CurrentXAxis, self.values[:,0][-self.displayWidth:])
        self.lines[1].set_data(CurrentXAxis, self.values[:,1][-self.displayWidth:])
        
        # Set limits of plot
        self.ax.axis([CurrentXAxis.min(), CurrentXAxis.max(), 0, 1000])
        
        self.manager.canvas.draw()
        
    def parser(self, argument):
        """ Parse data from the data source and fills the values array
        """
        while True:
            buffer = self.handler.serialHandler()
            if buffer <> None:
                sample = self.unpackSample(buffer)
                if sample <> None:
                    self.values = pylab.append(self.values, pylab.array([sample]), axis=0)
            else:
                break
        
    def startTimers(self):
        timerPlotter = self.fig.canvas.new_timer(interval = 50)
        timerPlotter.add_callback(self.realtimePlotter, ())
                
        timerParser = self.fig.canvas.new_timer(interval = 10)
        timerParser.add_callback(self.parser, ())
        
        timerPlotter.start()
        timerParser.start()

class SerialHandler:
    """ 
    """
    synchronised = False
    packetLength = 0
    
    def __init__(self, args, packetLength):
        self.packetLength = packetLength
        if args.verbose:
            print "Packets of size %d expected." % (self.packetLength,)
            print "Opening port %s at %s baud." % (args.port, args.baudrate)
            
        self.ser = serial.Serial(args.port, args.baudrate, timeout = None)
        
    def serialHandler(self):
        """ Receive the stream from the serial port.
            It should be called regularly from a timer
        """
        if not self.synchronised:
            if self.ser.inWaiting() >= 1:
                sBuf = self.ser.read(1)
                if sBuf == '#':
                    print "Go!"
                    self.synchronised = True
        
        if self.synchronised:
            while self.ser.inWaiting() >= self.packetLength:
                sBuf = self.ser.read(self.packetLength)
                return sBuf

class SerialDummy:
    """ Dummy data source for development and debugging.
    """
    ii = 0
    def __init__(self, args, packetLength):
        self.struct = struct.Struct('hhh')
    
    def read(self, len):
        self.ii = self.ii + 1
        
        if len <> self.struct.size:
            raise "Wrong length requested."
        
        return self.struct.pack(self.ii, self.ii * 2)
        


def main():
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
        pass
    
    myMonitor = Monitor(args, 'hhh')
    myMonitor.realtimePlotter('nix')

#    # Main loop
    pylab.show()
    
if __name__ == "__main__":
    main()
