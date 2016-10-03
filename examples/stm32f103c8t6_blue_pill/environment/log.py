#!/usr/bin/env python

import sys
import serial

ser = serial.Serial('/dev/tty.SLAB_USBtoUART', 115200);

f = open('log.csv', 'a')

while True:
	line = ser.readline();
	line = line.rstrip('\n')

	counter, bmp180atemp, bmp180apress, bmp180btemp, bmp180bpress, bme280atemp, bme280apress, bme280ahumid, bme280btemp, bme280bpress, bme280bhumid = line.split(',');

	counter = int(counter)
	bmp180atemp = float(bmp180atemp)
	bmp180apress = float(bmp180apress)
	bme280atemp = float(bme280atemp)
	bme280apress = float(bme280apress)
	bme280ahumid = float(bme280ahumid)
	bmp180btemp = float(bmp180btemp)
	bmp180bpress = float(bmp180bpress)
	bme280btemp = float(bme280btemp)
	bme280bpress = float(bme280bpress)
	bme280bhumid = float(bme280bhumid)


	out = ('%d %6.3f %9.3f %6.3f %9.3f %6.3f %9.3f %6.3f %6.3f %9.3f %6.3f' % (
		counter,
		bmp180atemp * 0.1, bmp180apress,
		bmp180btemp * 0.1, bmp180bpress,
		bme280atemp * 0.01, bme280apress * 0.001, bme280ahumid * 0.001,
		bme280btemp * 0.01, bme280bpress * 0.001, bme280bhumid * 0.001))

	print(out)
	f.write(out + '\n')
	f.flush()

	sys.stdout.flush()
