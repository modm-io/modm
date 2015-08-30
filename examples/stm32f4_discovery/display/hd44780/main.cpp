
/**
 * Example to demonstrate a XPCC driver for HD44780 displays,
 * including displays with I2C PCA8574 port expanders.
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 *
 * GND and +5V are connected to the the port expander of the display.
 *
 */

#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>
#include <xpcc/io/iostream.hpp>
#include <xpcc/debug/logger.hpp>
#include <xpcc/driver/gpio/pca8574.hpp>

#include "../../stm32f4_discovery.hpp"

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > device;
xpcc::IOStream stream(device);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(device);
xpcc::log::Logger xpcc::log::info(device);
xpcc::log::Logger xpcc::log::warning(device);
xpcc::log::Logger xpcc::log::error(device);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

using namespace Board;

typedef I2cMaster2 MyI2cMaster;
// typedef xpcc::SoftwareI2cMaster<GpioB10, GpioB11> MyI2cMaster;

// define the pins used by the LCD when not using a port expander
namespace lcd
{
	typedef GpioOutputC7 Backlight;
	typedef GpioOutputC6 E;
	typedef GpioOutputC5 Rw;
	typedef GpioOutputC4 Rs;
	// note: an 8bit data port
	typedef GpioPort<GpioOutputC0, 8> Data8Bit;
	// and a 4 bit data port
	typedef GpioPort<GpioOutputC4, 4> Data4Bit;
}

typedef xpcc::Pca8574<MyI2cMaster> GpioExpander;
GpioExpander gpioExpander;

namespace expander
{
	// Instances for each pin
	typedef GpioExpander::P0< gpioExpander > Rs;
	typedef GpioExpander::P1< gpioExpander > Rw;
	typedef GpioExpander::P2< gpioExpander > E;
	typedef GpioExpander::P3< gpioExpander > Backlight;
	typedef GpioExpander::P4< gpioExpander > Pin4;
	typedef GpioExpander::P5< gpioExpander > Pin5;
	typedef GpioExpander::P6< gpioExpander > Pin6;
	// you can also declare the pin like this, however it is too verbose
	typedef xpcc::GpioExpanderPin< GpioExpander, gpioExpander, GpioExpander::Pin::P7 > Pin7;
//	typedef GpioExpander::P7< gpioExpander > Pin7;

	// Form a GpioPort out of four single pins.
	// This can be optimised by a special implementation of Pca8574Gpio which
	// should provide an interface to a GpioPort, too.
	typedef xpcc::SoftwareGpioPort<Pin7, Pin6, Pin5, Pin4> Data4BitGpio;
}

// You can choose either port width simply by using it.
// The driver will handle it internally.

// create a LCD object with an 8bit data port
// xpcc::Hd44780< lcd::Data8Bit, lcd::Rw, lcd::Rs, lcd::E > display(20, 4);

// create a LCD object with an 4bit data port
// xpcc::Hd44780< lcd::Data4Bit, lcd::Rw, lcd::Rs, lcd::E  > display(20, 4);

// create a LCD object with an 4bit data port at a I2C Gpio port expander
xpcc::Hd44780< expander::Data4BitGpio, expander::Rw, expander::Rs, expander::E  > display(20, 4);

class ThreadOne : public xpcc::pt::Protothread
{
public:
	ThreadOne()
	{
	}

	bool
	update()
	{
		PT_BEGIN();
		XPCC_LOG_DEBUG << "Ping the device from ThreadOne" << xpcc::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(gpioExpander.ping())) {
			 	break;
			}
			XPCC_LOG_DEBUG << "Device did not respond" << xpcc::endl;

			// otherwise, try again in 100ms
			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_DEBUG << "Device responded" << xpcc::endl;

		expander::Backlight::set();

		// Initialize twice as some display are not initialised after first try.
		display.initialize();
		display.initialize();

		// Fill CGRAM
		display.writeCGRAM(0, cgA);
		display.writeCGRAM(1, cgB);

		display.setCursor(0, 0);

		// Write the standard welcome message ;-)
		display << "Hello xpcc.io **\n";

		// Write two special characters in second row
		display.setCursor(0, 1);
		display.write(0);
		display.write(1);

		counter = 0;

		while (true)
		{
			display.setCursor(3, 1);
			display << counter << "   ";

			counter++;

			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	uint8_t counter;

	// Bitmaps for special characters
	uint8_t cgA[8] = {0, 0b00100, 0b01110, 0b11111, 0b11111, 0b01110, 0b00100, 0};
	uint8_t cgB[8] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};
};

ThreadOne one;

int
main()
{
	Board::initialize();

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, xpcc::Uart::B115200>(10);

	XPCC_LOG_INFO << "\n\nWelcome to HD44780 I2C demo!\n\n";

	GpioB11::connect(MyI2cMaster::Sda, Gpio::InputType::PullUp);
	GpioB10::connect(MyI2cMaster::Scl, Gpio::InputType::PullUp);

	MyI2cMaster::initialize<Board::systemClock, 100000>();

	xpcc::ShortPeriodicTimer tmr(500);

	while(true)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}
}
