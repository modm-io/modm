// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_IODEVICE_HPP
#define XPCC_IODEVICE_HPP

namespace xpcc
{

/**
 * Abstract calls of IO devices
 *
 * @ingroup io
 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
class IODevice
{
public :
	/// The preferred behavior when the IODevice buffer is full
	enum class
	BufferBehavior
	{
		BusyWait,
		Discard,
	};

	IODevice()
	{
	}

	virtual
	~IODevice()
	{
	}

	///	Write a single character
	virtual void
	write(char c) = 0;

	/// Write a C-string
	virtual void
	write(const char* str);

	virtual void
	flush() = 0;

	/// Read a single character
	virtual bool
	read(char& c) = 0;

private :
	IODevice(const IODevice&);
};

}	// namespace xpcc

#endif // XPCC_IODEVICE_HPP
