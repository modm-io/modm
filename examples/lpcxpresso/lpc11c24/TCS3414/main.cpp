/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 *
 */
#define __NEWLIB__

#include <xpcc/architecture.hpp>
#include <xpcc/architecture/platform.hpp>

#include <xpcc/workflow/periodic_timer.hpp>
#include <xpcc/driver/other/tcs3414.hpp>

// ----------------------------------------------------------------------------
// Logging
#include <xpcc/debug/logger.hpp>
extern xpcc::lpc::Uart1 loggerUart;
xpcc::IODeviceWrapper<xpcc::lpc::Uart1> loggerDevice(loggerUart);
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);
// ----------------------------------------------------------------------------

#include <xpcc/architecture/peripheral/gpio.hpp>
GPIO__IO(scl, 0, 4);
GPIO__IO(sda, 0, 5);
GPIO__OUTPUT(rot, 0, 6);
GPIO__OUTPUT(blau, 0, 7);
GPIO__OUTPUT(gelb, 0, 8);
GPIO__OUTPUT(gruen, 2, 8);
GPIO__OUTPUT(weiss, 2, 6);

#include <xpcc/communication/i2c/software_i2c.hpp>
typedef xpcc::SoftwareI2C<scl, sda> i2c;
typedef xpcc::Tcs3414<i2c> tcs;
// ----------------------------------------------------------------------------

int
main(void)
{
	SystemInit();
	xpcc::lpc11::SysTickTimer::enable();

	scl::setOutput(xpcc::lpc::OPEN_DRAIN);
	sda::setOutput(xpcc::lpc::OPEN_DRAIN);

	// Set LED port pin to output
	rot::setOutput();
	gelb::setOutput();
	blau::setOutput();
	gruen::setOutput();
	weiss::setOutput();
	rot::reset();
	gelb::reset();
	blau::reset();
	gruen::reset();
	weiss::set();


	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	xpcc::lpc::Uart1 uart(115200);

	xpcc::delay_ms(10); // glitch ?

	i2c::initialize();
	XPCC_LOG_DEBUG << "TCS3414" << xpcc::endl;

	// Initialize colour sensor
	const uint8_t address = 0b0111001 << 1;

	if(!tcs::initialize()) {
		XPCC_LOG_DEBUG << "ERROR WITH INTIALIZING!" << xpcc::endl; }
	xpcc::delay_ms(100);
	tcs::setGain(xpcc::tcs3414::Gain::X16);
	xpcc::delay_ms(100);
	tcs::setIntegrationTime(xpcc::tcs3414::IntegrationMode::DEFAULT, xpcc::tcs3414::NominalIntegrationTime::MSEC_100);

	while (1)
	{
		gelb::toggle();
		xpcc::delay_ms(150);

		const xpcc::tcs3414::Rgb colors = tcs::getNewColors();
		xpcc::color::HsvT<xpcc::tcs3414::UnderlyingType> hsv;
		colors.toHsv(&hsv);

		XPCC_LOG_DEBUG << "RGB: " << colors << ";" << xpcc::endl;
		XPCC_LOG_DEBUG << "HSV: " << hsv.hue << "\t" << hsv.saturation << "\t" << hsv.value << "\t" << xpcc::endl;

		if(colors.red > 55)
			rot::set();
		else
			rot::reset();

		if(colors.red < 45)
			blau::set();
		else
			blau::reset();
	}
}
