#include <modm/architecture/platform.hpp>
#include <modm/processing/timer.hpp>
#include <modm/driver/rtc/ds1302/ds1302.hpp>

struct ds1302_config : public modm::ds1302::Config
{
	using Ce   = Board::D8;
	using Io   = Board::D9;
	using Sclk = Board::D10;
};
using ds1302 = modm::Ds1302< ds1302_config >;

// ----------------------------------------------------------------------------
/**
 * Realtime Clock RTC DS1302 test.
 *
 * This example compares the internal oscillator of the processor with an external
 * Real Time Clock (RTC).
 *
 * External hardware is a "RTC DS1302 module" connected to D8, D9 and D10.
 * See the code for the pinout.
 *
 * The expected result should be:
 * \code
 * Welcome to RTC DS1302 demo.
 * CPU frequency is 16000000 Hz
 * *  9.995 seconds from CPU are 10 seconds from RTC.
 * *  9.975 seconds from CPU are 10 seconds from RTC.
 * *  9.955 seconds from CPU are 10 seconds from RTC.
 * *  9.935 seconds from CPU are 10 seconds from RTC.
 * *  9.915 seconds from CPU are 10 seconds from RTC.
 * *  9.895 seconds from CPU are 10 seconds from RTC.
 * *  9.875 seconds from CPU are  9 seconds from RTC.
 * *  9.876 seconds from CPU are  9 seconds from RTC.
 * *  9.877 seconds from CPU are  9 seconds from RTC.
 * *  9.878 seconds from CPU are  9 seconds from RTC.
 * *  9.879 seconds from CPU are  9 seconds from RTC.
 * *  9.880 seconds from CPU are  9 seconds from RTC.
 * *  9.881 seconds from CPU are  9 seconds from RTC.
 * *  9.882 seconds from CPU are  9 seconds from RTC.
 * *  9.883 seconds from CPU are  9 seconds from RTC.
 * *  9.884 seconds from CPU are  9 seconds from RTC.
 * *  9.885 seconds from CPU are  9 seconds from RTC.
 * *  9.886 seconds from CPU are  9 seconds from RTC.
 * *  9.887 seconds from CPU are  9 seconds from RTC.
 * *  9.888 seconds from CPU are  9 seconds from RTC.
 * *  9.889 seconds from CPU are 10 seconds from RTC.
 * /endcode
 */

int
main()
{
	Board::initialize();

	MODM_LOG_DEBUG << modm::endl << "Welcome to RTC DS1302 demo." << modm::endl;

	ds1302_config::Ce::setOutput();
	ds1302_config::Sclk::setOutput();
	ds1302::initialize();

	// Disable write protect
	ds1302::writeProtect(false);

	// Enable RTC oscillator
	// Side effect: set seconds to 0
	ds1302::enableOscillator();

	uint16_t tt = 9995; // Milliseconds
	modm::Timeout timeout;
	timeout.restart(tt);

	// Periodically report progress
	modm::PeriodicTimer blinkTimer(250);

	MODM_LOG_DEBUG.printf("CPU frequency is %ld Hz\n", Board::systemClock::Frequency);

	uint8_t cc[] = "|/-\\";
	uint8_t cycle(0);

	while(true)
	{
		if (blinkTimer.execute()) {
			MODM_LOG_DEBUG.printf("\b%c", cc[cycle++]);
			cycle %= 4;
			Board::Leds::toggle();
		}
		if (timeout.execute())
		{
			modm::ds1302::Data rtc_data;
			ds1302::readRtc(rtc_data);
			uint8_t seconds = rtc_data.getSeconds();

			MODM_LOG_DEBUG.printf("\b* %2d.%03d seconds from CPU are %2d seconds from RTC.\n",
				tt / 1000,
				tt % 1000,
				seconds);

			// Reset seconds to 0
			ds1302::write(0x80, 0x00);

			// Adjust timeout time by some milliseconds to match RTC time.
			if (seconds >= 10) {
				tt -= 20;
			} else {
				tt += 1;
			}

			timeout.restart(tt);
			blinkTimer.restart();
		}
	};

	return 0;
}
