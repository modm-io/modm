
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/can/message.hpp>
#include <xpcc/processing/periodic_timer.hpp>
#include <xpcc/driver/ui/display/siemens_s65.hpp>

#define LED_TOGGLE_TICKS 150		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

GPIO__OUTPUT(Led, 0, 7);

namespace lcd
{
	GPIO__OUTPUT(Rs,    2,  1);
	GPIO__OUTPUT(Reset, 2,  0);
	GPIO__OUTPUT(Cs,    2,  2);
}
typedef xpcc::lpc::SpiMaster0 SpiDisplay;
typedef xpcc::SiemensS65Portrait<SpiDisplay, lcd::Cs, lcd::Rs, lcd::Reset> Display;

Display display;

int
main(void)
{
	SystemInit();
	xpcc::lpc::SysTickTimer::enable();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	// Set LED port pin to output
	Led::setOutput();

	xpcc::lpc::Can::initialize(xpcc::can::BITRATE_125_KBPS);

	SpiDisplay::configurePins(SpiDisplay::MappingSck::PIO0_6);
	SpiDisplay::initialize(SpiDisplay::Mode::MODE_0, SpiDisplay::Presacler::DIV002, 0);
	display.initialize();
	display.setFont(xpcc::font::AllCaps3x5);

	display.clear();
	display.setCursor(5, 5);
	display << "Micro-CAN-Debugger" << xpcc::endl;
	display.update();
	display.clear();

	xpcc::PeriodicTimer<> canRxTimer(10);

	while (1)
	{
		if (canRxTimer.isExpired()) {
			// Don't check for messages too often, simulate busy main application
			if (xpcc::lpc::Can::isMessageAvailable()) {
				static uint8_t cnt = 0;
				xpcc::can::Message message;
				xpcc::lpc::Can::getMessage(message);
				display.printf("%3d %7x %1d %02x %02x %02x %02x %02x %02x\n",
						cnt++,
						message.identifier,
						message.length,
						message.data[0],
						message.data[1],
						message.data[2],
						message.data[3],
						message.data[4],
						message.data[5]);
				display.update();
			}

		}

		// Each time we wake up...
		// Check TimeTick to see whether to set or clear the LED I/O pin
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX)) < ((LED_TOGGLE_TICKS / COUNT_MAX) / 2)) {
			Led::reset();
		}
		else {
			Led::set();
		}
		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}
