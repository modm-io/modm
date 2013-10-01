
#include <xpcc/architecture.hpp>
#include <xpcc/communication/can/message.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

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


GPIO__OUTPUT(rot, 0, 6);
GPIO__OUTPUT(blau, 0, 7);
GPIO__OUTPUT(gelb, 0, 8);
GPIO__OUTPUT(gruen, 2, 8);
GPIO__OUTPUT(weiss, 2, 6);

int
main(void)
{
		SystemInit();

	// ----------------------------------------
	// Initialize timer
	// ----------------------------------------
	xpcc::lpc11::SysTickTimer::enable();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);

	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);

	// ----------------------------------------
	// Initialize LEDs
	// ----------------------------------------
	// Set LEDs as output
	rot::setOutput();
	gelb::setOutput();
	blau::setOutput();
	gruen::setOutput();
	weiss::setOutput();

	// Switch LEDs to initial state
	rot::reset();
	gelb::reset();
	blau::reset();
	gruen::set();
	weiss::reset();

	// ----------------------------------------
	// Initialize UART
	// ----------------------------------------
	xpcc::lpc::Uart1 uart(115200);

	xpcc::PeriodicTimer<> sendTimer(1000);
	xpcc::PeriodicTimer<> tenMSecTimer(10);

	// ----------------------------------------
	// Initialize CAN
	// ----------------------------------------
	xpcc::can::Message message;
	xpcc::lpc::Can::initialize(xpcc::can::BITRATE_125_KBPS);

	// allow all messages to be received
	xpcc::lpc11c::CanFilter::setFilter(
			xpcc::lpc11c::CanFilter::ExtendedIdentifier(0),
			xpcc::lpc11c::CanFilter::ExtendedFilterMask(0),
			0, 4);

	XPCC_LOG_INFO << "CAN Counter Alpha 0.1" << xpcc::endl;

	uint32_t msg_counter = 0;
	uint32_t msg_counter_win_start = 0; // start of 10 ms window
	uint32_t msg_counter_max_win = 0;   // max messages in 10 ms window
	uint32_t loop_counter = 0;

	while (1)
	{
		if (xpcc::lpc::Can::isMessageAvailable()) {
			++msg_counter;
			xpcc::lpc::Can::getMessage(message);	// dummy read
		}

		if(tenMSecTimer.isExpired()) {
			uint32_t count = msg_counter - msg_counter_win_start;
			if (count > msg_counter_max_win)
				msg_counter_max_win = count;
			msg_counter_win_start = msg_counter;
		}

		if(sendTimer.isExpired()) {
			XPCC_LOG_INFO << xpcc::endl << "------------------------ " << loop_counter << xpcc::endl;
			XPCC_LOG_INFO << msg_counter << "msg/sec" << xpcc::endl;
			XPCC_LOG_INFO << "10ms peak window:" << xpcc::endl;
			XPCC_LOG_INFO << msg_counter_max_win << "msg/10msec" << xpcc::endl;

			msg_counter = 0;
			msg_counter_win_start = 0;
			msg_counter_max_win = 0;
			gelb::toggle();
			loop_counter++;
		}
	}
}
