
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/can/message.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

#define LED_TOGGLE_TICKS 150		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

GPIO__OUTPUT(Led, 0, 7);

int
main(void)
{
	SystemInit();
	xpcc::lpc11::SysTickTimer::enable();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	// Set LED port pin to output
	Led::setOutput();
	
	/* Output the Clk onto the CLKOUT Pin PIO0_1 to monitor the freq on a scope */
	LPC_IOCON->PIO0_1 = (1<<0);
	/* Select the MAIN clock as the clock out selection since it's driving the core */
	LPC_SYSCON->CLKOUTCLKSEL = 3;
	/* Set CLKOUTDIV to 10 */
	LPC_SYSCON->CLKOUTDIV = 10;		//	CLKOUT Divider = 10
	/* Enable CLKOUT */
	LPC_SYSCON->CLKOUTUEN = 0;
	LPC_SYSCON->CLKOUTUEN = 1;
	while (!(LPC_SYSCON->CLKOUTUEN & 0x01));

	xpcc::lpc::Can::initialize(xpcc::can::BITRATE_125_KBPS);

	xpcc::can::Message message;
	message.identifier = 0x123456;
	message.length = 8;
	message.setRemoteTransmitRequest(false);
	message.data[0] = 'A';
	message.data[1] = 'B';
	message.data[2] = 'C';
	message.data[3] = 'D';
	message.data[4] = 'E';
	message.data[5] = 'F';
	message.data[6] = 'G';
	message.data[7] = 'H';
	xpcc::lpc::Can::sendMessage(message);

	xpcc::PeriodicTimer<> canTxTimer(3000);

	while (1)
	{
		if (canTxTimer.isExpired()) {
			// burst!
			for (uint8_t ii = 0; ii < 32; ++ii)
			{
				static uint8_t id = 0;
				message.data[7] = id++;
				xpcc::lpc::Can::sendMessage(message);
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
