#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture.hpp>
#include <xpcc/processing.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/driver/display/max7219matrix.hpp>

/**
 * Example to demonstrate a XPCC graphics display made
 * of three 8x8 LED matrices driven by MAX7219 chips.
 *
 * The modules are daisy-chained using SPI.
 *
 * The demo shows a counter counting from 9999 down to 0.
 *
 * The first of three MAX7219 based LED Matrix displays is connected
 * as following:
 *
 * PE1	Data
 * PE3	Cs
 * PE5	Clk
 *
 * GND and +3V3 are connected to the display module.
 * The other modules are daisy-chained.
 *
 */

// Software SPI is simple and fast
typedef xpcc::stm32::GpioOutputE1 Data;
typedef xpcc::stm32::GpioOutputE3 Cs;
typedef xpcc::stm32::GpioOutputE5 Clk;

typedef xpcc::SoftwareSpiMaster< Clk, Data > Spi;

// The array of 3 modules which all are placed horizontally
xpcc::Max7219matrix<Spi, Cs, 3, 1> ledMatrixDisplay;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);

    Data::setOutput();
    Cs::setOutput();
    Clk::setOutput();

    Spi::initialize< defaultSystemClock, 10000000 >();

    ledMatrixDisplay.initialize();

    ledMatrixDisplay.setFont(xpcc::font::FixedWidth5x8);
	ledMatrixDisplay.setCursor(0, 0);

	xpcc::ShortPeriodicTimer countdownTimer(100);

    while (1)
    {
		if (countdownTimer.isExpired())
		{
			for (int16_t sec = 9999; sec >= 0; --sec)
			{
				// Use the LED Matrix as a normal xpcc buffered graphics display
				ledMatrixDisplay.clear();
				ledMatrixDisplay.printf("%04d", sec);
				ledMatrixDisplay.update();

				LedOrange::toggle();
			}
		}
    }

    return 0;
}
