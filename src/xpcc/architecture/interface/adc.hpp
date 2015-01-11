// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_ADC_HPP
#define XPCC_PERIPHERAL_ADC_HPP

#include <xpcc/architecture/interface.hpp>

/**
 * @ingroup		interface
 * @defgroup	adc		Analog-to-Digital Converter
 */

namespace xpcc
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
 * @see AdcMaster		for a generic interrupt-based channel-mapped oversampling method.
 *
 * @author	Niklas Hauser
 * @ingroup adc
 */
class Adc : public ::xpcc::Peripheral
{
#ifdef __DOXYGEN__
public:
	/// this type may also be an enum or enum class
	typedef uint8_t Channel;

	/// describes the maximum resolution in bits
	static constexpr uint8_t Resolution;

public:
	/**
	 * Initializes the hardware and sets the datarate.
	 *
	 * @tparam	clockSource
	 * 		the currently active system clock
	 * @tparam	frequency
	 * 		the desired clock frequency in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting clock frequency
	 */
	template< class clockSource, uint32_t frequency=200000,
			Tolerance tolerance = Tolerance::TenPercent >
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

}	// namespace xpcc

#endif // XPCC_PERIPHERAL_ADC_HPP
