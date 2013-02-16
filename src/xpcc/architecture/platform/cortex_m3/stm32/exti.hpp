#ifndef XPCC_STM32__EXTI_HPP
#define XPCC_STM32__EXTI_HPP

#include <stdint.h>
#include "device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * \brief	External Interrupts from GPIO pins
		 *
		 * This class simplifies the usage of external interrupts from
		 * GPIO pins. Keep the restrictions of the STM32 External
		 * Interrupts in mind!
		 *
		 * Although each GPIO pin can generate an interrupt, there are
		 * 16 interrupt lines and only seven interrupts handlers that
		 * are shared by all GPIO pins.
		 *
		 * Each of the 16 interrupt lines can only be connected to one
		 * GPIO pin at the same time. Interrupt line 0 can only be
		 * connected to one of PA0, PB0, PC0, ..., PI0.
		 * The same is true for interrupt line 1 which can only be
		 * connected to one of PA1, PB1, PC1, ..., PI1, et cetera.
		 *
		 * Interrupt lines 0 to 4 will trigger
		 *   EXTI0_IRQHandler
		 *   EXTI1_IRQHandler
		 *   EXTI2_IRQHandler
		 *   EXTI3_IRQHandler
		 *   EXTI4_IRQHandler, respectively.
		 * interrupt lines 5 to 9 will trigger
		 *   EXTI9_5_IRQHandler
		 * and interrupt lines 10 to 15 will trigger
		 *   EXTI15_10_IRQHandler
		 *
		 * When PA0 should generate an interrupt PB0 cannot as both pins
		 * share the same interrupt line 0.
		 *
		 * Usage:
		 *
		 * 	ExtInt::enable(ExtInt::Pin::PE11);
		 * 	ExtInt::setMode(ExtInt::Pin::PE11, ExtInt::Mode::Rising);
		 * 	ExtInt::enableInterrupt(ExtInt::Pin::PE11);
		 * 	ExtInt::enableInterruptVector(ExtInt::Pin::PE11, 14);
		 *
		 * 	Declare the appropriate interrupt handler.
		 * 	You must manually choose the interrupt handler that serves
		 * 	the interrupt line you implicitly chose with the GPIO pin.
		 *
		 * 	extern "C" void
		 * 	EXTI15_10_IRQHandler(void)
		 * 	{
		 * 		// Reset interrupt after it triggered
		 * 		ExtInt::resetInterruptFlag(ExtInt::Pin::PE11);
		 * 	}
		 *
		 */
		class ExtInt
		{
		public:
			enum class Line : uint_fast8_t /* IRQn_Type */
			{
				LINE0 = EXTI0_IRQn,
				LINE1 = EXTI1_IRQn,
				LINE2 = EXTI2_IRQn,
				LINE3 = EXTI3_IRQn,
				LINE4 = EXTI4_IRQn,
				LINE9_5 = EXTI9_5_IRQn,
				LINE15_10 = EXTI15_10_IRQn,
			};

			enum class Pin : uint_fast8_t /* Line */
			{
				PA0, PA1, PA2, PA3,	PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
				PB0, PB1, PB2, PB3,	PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
				PC0, PC1, PC2, PC3,	PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
				PD0, PD1, PD2, PD3,	PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
				PE0, PE1, PE2, PE3,	PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
				PF0, PF1, PF2, PF3,	PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
				PG0, PG1, PG2, PG3,	PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15,
				PH0, PH1, PH2, PH3,	PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15,
				PI0, PI1, PI2, PI3,	PI4, PI5, PI6, PI7, PI8, PI9, PI10, PI11, PI12, PI13, PI14, PI15,
			};

			enum class Mode : uint8_t
			{
				Rising,
				Falling,
				RisingFalling,
			};

		public:
			/**	\brief	Enables the External Interrupt for a pin
			 *
			 */
			static inline void
			enable(const Pin pin)
			{
				// Enable SYSCFG
				RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

				const uint_fast8_t pinnr = getPinnrFromPin(pin);

				// PA[x], x =  0 ..  3 maps to EXTICR[0]
				// PA[x], x =  4 ..  7 maps to EXTICR[1]
				// PA[x], x =  8 .. 11 maps to EXTICR[2]
				// PA[x], x = 12 .. 15 maps to EXTICR[3]

				const uint_fast8_t idx = (pinnr & 0x0f) >> 2;
				const uint16_t mask = ((pinnr & 0xf0) >> 4) << ( (pinnr & 0x03)  << 2);

				SYSCFG->EXTICR[idx] |= mask;
			}

			static inline void
			setMode(const Pin pin, const Mode mode)
			{
				const uint32_t mask = getLineMaskFromPin(pin);
				switch (mode)
				{
				case Mode::Rising:
					EXTI->RTSR |=  (mask);
					EXTI->FTSR &= ~(mask);
					break;
				case Mode::Falling:
					EXTI->RTSR &= ~(mask);
					EXTI->FTSR |=  (mask);
					break;
				case Mode::RisingFalling:
					EXTI->RTSR |=  (mask);
					EXTI->FTSR |=  (mask);
					break;
				}
			}

			static inline void
			enableInterruptVector(const Pin pin, const uint32_t priority)
			{
				const IRQn_Type irq = getIrqnFromPin(pin);

				// Set priority for the interrupt vector
				NVIC_SetPriority(irq, priority);

				// register IRQ at the NVIC
				NVIC_EnableIRQ(irq);
			}

			static inline void
			disableInterruptVector(const Pin pin)
			{
				const IRQn_Type irq = getIrqnFromPin(pin);

				NVIC_DisableIRQ(irq);
			}

			static inline void
			enableInterrupt(const Pin pin)
			{
				EXTI->IMR |=  (getLineMaskFromPin(pin));
			}

			static void
			disableInterrupt(const Pin pin)
			{
				EXTI->IMR &= ~(getLineMaskFromPin(pin));
			}

		public:
			static inline bool
			getInterruptFlag(const Pin pin)
			{
				return (EXTI->PR & (getLineMaskFromPin(pin)));
			}

			/**	\brief	Reset the interrupt flag in the interrupt routine. */
			static inline void
			resetInterruptFlag(const Pin pin)
			{
				// Flags are cleared by writing a one to the flag position.
				EXTI->PR |= (getLineMaskFromPin(pin));
			}

		private:
			/** \brief	Get the IRQ number which a pin could trigger. */
			static inline IRQn_Type
			getIrqnFromPin(const Pin pin)
			{
				const Line line = getLineFromPin(pin);
				IRQn_Type irq = static_cast<IRQn_Type>(line);

				return irq;
			}

			/**
			 * \brief	Get the interrupt line to which a pin is connected
			 *
			 * EXTI0 : Px0
			 * EXTI1 : Px1
			 * EXTI2 : Px2
			 * EXTI3 : Px3
			 * EXTI4 : Px4
			 * EXTI9_5   : Px5  to Px9
			 * EXTI15_10 : Px10 to Px15
			 */
			static inline Line
			getLineFromPin(const Pin pin)
			{
				const uint_fast8_t pinnr = getPinnrFromPin(pin);
				const uint_fast8_t port_pin = pinnr & 0x0f; // 0 .. 15

				switch (port_pin) {
					case 0:
						return Line::LINE0;
						break;
					case 1:
						return Line::LINE1;
						break;
					case 2:
						return Line::LINE2;
						break;
					case 3:
						return Line::LINE3;
						break;
					case 4:
						return Line::LINE4;
						break;
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
						return Line::LINE9_5;
						break;
					default:
						return Line::LINE15_10;
						break;
				} // switch
				// never reached
			}

			static inline uint32_t
			getLineMaskFromPin(const Pin pin)
			{
				return (1 << (static_cast<uint8_t>(pin) & 0x0f));
			}

			static inline uint_fast8_t
			getPinnrFromPin(const Pin pin)
			{
				return static_cast<uint_fast8_t>(pin);
			}
		}; // ExtInt class
	} // stm32 namespace
} // xpcc namespace


#endif /* XPCC_STM32__EXTI_HPP */
