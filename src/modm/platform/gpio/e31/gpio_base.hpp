/*
 * Copyright (c) 2019, Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_E31_GPIO_BASE_HPP
#define MODM_E31_GPIO_BASE_HPP

#include <cstdint>

#include <modm/architecture/interface/gpio.hpp>

namespace modm::platform
{

/// @ingroup modm_platform_gpio
struct Gpio
{
	enum class
	InputTrigger
	{
		RisingEdge,
		FallingEdge,
		Low,
		High
	};
};
/// @endcond

template<uint32_t portBaseAddress>
class GpioPort : public ::modm::GpioPort 
{

private:

    // Register definitions for the E31 GPIO Core
    static inline volatile uint32_t* const REG_INPUT_VAL  = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x00);
    static inline volatile uint32_t* const REG_INPUT_EN   = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x04);
    static inline volatile uint32_t* const REG_OUTPUT_EN  = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x08);
    static inline volatile uint32_t* const REG_OUTPUT_VAL = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x0C);
    static inline volatile uint32_t* const REG_PULLUP_EN  = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x10);
    static inline volatile uint32_t* const REG_DRIVE_STR  = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x14);
    static inline volatile uint32_t* const REG_RISE_IE    = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x18);
    static inline volatile uint32_t* const REG_RISE_IP    = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x1C);
    static inline volatile uint32_t* const REG_FALL_IE    = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x20);
    static inline volatile uint32_t* const REG_FALL_IP    = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x24);
    static inline volatile uint32_t* const REG_HIGH_IE    = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x28);
    static inline volatile uint32_t* const REG_HIGH_IP    = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x2C);
    static inline volatile uint32_t* const REG_LOW_IE     = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x30);
    static inline volatile uint32_t* const REG_LOW_IP     = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x34);
    static inline volatile uint32_t* const IOF_EN         = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x38);
    static inline volatile uint32_t* const IOF_SEL        = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x3C);
    static inline volatile uint32_t* const REG_OUTPUT_INV = reinterpret_cast<volatile uint32_t*>(portBaseAddress+0x40);

public:

    template<uint8_t pinOffset> 
    class GpioPin : public Gpio ,public ::modm::GpioIO
    {
	public:

	    inline static void setOutput()
        {
            *REG_INPUT_EN  &= ~(1<<pinOffset);
            *REG_OUTPUT_EN |=  (1<<pinOffset);
        }
        inline static void set() 
        {
            *REG_OUTPUT_VAL |= (1<<pinOffset);
        }

        inline static void reset()
        {
            *REG_OUTPUT_VAL &= ~(1<<pinOffset);
        }

        inline static void set(bool status) 
        {
            if(status)
            {
                set();
            }
            else
            {
                reset();
            }
            
        }

	    inline static void setOutput(bool status)
        {
            setOutput();
            set(status);
        }

		inline static void setInverted(bool inverted)
		{
			if(inverted)
			{
				*REG_OUTPUT_INV |= (1<<pinOffset);
			}
			else
			{
				*REG_OUTPUT_INV &= ~(1<<pinOffset);
			}
			
		}
	
	    inline static void toggle() {
		    if (isSet()) { reset(); }
		    else         { set();   }
	    }

	    inline static bool isSet() {
            return (*REG_OUTPUT_VAL & (1<<pinOffset)); 
        }
	
	    inline static void setInput() 
        {
            *REG_OUTPUT_EN &= ~(1<<pinOffset);
            *REG_INPUT_EN  |=  (1<pinOffset);
        }


	    inline static bool read() 
        {
            return (*REG_INPUT_VAL & (1<pinOffset));
        }

        inline static void enableExternalInterrupt(const InputTrigger trigger)
        {
            switch(trigger)
			{
				case InputTrigger::FallingEdge:
				{
					*REG_FALL_IE |= (1<<pinOffset);
				}
				break;
				case InputTrigger::RisingEdge:
				{
					*REG_RISE_IE |= (1<<pinOffset);
				}
				break;
				case InputTrigger::High:
				{
					*REG_HIGH_IE |= (1<<pinOffset);
				}
				break;
				case InputTrigger::Low:
				{
					*REG_LOW_IE |= (1<<pinOffset);
				}
				break;
			}
        }

		inline static void disableExternalInterrupt(const InputTrigger trigger)
        {
            switch(trigger)
			{
				case InputTrigger::FallingEdge:
				{
					*REG_FALL_IE &= ~(1<<pinOffset);
				}
				break;
				case InputTrigger::RisingEdge:
				{
					*REG_RISE_IE &= ~(1<<pinOffset);
				}
				break;
				case InputTrigger::High:
				{
					*REG_HIGH_IE &= ~(1<<pinOffset);
				}
				break;
				case InputTrigger::Low:
				{
					*REG_LOW_IE &= ~(1<<pinOffset);
				}
				break;
			}
        }

        inline static bool getExternalInterruptFlag(const InputTrigger trigger)
		{
            switch(trigger)
			{
				case InputTrigger::FallingEdge:
					return (*REG_FALL_IP & (1<<pinOffset));
				case InputTrigger::RisingEdge:
					return (*REG_RISE_IP & (1<<pinOffset));
				case InputTrigger::High:
					return (*REG_HIGH_IP & (1<<pinOffset));
				case InputTrigger::Low:
					return (*REG_LOW_IP & (1<<pinOffset));
			}
        }
		
        inline static void acknowledgeExternalInterruptFlag(const InputTrigger trigger)
		{
			switch(trigger)
			{
				case InputTrigger::FallingEdge:
				{
					*REG_FALL_IE |= (1<<pinOffset);
				}
				break;
				case InputTrigger::RisingEdge:
				{
					*REG_RISE_IE |= (1<<pinOffset);
				}
				break;
				case InputTrigger::High:
				{
					*REG_HIGH_IE |= (1<<pinOffset);
				}
				break;
				case InputTrigger::Low:
				{
					*REG_LOW_IE |= (1<<pinOffset);
				}
				break;
			}
		}
    };

};

}

#endif //MODM_E31_GPIO_BASE_HPP