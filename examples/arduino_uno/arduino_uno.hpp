// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// Arduino UNO
//

#ifndef XPCC_ARDUINO_UNO_HPP
#define XPCC_ARDUINO_UNO_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// "Digital" Pins
typedef GpioD0       Digital0;
typedef GpioOutputD0 DigitalOutput0;
typedef GpioInputD0  DigitalInput0;
typedef GpioD1       Digital1;
typedef GpioOutputD1 DigitalOutput1;
typedef GpioInputD1  DigitalInput1;
typedef GpioD2       Digital2;
typedef GpioOutputD2 DigitalOutput2;
typedef GpioInputD2  DigitalInput2;
typedef GpioD3       Digital3;
typedef GpioOutputD3 DigitalOutput3;
typedef GpioInputD3  DigitalInput3;
typedef GpioD4       Digital4;
typedef GpioOutputD4 DigitalOutput4;
typedef GpioInputD4  DigitalInput4;
typedef GpioD5       Digital5;
typedef GpioOutputD5 DigitalOutput5;
typedef GpioInputD5  DigitalInput5;
typedef GpioD6       Digital6;
typedef GpioOutputD6 DigitalOutput6;
typedef GpioInputD6  DigitalInput6;
typedef GpioD7       Digital7;
typedef GpioOutputD7 DigitalOutput7;
typedef GpioInputD7  DigitalInput7;
typedef GpioB0       Digital8;
typedef GpioOutputB0 DigitalOutput8;
typedef GpioInputB0  DigitalInput8;
typedef GpioB1       Digital9;
typedef GpioOutputB1 DigitalOutput9;
typedef GpioInputB1  DigitalInput9;
typedef GpioB2       Digital10;
typedef GpioOutputB2 DigitalOutput10;
typedef GpioInputB2  DigitalInput10;
typedef GpioB3       Digital11;
typedef GpioOutputB3 DigitalOutput11;
typedef GpioInputB3  DigitalInput11;
typedef GpioB4       Digital12;
typedef GpioOutputB4 DigitalOutput12;
typedef GpioInputB4  DigitalInput12;
typedef GpioB5       Digital13;
typedef GpioOutputB5 DigitalOutput13;
typedef GpioInputB5  DigitalInput13;


// "Analog" Input
typedef GpioC0       Analog0;
typedef GpioOutputC0 AnalogOutput0;
typedef GpioInputC0  AnalogInput0;
typedef GpioC1       Analog1;
typedef GpioOutputC1 AnalogOutput1;
typedef GpioInputC1  AnalogInput1;
typedef GpioC2       Analog2;
typedef GpioOutputC2 AnalogOutput2;
typedef GpioInputC2  AnalogInput2;
typedef GpioC3       Analog3;
typedef GpioOutputC3 AnalogOutput3;
typedef GpioInputC3  AnalogInput3;
typedef GpioC4       Analog4;
typedef GpioOutputC4 AnalogOutput4;
typedef GpioInputC4  AnalogInput4;
typedef GpioC5       Analog5;
typedef GpioOutputC5 AnalogOutput5;
typedef GpioInputC5  AnalogInput5;


#endif	// XPCC_ARDUINO_UNO_HPP
