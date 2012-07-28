#ifndef XPCC_AVR32__TIMER_0_HPP
#define XPCC_AVR32__TIMER_0_HPP

#include <stdint.h>

#include <avr32/io.h>

#include "../timer.hpp"

namespace xpcc
{
	namespace avr32
	{
		class Timer0
		{
		public:
			// This is a 16 bit timer
			typedef uint16_t Value;

			static inline void
			start()
			{
				// Enable, reset and start the selected timer/counter channel.
				AVR32_TC.channel[0].ccr = AVR32_TC_SWTRG_MASK | AVR32_TC_CLKEN_MASK;
			}

			static inline void
			stop()
			{
				// Disable the selected timer/counter channel.
				AVR32_TC.channel[0].ccr = AVR32_TC_CLKDIS_MASK;
			}

			static void
			initWaveform(const timer::waveform *opt);

			static void
			configureInterrupts(const timer::interrupt *bitfield);


			static inline Value
			getValue()
			{
				return AVR32_TC.channel[0].cv;
			}

			static inline Value
			getRa()
			{
				return AVR32_TC.channel[0].ra;
			}

			static inline Value
			getRb()
			{
				return AVR32_TC.channel[0].rb;
			}

			static inline Value
			getRc()
			{
				return AVR32_TC.channel[0].rc;
			}

			static inline void
			setRa(Value ra)
			{
				AVR32_TC.channel[0].ra = ra;
			}

			static inline void
			setRb(Value rb)
			{
				AVR32_TC.channel[0].ra = rb;
			}

			static inline void
			setRc(Value rc)
			{
				AVR32_TC.channel[0].rc = rc;
			}

		}; // Timer0 class
	} // avr32 namespace
} // xpcc namesapce

#endif // XPCC_AVR32__TIMER_0_HPP
