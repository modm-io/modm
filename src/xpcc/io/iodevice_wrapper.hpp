// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_IODEVICE_WRAPPER_HPP
#define XPCC_IODEVICE_WRAPPER_HPP

#include <stdint.h>

#include "iodevice.hpp"

namespace xpcc
{

/**
 * Wrapper to use any peripheral device that supports static
 * write() and read() as an IODevice.
 * This implementation will not wait for the peripheral to be
 * ready for data.
 * If the peripheral buffers are full, the remaining data will
 * be discarded.
 *
 * Example:
 * @code
 * // configure a UART
 * xpcc::Uart0 uart;
 *
 * // wrap it into an IODevice
 * xpcc::IODeviceWrapper<xpcc::Uart0> device;
 *
 * // use this device to print a message
 * device.write("Hello");
 *
 * // or create a IOStream and use the stream to print something
 * xpcc::IOStream stream(device);
 * stream << " World!";
 * @endcode
 *
 * @ingroup		io
 * @tparam		T	Peripheral which should be wrapped
 */
template<typename T>
class IODeviceWrapper : public IODevice
{
public:
	/**
	 * \brief	Constructor
	 *
	 * \param	device	configured object
	 */
	IODeviceWrapper(const T& /*device*/)
	{
	}
	IODeviceWrapper()
	{
	}

	virtual void
	write(char c)
	{
		T::write(c);
	}

	virtual void
	write(const char *s)
	{
		char c;
		while ((c = *s++)) {
			T::write(static_cast<uint8_t>(c));
		}
	}

	virtual void
	flush()
	{
	}

	virtual bool
	read(char& c)
	{
		// FIXME
		uint8_t t;
		if (T::read(t)) {
			c = t;
			return true;
		}
		else {
			return false;
		}
	}
};

/**
 * Wrapper to use any peripheral device that supports static
 * write() and read() as an IODevice.
 * This implementation will busy wait until the peripheral is
 * ready for data.
 *
 * @ingroup		io
 * @tparam		T	Peripheral which should be wrapped
*/
template<typename T>
class IODeviceWrapperBlocking : public IODevice
{
public:
	/**
	* \brief	Constructor
	*
	* \param	device	configured object
	*/
	IODeviceWrapperBlocking(const T& /*device*/)
	{
	}
	IODeviceWrapperBlocking()
	{
	}

	virtual void
	write(char c)
	{
		while( !T::write(c) )
			;
	}

	virtual void
	write(const char *s)
	{
		char c;
		while ((c = *s++)) {
			while( !T::write(static_cast<uint8_t>(c)) )
				;
		}
	}

	virtual void
	flush()
	{
	}

	virtual bool
	read(char& c)
	{
		// FIXME
		uint8_t t;
		if (T::read(t)) {
			c = t;
			return true;
		}
		else {
			return false;
		}
	}
};

}

#endif // XPCC_IODEVICE_WRAPPER_HPP
