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
	 * Create a 8-bit port from arbitrary pins.
	 *
	 * Be aware that this method is slow, because for every write or read
	 * cycle, every of the eight pins have to be read/written
	 * individually.
	 *
	 * This class is included here because it can be very useful sometimes,
	 * for example when connecting a LCD where speed is not the main
	 * concern.
	 *
	 * When possible prefer the native `GpioPort` implementations
	 * over this class as they are much faster and require less code.
	 *
	 * @ingroup	gpio
	 */
	template <typename T0 = GpioUnused,
			  typename T1 = GpioUnused,
			  typename T2 = GpioUnused,
			  typename T3 = GpioUnused,
			  typename T4 = GpioUnused,
			  typename T5 = GpioUnused,
			  typename T6 = GpioUnused,
			  typename T7 = GpioUnused>
	class SoftwareGpioOctet : public GpioOctet
	{
	public:
		static constexpr uint8_t width =
			(xpcc::tmp::SameType<T0, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T1, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T2, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T3, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T4, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T5, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T6, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T7, GpioUnused>::value ? 0 : 1);

	public:
		static inline void
		setOutput()
		{
			T7::setOutput();
			T6::setOutput();
			T5::setOutput();
			T4::setOutput();
			T3::setOutput();
			T2::setOutput();
			T1::setOutput();
			T0::setOutput();
		}

		static inline void
		setInput()
		{
			T7::setInput();
			T6::setInput();
			T5::setInput();
			T4::setInput();
			T3::setInput();
			T2::setInput();
			T1::setInput();
			T0::setInput();
		}

		static inline uint8_t
		read()
		{
			uint8_t value = 0;

			if (T7::read()) { value |= 0b10000000; }
			if (T6::read()) { value |= 0b01000000; }
			if (T5::read()) { value |= 0b00100000; }
			if (T4::read()) { value |= 0b00010000; }
			if (T3::read()) { value |= 0b00001000; }
			if (T2::read()) { value |= 0b00000100; }
			if (T1::read()) { value |= 0b00000010; }
			if (T0::read()) { value |= 0b00000001; }

			return value;
		}

		static inline void
		write(uint8_t data)
		{
			T7::set(data & 0b10000000);
			T6::set(data & 0b01000000);
			T5::set(data & 0b00100000);
			T4::set(data & 0b00010000);
			T3::set(data & 0b00001000);
			T2::set(data & 0b00000100);
			T1::set(data & 0b00000010);
			T0::set(data & 0b00000001);
		}
	};

	/**
	 * Create a 16-bit port from arbitrary pins.
	 *
	 * Be aware that this method is slow, because for every write or read
	 * cycle, every of the sixteen (!) pins have to be read/written
	 * individually.
	 *
	 * This class is included here because it can be very useful sometimes,
	 * for example when connecting a LCD where speed is not the main
	 * concern.
	 *
	 * When possible prefer the native `GpioPort` implementations
	 * over this class as they are much faster and require less code.
	 *
	 * @ingroup	gpio
	 */
	template <typename T0  = GpioUnused,
			  typename T1  = GpioUnused,
			  typename T2  = GpioUnused,
			  typename T3  = GpioUnused,
			  typename T4  = GpioUnused,
			  typename T5  = GpioUnused,
			  typename T6  = GpioUnused,
			  typename T7  = GpioUnused,
			  typename T8  = GpioUnused,
			  typename T9  = GpioUnused,
			  typename T10 = GpioUnused,
			  typename T11 = GpioUnused,
			  typename T12 = GpioUnused,
			  typename T13 = GpioUnused,
			  typename T14 = GpioUnused,
			  typename T15 = GpioUnused>
	class SoftwareGpioWord : public GpioWord
	{
	public:
		static constexpr uint16_t width =
			(xpcc::tmp::SameType<T0,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T1,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T2,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T3,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T4,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T5,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T6,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T7,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T8,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T9,  GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T10, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T11, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T12, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T13, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T14, GpioUnused>::value ? 0 : 1) +
			(xpcc::tmp::SameType<T15, GpioUnused>::value ? 0 : 1);

	public:
		static void
		setOutput()
		{
			T15::setOutput();
			T14::setOutput();
			T13::setOutput();
			T12::setOutput();
			T11::setOutput();
			T10::setOutput();
			T9::setOutput();
			T8::setOutput();
			T7::setOutput();
			T6::setOutput();
			T5::setOutput();
			T4::setOutput();
			T3::setOutput();
			T2::setOutput();
			T1::setOutput();
			T0::setOutput();
		}

		static void
		setInput()
		{
			T15::setInput();
			T14::setInput();
			T13::setInput();
			T12::setInput();
			T11::setInput();
			T10::setInput();
			T9::setInput();
			T8::setInput();
			T7::setInput();
			T6::setInput();
			T5::setInput();
			T4::setInput();
			T3::setInput();
			T2::setInput();
			T1::setInput();
			T0::setInput();
		}

		static uint16_t
		read()
		{
			uint16_t value = 0;

			if (T15::read()) { value |= 0b1000000000000000; }
			if (T14::read()) { value |= 0b0100000000000000; }
			if (T13::read()) { value |= 0b0010000000000000; }
			if (T12::read()) { value |= 0b0001000000000000; }
			if (T11::read()) { value |= 0b0000100000000000; }
			if (T10::read()) { value |= 0b0000010000000000; }
			if (T9::read())  { value |= 0b0000001000000000; }
			if (T8::read())  { value |= 0b0000000100000000; }
			if (T7::read())  { value |= 0b0000000010000000; }
			if (T6::read())  { value |= 0b0000000001000000; }
			if (T5::read())  { value |= 0b0000000000100000; }
			if (T4::read())  { value |= 0b0000000000010000; }
			if (T3::read())  { value |= 0b0000000000001000; }
			if (T2::read())  { value |= 0b0000000000000100; }
			if (T1::read())  { value |= 0b0000000000000010; }
			if (T0::read())  { value |= 0b0000000000000001; }

			return value;
		}

		static void
		write(uint16_t data)
		{
			T15::set(data & 0b1000000000000000);
			T14::set(data & 0b0100000000000000);
			T13::set(data & 0b0010000000000000);
			T12::set(data & 0b0001000000000000);
			T11::set(data & 0b0000100000000000);
			T10::set(data & 0b0000010000000000);
			T9::set (data & 0b0000001000000000);
			T8::set (data & 0b0000000100000000);
			T7::set (data & 0b0000000010000000);
			T6::set (data & 0b0000000001000000);
			T5::set (data & 0b0000000000100000);
			T4::set (data & 0b0000000000010000);
			T3::set (data & 0b0000000000001000);
			T2::set (data & 0b0000000000000100);
			T1::set (data & 0b0000000000000010);
			T0::set (data & 0b0000000000000001);
		}
	};
}

#endif // XPCC_SOFTWARE_GPIO_HPP
