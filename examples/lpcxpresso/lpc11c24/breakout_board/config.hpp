// ----------------------------------------------------------------------------

#ifndef CONFIG_HPP
#define	CONFIG_HPP

#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/spi.hpp>
#include <xpcc/driver/ui/seven_segment.hpp>

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

// ----------------------------------------------------------------------------

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

	// A seven-segment display with 4 Digits
	typedef xpcc::sevenSegment::SevenSegmentDisplay<Spi, Load, 4> Ssd;
}

// ----------------------------------------------------------------------------

namespace can
{
	// CAN Controller
	typedef xpcc::lpc::Can Can;
}
#endif	// CONFIG_HPP

// ----------------------------------------------------------------------------

namespace adc
{
	// BATTERY	PIO0.11	AD0
	// CURRENT	PIO1.0	AD1
	// --				AD2
	// POSITION	PIO1.2	AD3 (Servo)
	// --		SWD		AD4
	// POTI		PIO1.4	AD5
	// BEMF_A	PIO1.10	AD6
	// BEMF_B	PIO1.11	AD7

//	typedef xpcc::lpc111x::AdcManualSingle Adc;
	typedef xpcc::lpc111x::AdcAutomaticBurst Adc;
}

// ----------------------------------------------------------------------------

namespace servo
{
	GPIO__OUTPUT(EnA,  2, 11);
	GPIO__OUTPUT(EnB,  0,  2);
	GPIO__OUTPUT(PwmA, 0,  8);
	GPIO__OUTPUT(PwmB, 0,  9);
}
