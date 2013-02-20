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

#define DEBUG_SW_I2C(x) xpcc::lpc::Uart1::write(x);

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// I2C
#include <xpcc/driver/connectivity/i2c/software_i2c.hpp>
#include <xpcc/driver/connectivity/i2c/write_read_adapter.hpp>

#include <xpcc/architecture/driver/gpio.hpp>
GPIO__IO(scl, 0, 4);
GPIO__IO(sda, 0, 5);
typedef xpcc::SoftwareI2C<scl, sda> i2c;

// ----------------------------------------------------------------------------

int
main(void)
{
	SystemInit();
	xpcc::lpc11::SysTickTimer::enable();

	scl::setOutput(xpcc::lpc::OPEN_DRAIN);
	sda::setOutput(xpcc::lpc::OPEN_DRAIN);

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	xpcc::lpc::Uart1 uart(115200);

	xpcc::delay_ms(10000); // glitch ?

	xpcc::i2c::WriteReadAdapter wrA;
	i2c::initialize();
	XPCC_LOG_DEBUG << "Hello world" << xpcc::endl;


	uint8_t buffer_write[3];
	buffer_write[1] = 1;
	uint8_t buffer_read[2];

	// Initialize colour sensor
	const uint8_t address = 0b0111001 << 1;
	const uint8_t control_pwrUp = 0b11;

	buffer_write[0] = 0x80;			// command to write into the control register
	buffer_write[2] = control_pwrUp;	// control to power up and start conversion
	wrA.initialize(address, buffer_write, 3, buffer_read, 0	);

	if(!i2c::startSync(&wrA))
		{while(1){}}
	if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
		XPCC_LOG_DEBUG << "ERROR WITH:";
	XPCC_LOG_DEBUG << "Sensor initialised." << xpcc::endl;
	while (1)
	{
		static uint16_t i=0;
		xpcc::delay_ms(4000);
		XPCC_LOG_DEBUG << "Read clear value #" << i++ << xpcc::endl;

		buffer_write[0] = 0x16;		// clear value low
		buffer_write[1] = address;	// repeat address
		wrA.initialize(buffer_write, 2, buffer_read, 2);
		if(!i2c::startSync(&wrA))
			{while(1){}}
		if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
			XPCC_LOG_DEBUG << "ERROR WITH:";
		XPCC_LOG_DEBUG << "Low value read." << xpcc::endl;

		uint16_t value = 0;
		value = buffer_read[1];
		XPCC_LOG_DEBUG << "low Value read: " << value << xpcc::endl;
	}/*
		xpcc::delay_ms(100);
		buffer_write[0] = 0x17;		// clear value high
		buffer_write[1] = address;	// repeat address
		wrA.initialize(buffer_write, 2, buffer_read, 2);
		if(!i2c::startSync(&wrA))
			{while(1){}}
		if(wrA.getState() != xpcc::i2c::adapter::State::NO_ERROR)
			XPCC_LOG_DEBUG << "ERROR WITH:";
		XPCC_LOG_DEBUG << "High value read." << xpcc::endl;

		value |= buffer_read[1]	<< 8;
		XPCC_LOG_DEBUG << "Value read: " << value << xpcc::endl;
	}//*/
}

