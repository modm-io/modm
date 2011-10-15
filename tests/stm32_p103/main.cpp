
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

#include <xpcc/driver/gpio.hpp>
#include <xpcc/driver/connectivity/software_spi.hpp>

#include <xpcc/driver/ui/button_group.hpp>
#include <xpcc/driver/ui/display/ea_dog.hpp>
#include <xpcc/driver/ui/display/font.hpp>

#include <xpcc/driver/storage/fat.hpp>
#include <xpcc/driver/storage/sd.hpp>

#include <xpcc/workflow/freertos.hpp>

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
/*GPIO__INPUT(Button1Inverted, C, 8);
GPIO__INPUT(Button2Inverted, C, 9);*/

typedef xpcc::gpio::Invert<ButtonWakeUpInverted> ButtonWakeUp;
/*typedef xpcc::gpio::Invert<Button1Inverted> Button1;
typedef xpcc::gpio::Invert<Button2Inverted> Button2;*/

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

/*xpcc::freertos::BinarySemaphore event;

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
DisplayTask task3;*/

#include <libmaple/systick.h>
#include <fatfs/ff.h>
#include "sd.hpp"

SdCard sdCard;
xpcc::fat::FileSystem fileSystem(&sdCard);

FIL file;
DIR dir;
FILINFO fno;
FRESULT rc;

void
systick_timer(void)
{
	static uint16_t cnt=0;
	static uint8_t cntdiskio=0;

	cnt++;
	if (cnt >= 500) {
		cnt = 0;
		// alive sign
		Led1::toggle();
	}
	
	cntdiskio++;
	if (cntdiskio >= 10) {
		cntdiskio = 0;
		sdCard.disk_timerproc(); /* to be called every 10ms */
	}
}

// ----------------------------------------------------------------------------
int
main(void)
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	Led1::setOutput(xpcc::gpio::LOW);
	Led2::setOutput(xpcc::gpio::LOW);
	
	ButtonWakeUp::setInput();
	//Button1Inverted::setInput(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
	//Button2Inverted::setInput(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
	
	display.initialize();
	
	display.setFont(xpcc::font::Assertion);
	display.setCursor(10, 0);
	display << "STM32-P103";
	display.drawLine(0, 13, 102, 13);
	display.update();
	
	display.setFont(xpcc::font::FixedWidth5x8);
	
	// IO-Expander have to initialized after LCD because they use the same
	// Reset line
	gpioExpander.initialize();
	gpioExpander.configure(0x7c, 0x7c);
	gpioExpander.write(LED_GREEN | LCD_BL_GREEN);
	
	XPCC_LOG_DEBUG << "STM32-P103 Board (extended)" << xpcc::endl;
	
	LedStat::reset();
	
	systick_attach_callback(&systick_timer);
	
	// Start the FreeRTOS scheduler
	//xpcc::freertos::Scheduler::schedule();
	
	// Directory testing
	XPCC_LOG_DEBUG << "Open root directory " << xpcc::endl;
	rc = f_opendir(&dir, "");
	
	if (!rc) {
		XPCC_LOG_DEBUG << " OK" << xpcc::endl;
	}
	else {
		XPCC_LOG_DEBUG << " Error=" << rc << xpcc::endl;
	}
	
	XPCC_LOG_DEBUG << "\nDirectory listing" << xpcc::endl;
	for (;;)
	{
		rc = f_readdir(&dir, &fno); // Read a directory item
		if (rc || !fno.fname[0]) {
			break; // Error or end of dir
		}
		
		if (fno.fattrib & AM_DIR) {
			XPCC_LOG_DEBUG << "   [dir]  " << fno.fname << xpcc::endl; // is Directory
		}
		else {
			XPCC_LOG_DEBUG.printf("%8lu  %s\n", fno.fsize, fno.fname); // is file
		}
	}

	if (!rc) {
		XPCC_LOG_DEBUG << "Listing Dir complete" << xpcc::endl;
	}
	else {
		XPCC_LOG_DEBUG << "Listing Dir Failure...error = " << rc << xpcc::endl;
	}
	
	XPCC_LOG_DEBUG << "open file" << xpcc::endl;
	FRESULT res = f_open(&file, "test1.txt", FA_READ);
	if (res) {
		XPCC_LOG_DEBUG << "could not open file!" << xpcc::endl;
	}
	else {
		XPCC_LOG_DEBUG << "Read file:" << xpcc::endl;
		while (1) {
			char buffer[128];
			size_t bytesRead;
			
			res = f_read(&file, buffer, sizeof(buffer), &bytesRead);
			
			if (res || (bytesRead == 0)) {
				break;
			}
			
			for (uint32_t i = 0; i < bytesRead; ++i) {
				XPCC_LOG_DEBUG << buffer[i];
			}
		}
		XPCC_LOG_DEBUG << "eof" << xpcc::endl;
	}
	f_close(&file);
	
	f_mount(0, NULL);
	
	while (1)
	{
		display.setCursor(0, 16);
		display << "sw  = " << xpcc::hex << gpioExpander.read() << xpcc::ascii;
		display.update();
		
		//Led1::set(ButtonWakeUp::read());
	}
}
