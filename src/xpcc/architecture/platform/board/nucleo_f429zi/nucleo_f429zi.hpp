/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */

// NUCLEO-F429ZI
// Nucleo kit for STM32F429ZI
// http://www.st.com/web/en/catalog/tools/PF262637
//

#ifndef XPCC_STM32_NUCLEO_F429ZI_HPP
#define XPCC_STM32_NUCLEO_F429ZI_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#define XPCC_BOARD_HAS_LOGGER

using namespace xpcc::stm32;

namespace Board
{

/// STM32F429 running at 180MHz from the external 8MHz STLink clock
struct systemClock
{
	static constexpr uint32_t Frequency = MHz180;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	static bool inline
	enable()
	{
		ClockControl::enableExternalClock(); // 8 MHz
		ClockControl::enablePll(
			ClockControl::PllSource::ExternalClock,
			4,      // 8MHz / N=4 -> 2MHz
			180,    // 2MHz * M=180 -> 360MHz
			2,      // 360MHz / P=2 -> 180MHz = F_cpu
			7       // 360MHz / Q=7 -> ~51MHz = F_usb => bad for USB
		);
		ClockControl::setFlashLatency(Frequency);
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div4);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div2);
		// update clock frequencies
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

		return true;
	}
};

// Arduino Footprint
using A0 = GpioA3;
using A1 = GpioC0;
using A2 = GpioC3;
using A3 = GpioF3;
using A4 = GpioF5;
using A5 = GpioF10;
// Zio Footprint
using A6 = GpioB1;
using A7 = GpioC2;
using A8 = GpioF4;

// Arduino Footprint
using D0  = GpioG9;
using D1  = GpioG14;
using D2  = GpioF15;
using D3  = GpioF13;
using D4  = GpioF14;
using D5  = GpioE11;
using D6  = GpioE9;
using D7  = GpioF13;
using D8  = GpioF12;
using D9  = GpioD15;
using D10 = GpioD14;
using D11 = GpioA7;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB9;
using D15 = GpioB8;
// Zio Footprint
using D16 = GpioC6;
using D17 = GpioB15;
using D18 = GpioB13;
using D19 = GpioB12;
using D20 = GpioA15;
using D21 = GpioC7;
using D22 = GpioB5;
using D23 = GpioB3;
using D24 = GpioA4;
using D25 = GpioB4;
using D26 = GpioB6;
using D27 = GpioB2;
using D28 = GpioD13;
using D29 = GpioD12;
using D30 = GpioD11;
using D31 = GpioE2;
using D32 = GpioA0;
using D33 = GpioB0;
using D34 = GpioE0;
using D35 = GpioB11;
using D36 = GpioB10;
using D37 = GpioE15;
using D38 = GpioE14;
using D39 = GpioE12;
using D40 = GpioE10;
using D41 = GpioE7;
using D42 = GpioE8;
using D43 = GpioC8;
using D44 = GpioC9;
using D45 = GpioC10;
using D46 = GpioC11;
using D47 = GpioC12;
using D48 = GpioD2;
using D49 = GpioG2;
using D50 = GpioG3;
using D51 = GpioD7;
using D52 = GpioD6;
using D53 = GpioD5;
using D54 = GpioD4;
using D55 = GpioD3;
using D56 = GpioE2;
using D57 = GpioE4;
using D58 = GpioE5;
using D59 = GpioE6;
using D60 = GpioE3;
using D61 = GpioF8;
using D62 = GpioF7;
using D63 = GpioF9;
using D64 = GpioG1;
using D65 = GpioG0;
using D66 = GpioD1;
using D67 = GpioD0;
using D68 = GpioF0;
using D69 = GpioF1;
using D70 = GpioF2;
using D71 = GpioA7;
using D72 = xpcc::GpioUnused;

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputB14;	// LED3 [Red]
using Leds = xpcc::SoftwareGpioPort< LedRed, LedBlue, LedGreen >;

namespace stlink
{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = Usart3;
}

// Create an IODeviceWrapper around the Uart Peripheral we want to use
using LoggerDevice = xpcc::IODeviceWrapper< stlink::Uart, xpcc::IOBuffer::BlockIfFull >;


inline void
initialize()
{
    systemClock::enable();
    xpcc::cortex::SysTickTimer::initialize<systemClock>();

    stlink::Tx::connect(stlink::Uart::Tx);
    stlink::Rx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
    stlink::Uart::initialize<systemClock, xpcc::Uart::Baudrate::B115200>(12);

    LedGreen::setOutput(xpcc::Gpio::Low);
    LedBlue::setOutput(xpcc::Gpio::Low);
    LedRed::setOutput(xpcc::Gpio::Low);

    Button::setInput();
    Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
    Button::enableExternalInterrupt();
//  Button::enableExternalInterruptVector(12);
}

}

#endif  // XPCC_STM32_NUCLEO_F429ZI_HPP
