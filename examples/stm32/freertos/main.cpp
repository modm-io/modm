/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>
#include <modm/processing/processing.hpp>
#include <modm/debug/debug.hpp>

#include <modm/driver/display.hpp>
#include <modm/ui/display/font.hpp>

#include <modm/processing/rtos.hpp>

using namespace modm::platform;

// ----------------------------------------------------------------------------
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> > defaultSystemClock;

typedef GpioOutputC12 LedStatInverted;	// inverted, 0=on, 1=off
typedef GpioOutputA1  Led1;
typedef GpioOutputA8  Led2;

typedef GpioInverted<LedStatInverted> LedStat;

typedef GpioInputA0 ButtonWakeUp;		// 1=pressed, 0=not pressed

namespace lcd
{
    typedef GpioOutputC1 CS;
    typedef GpioOutputC3 A0;
    typedef GpioOutputB5 Reset;
}

// Graphic LCD
modm::DogS102< modm::platform::SpiMaster1, lcd::CS, lcd::A0, lcd::Reset, false > display;

// ----------------------------------------------------------------------------
modm::rtos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class LedThread1 : public modm::rtos::Thread
{
public:
	LedThread1() :
		modm::rtos::Thread(2)
	{
	}

	virtual void
	run()
	{
		while (1)
		{
			// synchronize with LedThread2
			event.release();

			LedStat::set();
			this->sleep(50 * MILLISECONDS);

			LedStat::reset();
			this->sleep(1000 * MILLISECONDS);
		}
	}
};

// Toggle Led2 when LedThread1 starts a new cycle
class LedThread2 : public modm::rtos::Thread
{
public:
	LedThread2() :
		modm::rtos::Thread(2)
	{
	}

	virtual void
	run()
	{
		while (1)
		{
			Led2::toggle();

			// wait for the other task
			event.acquire();
		}
	}
};

// Display a rotating hand in a circle and some fonts. Lowest priority, will
// be interrupted by the other Threads
class DisplayThread : public modm::rtos::Thread
{
public:
	DisplayThread() :
		modm::rtos::Thread(0), index(0)
	{
	}

	virtual void
	run()
	{
		while (1)
		{
			const modm::glcd::Point center(80, 32);
			const uint16_t radius = 20;

			index = (index + 1) % 360;

			display.clear();

			display.setCursor(4, 10);
			display.setFont(modm::font::FixedWidth5x8);
			display << "angle=";

			display.setCursor(4, 20);
			display.setFont(modm::font::Numbers14x32);
			display << index;

			display.drawCircle(center, radius);
			float angle = (index * M_PI) / 180.0;
			display.drawLine(center,
					center + modm::glcd::Point(
							 sin(angle) * radius,
							-cos(angle) * radius));
			display.update();

			Led1::set(ButtonWakeUp::read());

			this->sleep(100);
		}
	}

private:
	uint16_t index;
};

// Create the three Threads. They are automatically added to the thread list
// and started when the FreeRTOS scheduler is called.
LedThread1 thread1;
LedThread2 thread2;
DisplayThread thread3;

// ----------------------------------------------------------------------------
int
main()
{
	// Switch to the external clock and enable the PLL to let
	// the STM32 run at 168 MHz.
	defaultSystemClock::enable();
	modm::cortex::SysTickTimer::initialize<defaultSystemClock>();

	LedStat::setOutput(modm::Gpio::High);
	Led1::setOutput(modm::Gpio::Low);
	Led2::setOutput(modm::Gpio::Low);

	// The Button has an external Pull-Down resistor
    ButtonWakeUp::setInput(Gpio::InputType::Floating);

	display.initialize();
	display.setFont(modm::font::FixedWidth5x8);

	// Start the FreeRTOS scheduler
	modm::rtos::Scheduler::schedule();
}

