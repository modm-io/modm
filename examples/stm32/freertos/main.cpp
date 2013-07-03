
#include <xpcc/architecture.hpp>
#include <xpcc/processing.hpp>
#include <xpcc/debug.hpp>

#include <xpcc/driver/gpio.hpp>
#include <xpcc/ui/display/ea_dog.hpp>
#include <xpcc/ui/display/font.hpp>

#include <xpcc/processing/rtos.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedStatInverted, C, 12);	// inverted, 0=on, 1=off
GPIO__OUTPUT(Led1, A, 1);
GPIO__OUTPUT(Led2, A, 8);

typedef xpcc::gpio::Invert<LedStatInverted> LedStat;

GPIO__INPUT(ButtonWakeUp, A, 0);		// 1=pressed, 0=not pressed

namespace lcd
{
	GPIO__OUTPUT(CS, C, 1);
	GPIO__OUTPUT(A0, C, 3);
	GPIO__OUTPUT(Reset, B, 5);
}

static bool
initClock()
{
	typedef xpcc::stm32::Clock C;
	
	// use external 8MHz crystal, stm32f1
	if (!C::enableHse(C::HseConfig::HSE_CRYSTAL)) {
		return false;
	}
	
	C::enablePll(C::PllSource::PLL_HSE, C::PllMul::PLL_MUL_9);
	return C::switchToPll();
}

// Graphic LCD
xpcc::DogS102< xpcc::stm32::SpiMaster1, lcd::CS, lcd::A0, lcd::Reset, false > display;

// ----------------------------------------------------------------------------
xpcc::rtos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class LedThread1 : public xpcc::rtos::Thread
{
public:
	LedThread1() :
		xpcc::rtos::Thread(2)
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
class LedThread2 : public xpcc::rtos::Thread
{
public:
	LedThread2() :
		xpcc::rtos::Thread(2)
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
class DisplayThread : public xpcc::rtos::Thread
{
public:
	DisplayThread() :
		xpcc::rtos::Thread(0), index(0)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			const xpcc::glcd::Point center(80, 32);
			const uint16_t radius = 20;
			
			index = (index + 1) % 360;
			
			display.clear();
			
			display.setCursor(4, 10);
			display.setFont(xpcc::font::FixedWidth5x8);
			display << "angle=";
			
			display.setCursor(4, 20);
			display.setFont(xpcc::font::Numbers14x32);
			display << index;
			
			display.drawCircle(center, radius);
			float angle = (index * M_PI) / 180.0;
			display.drawLine(center,
					center + xpcc::glcd::Point(
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
main(void)
{
	// Switch to the external clock and enable the PLL to let
	// the STM32 run at 72 MHz.
	initClock();

	LedStat::setOutput(xpcc::Gpio::HIGH);
	Led1::setOutput(xpcc::Gpio::LOW);
	Led2::setOutput(xpcc::Gpio::LOW);
	
	// The Button has an external Pull-Down resistor
	ButtonWakeUp::setInput(xpcc::stm32::FLOATING);
	
	display.initialize();
	display.setFont(xpcc::font::FixedWidth5x8);
	
	// Start the FreeRTOS scheduler
	xpcc::rtos::Scheduler::schedule();
}

