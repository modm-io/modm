I2C Example
===========

Example for the STM32_F4 breakout board from Fabian.

The program configures and reads these four sensors, which can be found on the
[FreeIMU v0.3.5_BMP][freeimu]:

- BMA180 accelerometer,
- HMC5883L magnetometer,
- ITG3200 gyroscope,
- BMP085 barometer

All sensors except the barometer are configured to generate interrupts when
they have new data ready and their raw hex values are dumped into the iostream.
The barometer is read out every 100ms and its calibrated data is sent as ascii text.

You can choose between using the hardware or software I2C, just uncomment the
right lines.


[freeimu]: http://www.varesano.net/projects/hardware/FreeIMU