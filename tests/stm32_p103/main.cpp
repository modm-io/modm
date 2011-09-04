
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

#include <xpcc/driver/button_group.hpp>
#include <xpcc/driver/software_spi.hpp>
#include <xpcc/driver/gpio.hpp>

#include <xpcc/driver/lcd/ea_dog.hpp>
#include <xpcc/driver/lcd/font.hpp>

#include <xpcc/workflow/freertos.hpp>

#include <libmaple/flash.h>
#include <libmaple/rcc.h>
#include <libmaple/nvic.h>
#include <libmaple/systick.h>
#include <libmaple/gpio.h>
#include <libmaple/adc.h>
#include <libmaple/timer.h>
#include <libmaple/usb/usb.h>

// ----------------------------------------------------------------------------
xpcc::stm32::Usart2 debugUart(115200);
xpcc::IODeviceWrapper<xpcc::stm32::Usart2> loggerDevice(debugUart); 

xpcc::log::Logger xpcc::log::debug(loggerDevice); 
xpcc::log::Logger xpcc::log::info(loggerDevice); 
xpcc::log::Logger xpcc::log::warning(loggerDevice); 
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedStatInverted, C, 12);	// inverted, 0=on, 1=off
GPIO__OUTPUT(Led1, A, 1);
GPIO__OUTPUT(Led2, A, 8);

typedef xpcc::gpio::Invert<LedStatInverted> LedStat;

// ----------------------------------------------------------------------------
GPIO__INPUT(ButtonWakeUpInverted, A, 0);		// inverted, 0=pressed, 1=not pressed
GPIO__INPUT(Button1Inverted, C, 8);
GPIO__INPUT(Button2Inverted, C, 9);

typedef xpcc::gpio::Invert<ButtonWakeUpInverted> ButtonWakeUp;
typedef xpcc::gpio::Invert<Button1Inverted> Button1;
typedef xpcc::gpio::Invert<Button2Inverted> Button2;

//#define	BUTTON1		0x1
//static xpcc::ButtonGroup<uint32_t> button(BUTTON1);

// ----------------------------------------------------------------------------
// Graphic LCD
namespace lcd
{
	//GPIO__OUTPUT(Sck, A, 5);
	//GPIO__INPUT(Miso, A, 6);
	//GPIO__OUTPUT(Mosi, A, 7);
	
	GPIO__OUTPUT(CS, C, 1);
	GPIO__OUTPUT(A0, C, 3);
	GPIO__OUTPUT(Reset, B, 5);
}

//typedef xpcc::SoftwareSpi< lcd::Sck, lcd::Mosi, lcd::Miso, 10000000UL > Spi;
typedef xpcc::stm32::Spi1 Spi;

xpcc::DogS102< Spi, lcd::CS, lcd::A0, lcd::Reset, false > display;

// GPIO Expander (uses the same SPI as the LCD)
namespace gpio
{
	GPIO__OUTPUT(Cs, C, 0);
	GPIO__INPUT(Int, C, 2);
}

xpcc::Mcp23s08< Spi, gpio::Cs, gpio::Int > gpioExpander;

// ----------------------------------------------------------------------------
/*void
pitHandler(void)
{
	xpcc::Clock::increment();
	
	uint32_t status = 0;
	if (Button1::read()) {
		status |= BUTTON1;
	}
	if (Button2::read()) {
		status |= BUTTON2;
	}
	button.update(~status);
}*/

// ----------------------------------------------------------------------------
#define	LCD_BL_RED		0x01
#define	LCD_BL_GREEN	0x02
#define	LCD_BL_YELLOW	(LCD_BL_RED | LCD_BL_GREEN)

#define	BUTTON_LEFT		0x04
#define	BUTTON_DOWN		0x08
#define	BUTTON_UP		0x10
#define	BUTTON_OK		0x20
#define	BUTTON_RIGHT	0x40

#define	LED_GREEN		0x80

xpcc::freertos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class LedTask1 : public xpcc::freertos::Task
{
public:
	LedTask1() :
		xpcc::freertos::Task(1)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			Led2::toggle();
			this->delay(100 * MILLISECONDS);
			
			event.acquire();
		}
	}
};

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
			event.release();
			
			LedStat::set();
			this->delay(50 * MILLISECONDS);
			
			LedStat::reset();
			this->delay(1000 * MILLISECONDS);
		}
	}
};

class DisplayTask : public xpcc::freertos::Task
{
public:
	DisplayTask() :
		xpcc::freertos::Task(0)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			display.setCursor(0, 16);
			display << "sw  = " << xpcc::hex << gpioExpander.read() << xpcc::ascii;
			display.update();
			
			Led1::set(Button1::read());
			
			this->delay(100);
		}
	}
};

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
	
	ButtonWakeUp::setInput();
	Button1Inverted::setInput(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
	Button2Inverted::setInput(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
	
	display.initialize();
	
	display.setFont(xpcc::font::Assertion);
	display.setCursor(10, 0);
	display << "STM32-P103";
	display.drawLine(0, 13, 102, 13);
	display.update();
	
	display.setFont(xpcc::font::FixedWidth5x8);
	
	//display.setCursor(0, 40);
	//display << event.handle;
	//display.update();
	
	// IO-Expander have to initalized after LCD because they use the same
	// Reset line
	gpioExpander.initialize();
	gpioExpander.configure(0x7c, 0x7c);
	gpioExpander.write(LED_GREEN | LCD_BL_GREEN);
	
	XPCC_LOG_DEBUG << "STM32-P103 Board (extended)" << xpcc::endl;
	
	LedStat::reset();
	
	// Start the FreeRTOS scheduler
	xpcc::freertos::Scheduler::schedule();
	
	while (1)
	{
	}
}
