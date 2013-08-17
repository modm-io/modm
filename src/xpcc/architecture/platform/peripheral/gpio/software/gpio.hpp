// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_GPIO_HPP
#define XPCC_SOFTWARE_GPIO_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/peripheral/gpio.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>

/**
 * Some useful implementation of GPIO in software.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */

namespace xpcc
{
	/**
	 * Dummy implementation of an I/O pin.
	 *
	 * This class can be used when a pin is not required. All functions
	 * are dummy functions which do nothing. `read()` will always
	 * return `false`.
	 *
	 * For example when creating a software SPI with the xpcc::SoftwareSimpleSpi
	 * class and the return channel (MISO - Master In Slave Out) is not needed,
	 * a good way is to use this class as a parameter when defining the
	 * SPI class.
	 *
	 * Example:
	 * @code
	 * #include <xpcc/architecture/platform.hpp>
	 *
	 * namespace pin
	 * {
	 *     typedef GpioOutputD7 Clk;
	 *     typedef GpioOutputD5 Mosi;
	 * }
	 *
	 * SoftwareSimpleSpi< pin::Clk, pin::Mosi, xpcc::GpioUnused > Spi;
	 *
	 * ...
	 * Spi::write(0xaa);
	 * @endcode
	 *
	 * @ingroup	gpio
	 */
	class GpioUnused : GpioIO
	{
	public:
		ALWAYS_INLINE static void
		setOutput()
		{
		}

		ALWAYS_INLINE static void
		setOutput(bool)
		{
		}

		ALWAYS_INLINE static void
		setInput()
		{
		}

		ALWAYS_INLINE static void
		set()
		{
		}

		ALWAYS_INLINE static void
		set(bool)
		{
		}

		ALWAYS_INLINE static void
		reset()
		{
		}

		ALWAYS_INLINE static void
		toggle()
		{
		}

		/// Always returns `false`
		ALWAYS_INLINE static bool
		read()
		{
			return false;
		}
	};

	/**
	 * Invert a pin in software.
	 *
	 * This template can be used the invert the logic level of a normal
	 * pin template.
	 *
	 * Example:
	 * @code
	 * #include <xpcc/architecture/platform.hpp>
	 *
	 * namespace pin
	 * {
	 *     typedef GpioOutputB0 Led;
	 * }
	 *
	 * typedef xpcc::GpioInverted< pin::Led > Led;
	 *
	 * ...
	 * Led::reset();
	 * @endcode
	 *
	 * @ingroup	gpio
	 */
	template < class Pin >
	class GpioInverted : GpioIO
	{
	public:
		ALWAYS_INLINE static void
		setOutput()
		{
			Pin::setOutput();
		}

		ALWAYS_INLINE static void
		setOutput(bool value)
		{
			Pin::setOutput(!value);
		}

		ALWAYS_INLINE static void
		setInput()
		{
			Pin::setInput();
		}

		ALWAYS_INLINE static void
		set()
		{
			Pin::reset();
		}

		ALWAYS_INLINE static void
		set(bool value)
		{
			Pin::set(!value);
		}

		ALWAYS_INLINE static void
		reset()
		{
			Pin::set();
		}

		ALWAYS_INLINE static void
		toggle()
		{
			Pin::toggle();
		}

		ALWAYS_INLINE static bool
		read()
		{
			return !Pin::read();
		}
	};


	/**
	 * Create an up to 16-bit port from arbitrary pins.
	 *
	 * Be aware that this method is slow, because for every write or read
	 * cycle, every of the eight pins have to be read/written individually.
	 *
	 * This class is included here because it can be very useful sometimes,
	 * for example when connecting a LCD where speed is not the main concern.
	 *
	 * Whenever possible prefer the native `GpioPort` implementations
	 * over this class as they are much faster and require less code.
	 *
	 * This class optimizes the type size for the `read()` and `write()`
	 * methods.
	 * Supplying up to 8 Gpios will use `uint8_t`, up to 16 Gpios will use
	 * `uint16_t`.
	 *
	 * @tparam Gpios	Up to 16 GpioIO classes, ordered MSB to LSB
	 * @ingroup	gpio
	 */
	template<typename... Gpios>
	class SoftwareGpioPort : public GpioPort {};

	/// @private
	template<typename Gpio, typename... Gpios>
	class SoftwareGpioPort<Gpio, Gpios...> : private SoftwareGpioPort<Gpios...>
	{
	public:
		static constexpr uint8_t width = 1 + SoftwareGpioPort<Gpios...>::width;
	private:
		typedef typename xpcc::tmp::Select< (width > 8),
											uint16_t,
											uint8_t >::Result Index;
		typedef Index Size;

	public:
		static void
		setOutput()
		{
			Gpio::setOutput();
			SoftwareGpioPort<Gpios...>::setOutput();
		}

		static void
		setInput()
		{
			Gpio::setInput();
			SoftwareGpioPort<Gpios...>::setInput();
		}

		static Size
		read()
		{
			Size data = SoftwareGpioPort<Gpios...>::read();
			if (Gpio::read()) {
				data |= (1 << (width-1));
			}
			return data;
		}

		static void
		write(Size data)
		{
			Gpio::set(data & (1 << (width-1)));
			SoftwareGpioPort<Gpios...>::write(data);
		}
	};

	/// @private
	template<>
	class SoftwareGpioPort<>
	{
	public:
		static constexpr uint8_t width = 0;

		static void
		setOutput() {}

		static void
		setInput() {}

		static uint8_t
		read() { return 0; }

		static void
		write(uint8_t /*data*/) {}
	};
}

#endif // XPCC_SOFTWARE_GPIO_HPP
