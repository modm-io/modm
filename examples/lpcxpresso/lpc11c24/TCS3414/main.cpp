/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 *
 */
#define __NEWLIB__

#include <xpcc/architecture.hpp>
#include <xpcc/architecture/platform.hpp>

#include <xpcc/debug/logger.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

// ----------------------------------------------------------------------------
// Logging



extern xpcc::lpc::Uart1 loggerUart;
xpcc::IODeviceWrapper<xpcc::lpc::Uart1> loggerDevice(loggerUart);

//#define DEBUG_SW_I2C(x) xpcc::lpc::Uart1::write(x);
// I2C
#include <xpcc/driver/connectivity/i2c/software_i2c.hpp>
#include <xpcc/driver/connectivity/i2c/write_read_adapter.hpp>
#include <xpcc/driver/other/tcs3414.hpp>

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------


#include <xpcc/architecture/driver/gpio.hpp>
GPIO__IO(scl, 0, 4);
GPIO__IO(sda, 0, 5);

GPIO__OUTPUT(rot, 0, 6);
GPIO__OUTPUT(blau, 0, 7);
GPIO__OUTPUT(gelb, 0, 8);
GPIO__OUTPUT(gruen, 2, 8);
GPIO__OUTPUT(weiss, 2, 6);

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
	XPCC_LOG_DEBUG << "Hello world" << xpcc::endl;

	// Initialize colour sensor
	const uint8_t address = 0b0111001 << 1;

	if(!tcs::initialize()) {
		XPCC_LOG_DEBUG << "ERROR WITH INTIALIZING!" << xpcc::endl; }

	xpcc::delay_ms(100);

	tcs::configure(xpcc::tcs3414::Gain::X4, xpcc::tcs3414::Prescaler::DEFAULT,
			xpcc::tcs3414::IntegrationMode::DEFAULT, xpcc::tcs3414::NominalIntegrationTime::MSEC_100);

	while (1)
	{
		uint8_t buffer_read[8];
		gelb::toggle();
		static uint16_t i=0;
		xpcc::delay_ms(150);

		const xpcc::color::Rgb colors = tcs::getNewRelColors();

		XPCC_LOG_DEBUG <<
				colors.green	<< "%\t" <<
				colors.red		<< "%\t" <<
				colors.blue		<< "%" << xpcc::endl;

//		if((red*100)/clear > 70)
//			rot::set();
//		else
//			rot::reset();
//
//		if((red*100)/clear < 40)
//				blau::set();
//			else
//				blau::reset();
	}
}
