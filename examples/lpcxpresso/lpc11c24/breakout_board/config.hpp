// ----------------------------------------------------------------------------

#ifndef CONFIG_HPP
#define	CONFIG_HPP

#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/spi.hpp>

// ----------------------------------------------------------------------------

namespace led
{
	GPIO__OUTPUT(Xpresso,  0, 7);
	GPIO__OUTPUT(Onboard,  3, 3);
	GPIO__OUTPUT(DuoGreen, 3, 2);
	GPIO__OUTPUT(DuoRed,   3, 1);
}

// ----------------------------------------------------------------------------

namespace button
{
	GPIO__INPUT(nUp,   1, 5);
	GPIO__INPUT(nOk,   1, 8);
	GPIO__INPUT(nDown, 0, 6);

	typedef xpcc::lpc::Invert< nUp > Up;
	typedef xpcc::lpc::Invert< nOk > Ok;
	typedef xpcc::lpc::Invert< nDown > Down;
}

// ----------------------------------------------------------------------------

namespace spi
{
	// Pins for software SPI
	GPIO__OUTPUT(Sck,  2, 1);
	GPIO__OUTPUT(Mosi, 2, 3);
	GPIO__INPUT (Miso, 2, 2);
}

namespace ssd
{
	// Seven-segment displays, the first bit is the most right display
	GPIO__OUTPUT(Ssel, 2, 0);
	GPIO__OUTPUT(Load, 3, 0);
	GPIO__OUTPUT(Pwm,  1, 1);

	// software SPI, write only
//	typedef xpcc::SoftwareSpi<spi::Sck, spi::Mosi, xpcc::gpio::Unused, 2000000> Spi;

	// Hardware SPI1
	typedef xpcc::lpc::SpiMaster1 Spi;
}
#endif	// CONFIG_HPP
