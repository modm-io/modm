#pragma once

#include <modm/architecture/interface/clock.hpp>

namespace modm::platform
{

// @todo integrate with /platform/clock/stm32/rcc.hpp.in
class RccProto
{
public:
	template<Rcc::AhbPrescaler Prescaler>
	static consteval int
	prescalerToValue()
	{
		switch (Prescaler)
		{
			case Rcc::AhbPrescaler::Div1: return 1;
			case Rcc::AhbPrescaler::Div2: return 2;
			case Rcc::AhbPrescaler::Div4: return 4;
			case Rcc::AhbPrescaler::Div8: return 8;
			case Rcc::AhbPrescaler::Div16: return 16;
			case Rcc::AhbPrescaler::Div64: return 64;
			case Rcc::AhbPrescaler::Div128: return 128;
			case Rcc::AhbPrescaler::Div256: return 256;
			case Rcc::AhbPrescaler::Div512: return 512;
		};
	}

	template<Rcc::Apb1Prescaler Prescaler>
	static consteval int
	prescalerToValue()
	{
		switch (Prescaler)
		{
			case Rcc::Apb1Prescaler::Div1: return 1;
			case Rcc::Apb1Prescaler::Div2: return 2;
			case Rcc::Apb1Prescaler::Div4: return 4;
			case Rcc::Apb1Prescaler::Div8: return 8;
			case Rcc::Apb1Prescaler::Div16: return 16;
		};
	}

	template<Rcc::Apb2Prescaler Prescaler>
	static consteval int
	prescalerToValue()
	{
		switch (Prescaler)
		{
			case Rcc::Apb2Prescaler::Div1: return 1;
			case Rcc::Apb2Prescaler::Div2: return 2;
			case Rcc::Apb2Prescaler::Div4: return 4;
			case Rcc::Apb2Prescaler::Div8: return 8;
			case Rcc::Apb2Prescaler::Div16: return 16;
		}
	}

	template<Rcc::UsbPrescaler Prescaler>
	static consteval float
	prescalerToValue()
	{
		switch (Prescaler)
		{
			case Rcc::UsbPrescaler::Div1: return 1;
			case Rcc::UsbPrescaler::Div1_5: return 1.5;
		}
	}
};

}  // namespace modm::platform
