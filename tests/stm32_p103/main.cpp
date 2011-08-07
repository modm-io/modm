
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

#include <xpcc/driver/button_group.hpp>
#include <xpcc/driver/software_spi.hpp>
#include <xpcc/driver/gpio.hpp>

#include <xpcc/driver/lcd/ea_dog.hpp>
#include <xpcc/driver/lcd/font.hpp>

// ----------------------------------------------------------------------------
/*xpcc::at91::Debug debugUart(115200);
xpcc::IODeviceWrapper<xpcc::at91::Debug> loggerDevice(debugUart); 

xpcc::log::Logger xpcc::log::debug(loggerDevice); 
xpcc::log::Logger xpcc::log::info(loggerDevice); 
xpcc::log::Logger xpcc::log::warning(loggerDevice); 
xpcc::log::Logger xpcc::log::error(loggerDevice);*/

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
	GPIO__OUTPUT(Sck, A, 5);
	GPIO__INPUT(Miso, A, 6);
	GPIO__OUTPUT(Mosi, A, 7);
	
	GPIO__OUTPUT(CS, C, 1);
	GPIO__OUTPUT(A0, C, 3);
	GPIO__OUTPUT(Reset, B, 5);
}

typedef xpcc::SoftwareSpi< lcd::Sck, lcd::Mosi, lcd::Miso, 1000000UL > SPI;
xpcc::DogS102< SPI, lcd::CS, lcd::A0, lcd::Reset, false > display;

// GPIO Expander (uses the same SPI as the LCD)
namespace gpio
{
	GPIO__OUTPUT(Cs, C, 0);
	GPIO__INPUT(Int, C, 2);
}

xpcc::Mcp23s08< SPI, gpio::Cs, gpio::Int > gpioExpander;

// ----------------------------------------------------------------------------
/*void
pitHandler(void)
{
	xpcc::Clock::increment();
	xpcc::at91::Pit::acknowledgeInterrupt();
	
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

// ----------------------------------------------------------------------------
int
main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	LedStat::setOutput(xpcc::gpio::HIGH);
	Led1::setOutput(xpcc::gpio::LOW);
	Led2::setOutput(xpcc::gpio::LOW);
	
	ButtonWakeUp::setInput();
	Button1Inverted::configure(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
	Button2Inverted::configure(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
	
	display.initialize();
	
	display.setFont(xpcc::font::Assertion);
	display.setCursor(10, 0);
	display << "STM32-P103";
	display.drawLine(0, 13, 102, 13);
	display.update();
	
	display.setFont(xpcc::font::FixedWidth5x8);
	
	// IO-Expander have to initalized after LCD because they use the same
	// Reset line
	gpioExpander.initialize();
	gpioExpander.configure(0x7c, 0x7c);
	gpioExpander.write(LED_GREEN | LCD_BL_GREEN);
	
	//XPCC_LOG_DEBUG << "STM32-P103 Board (extended)" << xpcc::endl;
	
	LedStat::reset();
	
	while (1)
	{
		xpcc::delay_ms(100);
		LedStat::toggle();
		
		display.setCursor(0, 16);
		display << "sw  = " << xpcc::hex << gpioExpander.read() << xpcc::ascii;
		display.update();
		
		Led1::set(Button1::read());
		Led2::set(Button2::read());
	}
}
