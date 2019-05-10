/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IODEVICE_HPP
#define MODM_IODEVICE_HPP

namespace modm
{

/**
 * Abstract calls of IO devices
 *
 * @ingroup modm_io
 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
class IODevice
{
public:
	virtual
	~IODevice() = default;

	///	Write a single character
	virtual void
	write(char c) = 0;

	/// Write a C-string
	virtual inline void
	write(const char* str)
	{
		char c;
		while ( (c = *str++) ) write(c);
	}

	virtual void
	flush() = 0;

	/// Read a single character
	virtual bool
	read(char& c) = 0;
};

}	// namespace modm

#endif // MODM_IODEVICE_HPP
