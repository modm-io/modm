#ifndef XPCC_LPC11__TIMER_16_0_HPP
#define XPCC_LPC11__TIMER_16_0_HPP

#include <stdint.h>
#include <xpcc/utils.hpp>

#include "timer_registers.hpp"

#include "../device.h"

namespace xpcc
{
	namespace lpc11
	{
		class Timer16_0
		{
		public:
			typedef uint16_t Value;

			static void
			enable() {
				// provide clock
				LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

				// Set enable, reset reset.
				LPC_TMR16B0->TCR = TCR_CEn;
			}

			static void
			disable() {
				// Reset enable
				LPC_TMR16B0->TCR = 0;
			}

			static void
			pause();

			static void
			start();

			static void
			setModePwm(uint8_t matchEnable, Value period) {
				if (matchEnable & 0x01)
				{
					LPC_IOCON->PIO0_8           &= ~0x07;
					LPC_IOCON->PIO0_8           |= 0x02;		/* Timer0_16 MAT0 			*/
				}
				/* If match1 is enabled, set the output */
				if (matchEnable & 0x02)
				{
					LPC_IOCON->PIO0_9           &= ~0x07;
					LPC_IOCON->PIO0_9           |= 0x02;		/* Timer0_16 MAT1 			*/
				}
				/* If match2 is enabled, set the output */
				if (matchEnable & 0x04)
				{
					LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
					LPC_IOCON->SWCLK_PIO0_10 |= 0x03;		/* Timer0_16 MAT2 */
				}

				// usr channel 3 to set the PWM period
				LPC_TMR16B0->PWMC = (1<<3)|(matchEnable);

				LPC_TMR16B0->MR3 	= period;

				// Initial value: 50% PWM
				LPC_TMR16B0->MR0	= period/2;
				LPC_TMR16B0->MR1	= period/2;
				LPC_TMR16B0->MR2	= period/2;

				/* Set match control register */
				// Reset on MR3: the TC will be reset if MR3 matches it.
				LPC_TMR16B0->MCR = 1<<10;// | 1<<9;				/* Reset on MR3 */
			}

			static void
			setPrescaler(Value prescaler) {
				LPC_TMR16B0->PR = prescaler;
			}

			static void
			setOverflow(Value overflow);

			static void
			configureOutputChannel(uint32_t channel);

			static inline void
			setMatchValue(uint32_t channel, uint16_t value)
			{
				switch (channel)
				{
				case 0:
					LPC_TMR16B0->MR0	= value;
					break;
				case 1:
					LPC_TMR16B0->MR1	= value;
					break;
				case 2:
					LPC_TMR16B0->MR2	= value;
					break;
				default:
					break;
				}
			}

			inline void
			applyAndReset() {
				LPC_TMR16B0->TCR = TCR_CRst;
			}

			static ALWAYS_INLINE Value
			getValue() {
				return LPC_TMR16B0->TC;
			}

			static ALWAYS_INLINE void
			setValue(uint16_t value) {
				LPC_TMR16B0->TC = value;
			}
		};

	}
}

#endif // XPCC_LPC11__TIMER_16_0_HPP
