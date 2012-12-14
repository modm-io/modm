
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/spi.hpp>
#include <xpcc/driver/ui/display/siemens_s65.hpp>

namespace lcd
{
	GPIO__OUTPUT(Cs, C, 1);
	GPIO__OUTPUT(Rs, C, 3);
	GPIO__OUTPUT(Reset, B, 5);
//	GPIO__OUTPUT(Backlight, F, 0);
}

namespace spi
{
	GPIO__OUTPUT(Sck,  A, 5);
	GPIO__OUTPUT(Mosi, A, 7);
}

typedef xpcc::SiemensS65Portrait<xpcc::stm32::SpiMaster1, lcd::Cs, lcd::Rs, lcd::Reset> Display;

GPIO__OUTPUT(LedStat, C, 12);		// inverted, 0=on, 1=off


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

Display display;

MAIN_FUNCTION
{
	xpcc::stm32::SpiMaster1::configurePins(xpcc::stm32::SpiMaster1::REMAP_PA5_PA6_PA7);
	xpcc::stm32::SpiMaster1::initialize(xpcc::stm32::SpiMaster1::MODE_0, xpcc::stm32::SpiMaster1::PRESCALER_4);

	initClock();

//	Backlight::setOutput();
//	Backlight::set();

	display.initialize();
	display.setFont(xpcc::font::Assertion);


	LedStat::setOutput(xpcc::gpio::HIGH);
	//lcd::Cs::setOutput();
	//spi::Mosi::setOutput();

//	while (1)
//	{
//		//lcd::Cs::toggle();
//		//spi::Mosi::toggle();
//		LedStat::toggle();
//		xpcc::delay_ms(500);
//	}

	bool dir = true;
	uint8_t y = 0;
	while (1)
	{

		if (1==2)
		{
			display.clear();
			display.setCursor(5, y);
			display << "Hello";
			display.setCursor(46, 16);
			display << "World!";

			display.drawLine(0, y, 40, 31 - y);
			display.drawLine(132 - 40, 31 - y, 132, y);

			if (dir) {
				y++;
				if (y >= 5) {
					dir = false;
				}
			}
			else {
				y--;
				if (y == 0) {
					dir = true;
				}
			}
		} // show 1

		if (1==1)
		{
			display.clear();

			// text demo
			display.setFont(xpcc::font::Assertion);
			display.setCursor(xpcc::glcd::Point(0, 0));
			display << "Hello World!\n\n";
			display << "ABCDEFGHIJKLMNOP\nQRSTUVWXYZ\n";
			display << "0123456789!\"§$%&/\n()=?`´,;:-<>\n";
			display.setFont(xpcc::font::Numbers14x32);
			display << "0.123456789";


			// bouncing ball
			static int8_t xSpeed = 3;
			static int8_t ySpeed = 1;
			static uint8_t x = 15;
			static uint8_t y = 90;
			const uint8_t radius = 5;

			display.drawRectangle(xpcc::glcd::Point(10, 78),
					display.getWidth()-20,
					display.getHeight()-88);

			x += xSpeed;
			y += ySpeed;

			if (x < 10 + radius + 3 || x > display.getWidth() - radius - 13) {
				xSpeed = -xSpeed;
			}
			if (y < 78 + radius + 1 || y > display.getHeight() - 10 - radius - 1) {
				ySpeed = -ySpeed;
			}

			display.drawCircle(xpcc::glcd::Point(x, y), radius);

			// finished, copy to lcd
			display.update();
		}
	}

	while(1)
		;
}
