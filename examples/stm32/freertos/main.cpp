
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

#include <xpcc/driver/gpio.hpp>
#include <xpcc/driver/ui/display/ea_dog.hpp>
#include <xpcc/driver/ui/display/font.hpp>

#include <xpcc/workflow/freertos.hpp>

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

// Graphic LCD
xpcc::DogS102< xpcc::stm32::Spi1, lcd::CS, lcd::A0, lcd::Reset, false > display;

// ----------------------------------------------------------------------------
xpcc::freertos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class LedTask1 : public xpcc::freertos::Task
{
public:
	LedTask1() :
		xpcc::freertos::Task(2)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			// synchronise with LedTask2
			event.release();
			
			LedStat::set();
			this->delay(50 * MILLISECONDS);
			
			LedStat::reset();
			this->delay(1000 * MILLISECONDS);
		}
	}
};

// Toggle Led2 when LedTask1 starts a new cycle
class LedTask2 : public xpcc::freertos::Task
{
public:
	LedTask2() :
		xpcc::freertos::Task(2)
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
// be interrupted by the other Tasks
class DisplayTask : public xpcc::freertos::Task
{
public:
	DisplayTask() :
		xpcc::freertos::Task(0), index(0)
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
			
			this->delay(100);
		}
	}

private:
	uint16_t index;
};

// Create the three Tasks. They are automatically added to the task list
// and started when the FreeRTOS scheduler is called.
LedTask1 task1;
LedTask2 task2;
DisplayTask task3;

// ----------------------------------------------------------------------------
int
main(void)
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	Led1::setOutput(xpcc::gpio::LOW);
	Led2::setOutput(xpcc::gpio::LOW);
	
	// The Button has an external Pull-Down resistor
	ButtonWakeUp::setInput(xpcc::stm32::INPUT, xpcc::stm32::FLOATING);
	
	display.initialize();
	display.setFont(xpcc::font::FixedWidth5x8);
	
	// Start the FreeRTOS scheduler
	xpcc::freertos::Scheduler::schedule();
}

