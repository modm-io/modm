/*
 * Copyright (c) 2015, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_ADC_HPP
#define MODM_INTERFACE_ADC_HPP

#include <modm/architecture/interface/peripheral.hpp>

namespace modm
{

/**
 * Basic interface of an ADC.
 *
 * This interface only provides the very basic common functionality between modules.
 * Therefore for any special operating mode, such as interlaced operation, external trigger
 * options or DMA data management, you must refer to the platform specific documentation.
 *
 * This interface only allows for synchronous operation:
 * @code
 * // read channel 2
 * uint16_t value = Adc::readChannel(2);
 * @endcode
 *
 * The above code is equivalent to this:
 * @code
 * // start a new conversion on channel 3
 * Adc::setChannel(3);
 * Adc::startConversion();
 *
 * while(!Adc::isConversionFinished())
 *     ;
 *
 * // read the converted value
 * uint16_t value = Adc::getValue();
 * @endcode
 *
 * @see AdcInterrupt	for the asynchronous method.
 *
 * @author	Niklas Hauser
 * @ingroup modm_architecture_adc
 */
class Adc : public ::modm::PeripheralDriver
{
#ifdef __DOXYGEN__
public:
	/// this type may also be an enum or enum class
	typedef uint8_t Channel;

	/// describes the maximum resolution in bits
	static constexpr uint8_t Resolution;

public:
	/**
	 * Configures the input signals and connects them.
	 *
	 * @tparam	Signals
	 *		At least one input signal is required and can be passed out-of-order.
	 */
	template< class... Signals >
	static void
	connect();

	/**
	 * Initializes the hardware and sets the datarate.
	 *
	 * @tparam	SystemClock
	 * 		the currently active system clock
	 * @tparam	frequency
	 * 		the desired clock frequency in Hz
	 * @tparam	tolerance
	 * 		the allowed relative tolerance for the resulting clock frequency
	 */
	template< class SystemClock, frequency_t frequency=200_kHz, percent_t tolerance=10_pct >
	static void
	initialize();

	static void
	disable();


	/// starts a conversion on the selected analog channel
	/// @pre A channel must be selected with setChannel().
	static void
	startConversion();

	/// @return	`true` if all conversion have finished,
	///			`false` otherwise
	static bool
	isConversionFinished();

	/// @return the most recent result of any conversion
	static uint16_t
	getValue();


	/**
	 * Convenience function:
	 *
	 * 1. set the channel
	 * 2. starts the conversion
	 * 3. waits until conversion is complete
	 * @return	result of the conversion
	 */
	static uint16_t
	readChannel(Channel channel);


	/**
	 * Analog channel selection.
	 *
	 * @return	`true` if the channel exists and was available,
	 *			`false` otherwise
	 */
	static bool
	setChannel(Channel channel);

	/// @return the currently selected analog channel
	static uint8_t
	getChannel();


	/**
	 * Enables free running mode
	 *
	 * The ADC will continously start conversions and provide the most
	 * recent result in the ADC register.
	 */
	static void
	enableFreeRunningMode();

	/**
	 * Disables free running mode
	 *
	 * The ADC will do only one sample and stop. The result will be in
	 * the ADC register.
	 */
	static void
	disableFreeRunningMode();


	/// Change the presentation of the ADC conversion result to left adjusted.
	static void
	setLeftAdjustResult();

	/// Change the presentation of the ADC conversion result to right adjusted.
	static void
	setRightAdjustResult();
#endif
};

}	// namespace modm

#endif // MODM_INTERFACE_ADC_HPP
