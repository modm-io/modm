#ifndef MODM_STM32_DAC_HPP
#define MODM_STM32_DAC_HPP

#include <stdint.h>
#include "../device.hpp"
#include <modm/platform/gpio/connector.hpp>

namespace modm {

namespace platform {

/**
 * Digital-to-Analog Converter (DAC)
 *
 * Supports simple synchronous access to the 2-channel DAC found on most STM32
 * microcontrollers.
 *
 * @author Jeff McBride
 * @ingroup modm_platform_dac
 */
class Dac {
public:
	/**
	 * The DAC has two output channels, Channel1 and Channel2
	 */
	enum class Channel : uint8_t {
		Channel1 = 0,
		Channel2 = 1
	};

	template< template<Peripheral _> class... Signals >
	static void
	connect()
	{
		using Connector = GpioConnector<Peripheral::Dac, Signals...>;
		Connector::connect();
	}

	/**
	 * Initialize the D/A converter.
	 *
	 * Enables the RCC clock output for the DAC
	 */
	static inline void initialize();

	/** Enable DAC output for the given channel
	 *
	 * @param chan    The channel to be enabled
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static inline void enableChannel(Channel chan);

	/**
	 * Disable DAC output for the given channel
	 *
	 * @param chan    The channel to be disabled
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static inline void disableChannel(Channel chan);

	/**
	 * Control the output buffer setting
	 *
	 * @param chan    The channel to enable/disable the output buffer on
	 * @param enable  true to turn on the output buffer, false to turn it off
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static inline void enableOutputBuffer(Channel chan, bool enable);

	/**
	 * Set the output voltage for a DAC channel
	 *
	 * @param chan    The channel to set
	 * @param value   The 12-bit DAC output value, range 0-4095.
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static inline void setOutput(Channel chan, uint16_t value);

	/**
	 * Set output value for Channel1
	 *
	 * Equivalent to setOutput(modm::platform::Dac::Channel1, value)
	 *
	 * @param value   The 12-bit DAC output value, range 0-4095
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static inline void setOutput1(uint16_t value);

	/**
	 * Set output value for Channel2
	 *
	 * Equivalent to setOutput(modm::platform::Dac::Channel2, value)
	 *
	 * @param value   The 12-bit DAC output value, range 0-4095
	 *
	 * @pre The DAC clock must be enabled with initialize()
	 */
	static inline void setOutput2(uint16_t value);

	/// Get the channel for a Pin
	template< class Gpio >
	static inline constexpr Channel
	getPinChannel()
	{
		constexpr int8_t channel{Gpio::template DacChannel<Peripheral::Dac>};
		static_assert(channel >= 0, "Dac does not have a channel for this pin!");
		return Channel(channel);
	}

};

} // namespace platform

} // namespace modm

#include "dac_impl.hpp"

#endif // MODM_STM32_DAC_HPP