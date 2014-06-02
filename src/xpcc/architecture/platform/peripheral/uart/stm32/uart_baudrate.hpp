// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32_UART_BAUDRATE_HPP
#define XPCC_STM32_UART_BAUDRATE_HPP

#include "uart_base.hpp"
#include <xpcc/math/tolerance.hpp>

namespace xpcc
{

namespace stm32
{

/**
 * This class provides several helper functions to calculate the values for
 * the fractional baudrate generator.
 *
 * All of these functions are `constexpr` functions which allow compile time
 * evaluation.
 * If you really need to change the baudrate at runtime, consider calculating
 * the register values at compile time, storing them in your program, then
 * switching them at runtime.
 * This will save you a *lot* of flash and execution time.
 *
 * @author	Kevin Laeufer
 * @author	Niklas Hauser
 * @ingroup	stm32
 */
class UartBaudrate
{
public:
	/**
	 * Returns the highest Oversampling Mode that is possible for this configuration.
	 */
	static constexpr UartBase::OversamplingMode
	getOversamplingMode(uint32_t clockrate, uint32_t baudrate)
	{
		return (baudrate <= clockrate / 16) ? UartBase::OversamplingMode::By16 : UartBase::OversamplingMode::By8;
	}

	/**
	 * Calculates the best BRR value.
	 *
	 * @param	clockrate
	 * 		The APB clock frequency in Hz that the Uart module receives its clock from.
	 * @param	baudrate
	 * 		Desired baud rate.
	 */
	static constexpr uint16_t
	getBrr(uint32_t clockrate, uint32_t baudrate)
	{
		return getBrr(clockrate, baudrate,
				(baudrate <= clockrate / 16) ? UartBase::OversamplingMode::By16 : UartBase::OversamplingMode::By8);
	}

	/**
	 * Calculates the best BRR value for a given Oversampling Mode.
	 *
	 * @param	clockrate
	 * 		The APB clock frequency in Hz that the Uart module receives its clock from.
	 * @param	baudrate
	 * 		Desired baud rate.
	 * @param	oversamplingMode
	 * 		Either 8 or 16 oversamples.
	 */
	static constexpr uint16_t
	getBrr(uint32_t clockrate, uint32_t baudrate, UartBase::OversamplingMode oversample)
	{
		return (oversample == UartBase::OversamplingMode::By16) ?
				(((2 * clockrate) / baudrate + 1) / 2) :
				// By8: bit DIV_fraction[3] must be kept cleared
				((USART_BRR_DIV_MANTISSA & (((2 * clockrate) / (baudrate / 2) + 1) / 2)) |
						((USART_BRR_DIV_FRACTION & (((2 * clockrate) / (baudrate / 2) + 1) / 2)) >> 1));
	}

	static constexpr uint32_t
	getBaudrate(uint32_t clockrate, uint16_t brr, UartBase::OversamplingMode oversample)
	{
		return static_cast<uint32_t>(clockrate /
				((((brr & USART_BRR_DIV_MANTISSA) >> 4) + (brr & USART_BRR_DIV_FRACTION) / 8.f) *
						(oversample == UartBase::OversamplingMode::By16 ? 16 : 8)));
	}

	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	clockrate
	 * 		the modules clock frequency in Hz
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting baudrate
	 * t@param	oversample
	 * 		Either 8 or 16 oversamples.
	 */
	template<uint32_t clockrate, uint32_t baudrate, uint16_t tolerance,
			UartBase::OversamplingMode oversample >
	static uint16_t ALWAYS_INLINE
	getBrr()
	{
		static_assert((baudrate <= clockrate / 8),
				"Baudrate must be less or equal to the PeripheralClock divided by 8.");
		constexpr uint16_t brr = getBrr(clockrate, baudrate, oversample);
		// check if we found a prescaler which generates a baudrate within the tolerance
		xpcc::Tolerance::assertBaudrateInTolerance<
				/* nearest possible value */ getBaudrate(clockrate, brr, oversample),
				/* desired = */ baudrate,
				tolerance >();

		return brr;
	}

	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	clockrate
	 * 		the modules clock frequency in Hz
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting baudrate
	 */
	template<uint32_t clockrate, uint32_t baudrate, uint16_t tolerance>
	static uint16_t ALWAYS_INLINE
	getBrr()
	{
		return getBrr< clockrate, baudrate, tolerance, getOversamplingMode(clockrate, baudrate) >();
	}

};

} // stm32 namespace

} // xpcc namespace

#endif // XPCC_STM32_UART_BAUDRATE_HPP

