#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#include <xpcc/processing/timer.hpp>
#include <xpcc/processing/protothread.hpp>

#include <xpcc/driver/motion/adns9800.hpp>

#include <inttypes.h>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

class BlinkThread : public xpcc::pt::Protothread
{
public:
	BlinkThread()
	{
		timeout.restart(100);
	}

	bool
	update()
	{
		PT_BEGIN();

		while (true)
		{
			Board::LedGreen::reset();

			PT_WAIT_UNTIL(timeout.isExpired());
			timeout.restart(100);

			Board::LedGreen::set();

			PT_WAIT_UNTIL(timeout.isExpired()) ;
			timeout.restart(4900);

			XPCC_LOG_INFO << "Seconds since reboot: " << uptime << xpcc::endl;

			uptime += 5;
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	uint32_t uptime;
};

class Adns9800Thread : public xpcc::pt::Protothread
{
public:
	Adns9800Thread() : timer(10), x(0), y(0)
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		Cs::setOutput(xpcc::Gpio::High);

		xpcc::stm32::GpioOutputA7::connect(xpcc::stm32::SpiMaster1::Mosi);
		xpcc::stm32::GpioOutputA5::connect(xpcc::stm32::SpiMaster1::Sck);
		xpcc::stm32::GpioInputA6::connect(xpcc::stm32::SpiMaster1::Miso);
		xpcc::stm32::SpiMaster1::initialize<Board::systemClock, 2250000ul>();
		xpcc::stm32::SpiMaster1::setDataMode(xpcc::stm32::SpiMaster1::DataMode::Mode3);

		adns9800::initialise();

		while (true)
		{
			PT_WAIT_UNTIL(timer.execute());

			{
				int16_t delta_x, delta_y;
				adns9800::getDeltaXY(delta_x, delta_y);
				XPCC_LOG_INFO.printf("dx = %5" PRId16 ", dy = %5" PRId16"; x = %9" PRId32", y=%9" PRId32 "\n", delta_x, delta_y, x, y);

				x += delta_x;
				y += delta_y;
			}
		}

		PT_END();
	}

private:
	xpcc::ShortPeriodicTimer timer;
	int32_t x, y;

	using Cs = xpcc::stm32::GpioOutputA4;

	using adns9800 = xpcc::Adns9800< 
		/* Spi = */ xpcc::stm32::SpiMaster1, 
		/* Ncs = */ Cs >;
};


BlinkThread blinkThread;
Adns9800Thread adns9800Thread;


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_*
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	XPCC_LOG_INFO << "Welcome to ADNS 9800 demo." << xpcc::endl;

	while (true)
	{
		blinkThread.update();
		adns9800Thread.update();
	}

	return 0;
}
