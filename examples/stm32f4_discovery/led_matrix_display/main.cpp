#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/driver/display/max7219matrix.hpp>

/**
 * Example to demonstrate a XPCC graphics display made
 * of three 8x8 LED matrices driven by MAX7219 chips.
 *
 * The modules are daisy-chained using SPI.
 *
 * The demo
 */

// Software SPI is simple and fast
typedef xpcc::stm32::GpioOutputE1 Data;
typedef xpcc::stm32::GpioOutputE3 Cs;
typedef xpcc::stm32::GpioOutputE5 Clk;

typedef xpcc::SoftwareSpiSimpleMaster< Clk, Data > Spi;

// The array of 3 modules which all are placed horizontally
xpcc::Max7219matrix<Spi, Cs, 3, 1> ledMatrixDisplay;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);
    
    Data::setOutput();
    Cs::setOutput();
    Clk::setOutput();

    Spi::initialize< 10000000 >();
    
    ledMatrixDisplay.initialize();

    ledMatrixDisplay.setFont(xpcc::font::FixedWidth5x8);
	ledMatrixDisplay.setCursor(0, 0);

    while (1)
    {
    	for (int16_t sec = 9999; sec >= 0; --sec)
		{
    		// Use the LED Matrix as a normal xpcc buffered graphics display
			ledMatrixDisplay.clear();
			ledMatrixDisplay.printf("%04d", sec);
			ledMatrixDisplay.update();

			xpcc::delay_ms(100);
			LedOrange::toggle();
		}
    }

    return 0;
}
