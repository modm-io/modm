import matplotlib
import time
import pylab
import serial

ser = serial.Serial("/dev/ttyUSB0", 1500000, timeout=None)

# interactive mode on
pylab.ion()

while 1:
    if ser.inWaiting() >= 1:
        s = ser.read(1)
#        if ord(s) == ord('#'):
        if s == '#':
            break
        else:
            print('.')
            print s
            print ord(s)
            
print('go:')

while 1:
    if ser.inWaiting() >= 2:
        s = ser.read(2)
#        print(''.join(['%02x ' % ord(x) for x in s]))
        print('%4d' % (ord(s[1]) * 256 + ord(s[0])))
#        print s

