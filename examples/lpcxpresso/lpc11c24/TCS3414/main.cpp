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

	xpcc::i2c::WriteReadAdapter wrA;
	i2c::initialize();
	XPCC_LOG_DEBUG << "Hello world" << xpcc::endl;




	uint8_t buffer_write[3];
	uint8_t buffer_read[8];

	// Initialize colour sensor
	const uint8_t address = 0b0111001 << 1;

	if(!tcs::initialize()) {
		XPCC_LOG_DEBUG << "ERROR WITH INTIALIZING!" << xpcc::endl; }
//	buffer_write[0] = 0x80 | 0x40 | 0x00;		// command to write into the control register with block mode
//	buffer_write[1] = 1;				// set the number of bytes to write
//	buffer_write[2] = 0b11;				// control to power up and start conversion
//	wrA.initialize(address, buffer_write, 3, buffer_read, 0	);
//	if(!i2c::startSync(&wrA))
//		{while(1){}}
//	if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
//		XPCC_LOG_DEBUG << "ERROR WITH:";
//	XPCC_LOG_DEBUG << "Sensor initialised." << xpcc::endl;
	xpcc::delay_ms(100);

//	buffer_write[0] = 0x80 | 0x40 | 0x00;		// command to write into the control register with block mode
//	buffer_write[1] = 0b11;				// control to power up and start conversion
//	wrA.initialize(address, buffer_write, 2, buffer_read, 0	);
//
//	xpcc::delay_ms(100);

	buffer_write[0] = 0x80 | 0x40 | 0x01;		// command to write into timing register
	buffer_write[1] = 1;				// set the number of bytes to write
	buffer_write[2] = 0b01;				// command to set the integration time to 100ms
	wrA.initialize(buffer_write, 3, buffer_read, 0	);
	if(!i2c::startSync(&wrA))
		{while(1){}}
	if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
		XPCC_LOG_DEBUG << "ERROR WITH:";
	XPCC_LOG_DEBUG << "timing initialised." << xpcc::endl;
	xpcc::delay_ms(100);

	buffer_write[0] = 0x80 | 0x40 | 0x07;		// command to write into gain register
	buffer_write[1] = 1;				// set the number of bytes to write
	buffer_write[2] = 0x20;				// command to set the gain to 4x
	wrA.initialize(buffer_write, 3, buffer_read, 0	);
	if(!i2c::startSync(&wrA))
		{while(1){}}
	if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
		XPCC_LOG_DEBUG << "ERROR WITH:";
	XPCC_LOG_DEBUG << "gain initialised." << xpcc::endl;
	xpcc::delay_ms(100);

//	buffer_write[0] = 0x80 | 0x40 | 0x01;		// command to write into timing register
//	buffer_write[1] = 0b00;						// command to set the integration time to 100ms
//	wrA.initialize(buffer_write, 2, buffer_read, 0	);

	while (1)
	{
		gelb::toggle();
		static uint16_t i=0;
		xpcc::delay_ms(150);

		buffer_write[0] = 0x80 | 0x40 | 0x10;				// request to read data channels
		wrA.initialize(buffer_write, 1, buffer_read, 2*4);	// read all 8 data registers
		if(!i2c::startSync(&wrA))
			{while(1){}}
		if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
			XPCC_LOG_DEBUG << "ERROR WITH:";

		uint16_t value[4] = {0};
		for(uint8_t i=0; i<4; ++i)
		{
			value[i] =	buffer_read[0+2*i];
			value[i] |=	buffer_read[1+2*i]	<< 8;
			XPCC_LOG_DEBUG << "\t" << value[i] << ":";
		}
		XPCC_LOG_DEBUG << xpcc::endl;

		if((value[1]*100)/value[3] > 70)
			rot::set();
		else
			rot::reset();

		if((value[1]*100)/value[3] < 40)
				blau::set();
			else
				blau::reset();

//		if((value[2]*100)/value[3] < 20)
//				gelb::set();
//			else
//				gelb::reset();
	}
}

