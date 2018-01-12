/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2016, Niklas Hauser
 * Copyright (c) 2012, 2015-2016, Sascha Schade
 * Copyright (c) 2015-2016, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IOSTREAM_HPP
#define MODM_IOSTREAM_HPP

#include <cstddef>

#include <modm/architecture/detect.hpp>
#include <modm/architecture/utils.hpp>

#include <stdarg.h>	// va_list

#include "iodevice.hpp"
#include "iodevice_wrapper.hpp"

namespace modm
{

/**
 * This formats all primary types into a string stream for
 * output or it reads values from a input and converts them to
 * a given type;
 *
 * @ingroup io
 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
class IOStream
{
public:
	/**
	 * @param	device	device to write the stream to
	 *
	 * @code
	 *	MyIODevice device;
	 *	IOStream stream( device );
	 * @endcode
	 */
	IOStream(IODevice& device);

	inline IOStream&
	write(char c)
	{
		this->device->write(c);
		return *this;
	}

	static constexpr char eof = -1;

	/// Reads one character and returns it if available. Otherwise, returns IOStream::eof.
	inline IOStream&
	get(char& c)
	{
		if(!this->device->read(c)) {
			c = IOStream::eof;
		}
		return *this;
	}

	/// reads characters into NULL delimited c string
	/// in contrast to the standard implementation, this does not care about
	/// newline characters in the input
	inline IOStream&
	get(char* s, size_t n)
	{
		if(n < 1) {
			return *this;
		}
		char cc;
		size_t ii;
		for(ii = 0; ii < (n-1); ++ii) {
			if(this->device->read(cc)) {
				s[ii] = cc;
			} else {
				break;
			}
		}
		s[ii] = '\0';
		return *this;
	}

	template<size_t N>
	inline IOStream&
	get(char (&s)[N])
	{
		return this->get(s, N);
	}



	inline IOStream&
	flush()
	{
		this->device->flush();
		this->mode = Mode::Ascii;
		return *this;
	}

	/// set the output mode to binary style for `char` and `char*`
	modm_always_inline IOStream&
	bin()
	{
		this->mode = Mode::Binary;
		return *this;
	}

	/// set the output mode to hexadecimal style for `char` and `char*`
	modm_always_inline IOStream&
	hex()
	{
		this->mode = Mode::Hexadecimal;
		return *this;
	}

	/// set the output mode to ASCII style for `char` and `char*`
	modm_always_inline IOStream&
	ascii()
	{
		this->mode = Mode::Ascii;
		return *this;
	}


	IOStream&
	operator << (const unsigned char& v)
	{
		if (this->mode == Mode::Ascii) {
			this->writeInteger(static_cast<uint16_t>(v));
		}
		else if (this->mode == Mode::Binary) {
			this->writeBin(v);
		}
		else {
			this->writeHex(v);
		}
		return *this;
	}

	IOStream&
	operator << (const bool& v)
	{
		switch (this->mode)
		{
			case Mode::Ascii:
				this->device->write(v ? "true" : "false");
				break;
			case Mode::Binary:
				// upper nibble
				this->device->write('0');
				this->device->write('0');
				this->device->write('0');
				this->device->write('0');

				// lower nibble
				this->device->write('0');
				this->device->write('0');
				// fallthrough
			case Mode::Hexadecimal:
				this->device->write('0');
				this->device->write(v ? '1' : '0');
				break;
		}
		return *this;
	}

	IOStream&
	operator << (const char& v)
	{
		if (this->mode == Mode::Ascii) {
			this->device->write(v);
		}
		else if (this->mode == Mode::Binary) {
			this->writeBin(v);
		}
		else {
			this->writeHex(v);
		}
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const uint16_t& v)
	{
		if (this->mode == Mode::Ascii) {
			this->writeInteger(v);
		}
		else if (this->mode == Mode::Binary) {
			this->writeBin(static_cast<uint8_t>(v >> 8));
			this->writeBin(static_cast<uint8_t>(v & 0xff));
		}
		else {
			this->writeHex(static_cast<uint8_t>(v >> 8));
			this->writeHex(static_cast<uint8_t>(v));
		}
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const int16_t& v)
	{
		if (this->mode == Mode::Ascii) {
			this->writeInteger(v);
		}
		else if (this->mode == Mode::Binary) {
			this->writeBin(static_cast<uint8_t>(v >> 8));
			this->writeBin(static_cast<uint8_t>(v));
		}
		else {
			this->writeHex(static_cast<uint8_t>(v >> 8));
			this->writeHex(static_cast<uint8_t>(v));
		}
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const uint32_t& v)
	{
		if (this->mode == Mode::Ascii) {
			this->writeInteger(v);
		}
		else if (this->mode == Mode::Binary) {
			this->writeBin(static_cast<uint8_t>(v >> 24));
			this->writeBin(static_cast<uint8_t>(v >> 16));
			this->writeBin(static_cast<uint8_t>(v >> 8));
			this->writeBin(static_cast<uint8_t>(v));
		}
		else {
			this->writeHex(static_cast<uint8_t>(v >> 24));
			this->writeHex(static_cast<uint8_t>(v >> 16));
			this->writeHex(static_cast<uint8_t>(v >> 8));
			this->writeHex(static_cast<uint8_t>(v));
		}
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const int32_t& v)
	{
		if (this->mode == Mode::Ascii) {
			this->writeInteger(v);
		}
		else if (this->mode == Mode::Binary) {
			this->writeBin(static_cast<uint8_t>(v >> 24));
			this->writeBin(static_cast<uint8_t>(v >> 16));
			this->writeBin(static_cast<uint8_t>(v >> 8));
			this->writeBin(static_cast<uint8_t>(v));
		}
		else {
			this->writeHex(static_cast<uint8_t>(v >> 24));
			this->writeHex(static_cast<uint8_t>(v >> 16));
			this->writeHex(static_cast<uint8_t>(v >> 8));
			this->writeHex(static_cast<uint8_t>(v));
		}
		return *this;
	}

#if defined(MODM_OS_OSX) || defined(MODM_CPU_I386)
	// For OSX 'int64_t' is of type 'int'. Therefore there is no
	// function here for the default type 'long int'. As 'long int' has the same
	// width as 'int64_t' we just use a typedef here.
	modm_always_inline IOStream&
	operator << (const long int& v)
	{
		this->writeInteger(static_cast<int64_t>(v));
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const long unsigned int& v)
	{
		this->writeInteger(static_cast<uint64_t>(v));
		return *this;
	}
#endif

#if defined(MODM_CPU_ARM) && defined(MODM_OS_NONE)
	// For ARM 'int32_t' is of type 'long'. Therefore there is no
	// function here for the default type 'int'. As 'int' has the same
	// width as 'int32_t' we just use a typedef here.
	modm_always_inline IOStream&
	operator << (const int& v)
	{
		this->writeInteger(static_cast<int32_t>(v));
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const unsigned int& v)
	{
		this->writeInteger(static_cast<uint32_t>(v));
		return *this;
	}
#endif

// The 64-bit types on the AVR are extremely slow and are
// therefore excluded here
#if not defined(MODM_CPU_AVR)
	modm_always_inline IOStream&
	operator << (const uint64_t& v)
	{
		this->writeInteger(v);
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const int64_t& v)
	{
		this->writeInteger(v);
		return *this;
	}
#endif

	modm_always_inline IOStream&
	operator << (const float& v)
	{
		this->writeFloat(v);
		return *this;
	}

	modm_always_inline IOStream&
	operator << (const double& v)
	{
#if defined(MODM_CPU_AVR)
		this->writeFloat(static_cast<float>(v));
#else
		this->writeDouble(v);
#endif
		return *this;
	}

	IOStream&
	operator << (const char* s)
	{
		if( this->mode == Mode::Ascii ) {
			this->device->write(s);
		}
		else if( this->mode == Mode::Binary ) {
			this->writeBin(s);
		}
		else {
			this->writeHex(s);
		}
		return *this;
	}

	/// write the hex value of a pointer
	IOStream&
	operator << (const void* p);

	modm_always_inline IOStream&
	operator << (IOStream& (*function)(IOStream&))
	{
		return function(*this);
	}

	typedef void (*myfunc)();

	IOStream&
	operator << (const myfunc& value);

	/// Write the hex value of a function pointer, catches all kinds of function pointers.
	template<class Ret, class... Args>
	IOStream&
	operator << (Ret(*pointer)(Args...) )
	{
		unsigned char *p = (unsigned char *)&pointer;

		for (std::size_t i = 0; i < sizeof(myfunc); i++)
		{
			writeHex(p[sizeof(myfunc) - i - 1]);
		}

    	return *this;
	}

	/**
	 * Simple printf() implemenation
	 *
	 * The format string is composed of zero or more directives: ordinary
	 * characters (not %), which are copied unchanged to the output stream;
	 * and conversion specifications, each of which results in fetching zero
	 * or more subsequent arguments. Each conversion specification is
	 * introduced by the % character. The arguments must properly correspond
	 * (after type promotion) with the conversion specifier. After the %,
	 * the following appear in sequence:
	 *
	 * - Zero or more of the following flags:
	 *   - `-` A negative field width flag; the converted value is to be
	 *     left adjusted on the field boundary. The converted value is
	 *     padded on the right with blanks, rather than on the left with
	 *     blanks.
	 *   - `' '` (space) A blank should be left before a positive number
	 *     produced by a signed conversion (d).
	 *   - `+` A sign must always be placed before a number produced by a
	 *     signed conversion. A `+` overrides a space if both are used.
	 *   - An optional decimal digit string specifying a minimum field width.
	 *     If the converted value has fewer characters than the field width,
	 *     it will be padded with spaces on the left (or right, if the
	 *     left-adjustment flag has been given) to fill out the field width.
	 *   - An optional `h`, `l` or `ll` length modifier, that specifies that the argument
	 *     for the `d`, `u`, or `x` conversion is a 8-bit ("h"), 32-bit ("l") or
	 *     64-bit ("ll") rather than 16-bit.
	 *
	 * The conversion specifiers and their meanings are:
	 * - `c`	char (8-bit)
	 * - `s`	string (char *)
	 * - `p`	pointer (void *)
	 * - `d`	signed  decimal
	 * - `u`	unsigned decimal
	 * - `x`	hex
	 * - `f`	float
	 * - `%`	%
	 *
	 * Combined with the length modifiers you get:
	 * - `d`	signed 16-bit
	 * - `ld`	signed 32-bit
	 * - `lld`	signed 64-bit (not yet)
	 * - `u`	unsigned 16-bit
	 * - `lu`	unsigned 32-bit
	 * - `llu`	unsigned 64-bit (not yet)
	 * - `hx`	 8-bit hex
	 * - `x`	16-bit hex
	 * - `lx`	32-bit hex
	 * - `llx`	64-bit hex (not yet)
	 *
	 * Examples, given `-100` as argument in the right type:
	 * \code
	 * %c:
	 * %s:
	 * %p:  0x0100
	 * %d:  -100
	 * %ld: -100
	 * %u:  65436
	 * %lu:
	 * %hx: 0x9c
	 * %x:  0xff9c
	 * %lx: 0xffffff9c
	 * \endcode
	 *
	 * - `s` The "char *" argument is expected to be a pointer to an array
	 *   of character type (pointer to a string). Characters from the array
	 *   are written up to (but not including) a terminating NULL character;
	 *   if a precision is specified, no more than the number specified are
	 *   written. If a precision is given, no null character need be present;
	 *   if the precision is not specified, or is greater than the size of
	 *   the array, the array must contain a terminating NULL character.
	 * - `%` A `%` is written. No argument is converted. The complete conversion
	 *   specification is "%%".
	 *
	 * In no case does a non-existent or small field width cause truncation
	 * of a numeric field; if the result of a conversion is wider than the
	 * field width, the field is expanded to contain the conversion result.
	 *
	 * @param	fmt		Format string
	 */
	IOStream&
	printf(const char* fmt, ...)  __attribute__((format(printf, 2, 3)));

	IOStream&
	vprintf(const char *fmt, va_list vlist) __attribute__((format(printf, 2, 0)));

protected:
	void
	writeInteger(int16_t value);

	void
	writeInteger(uint16_t value);

	void
	writeInteger(int32_t value);

	void
	writeInteger(uint32_t value);

	void
	writeInteger(int64_t value);

	void
	writeInteger(uint64_t value);

	void
	writeHex(const char* s);

	void
	writeBin(const char* s);

	void
	writeHexNibble(uint8_t nibble);

	void
	writeHex(uint8_t value);

	void
	writeBin(uint8_t value);

	void
	writeFloat(const float& value);

#if not defined(MODM_CPU_AVR)
	void
	writeDouble(const double& value);
#endif

        void
        writeUnsignedInteger(unsigned long unsignedValue, uint_fast8_t base, size_t width, char fill, bool isNegative);
#if not defined(MODM_CPU_AVR)
        void
        writeUnsignedLongLong(unsigned long long unsignedValue, uint_fast8_t base, size_t width, char fill, bool isNegative);
#endif


private:
	enum class
	Mode
	{
		Ascii,
		Hexadecimal,
		Binary
	};

	IOStream(const IOStream&);

	IOStream&
	operator =(const IOStream&);

private:
	IODevice* const	device;
	Mode mode;
};

/// Flushes the output stream.
/// This manipulator simply calls the stream's flush() member function.
/// @ingroup io
inline IOStream&
flush(IOStream& ios)
{
	return ios.flush();
}

//// Write a newline and flush the stream.
/// @ingroup io
inline IOStream&
endl(IOStream& ios)
{
	return flush(ios.write('\n'));
}

/// set the output mode to binary style for `char` and `char*`
/// @ingroup io
inline IOStream&
bin(IOStream& ios)
{
	return ios.bin();
}

/// set the output mode to hexadecimal style for `char` and `char*`
/// @ingroup io
inline IOStream&
hex(IOStream& ios)
{
	return ios.hex();
}

/// set the output mode to ASCII style for `char` and `char*`
/// @ingroup io
inline IOStream&
ascii(IOStream& ios)
{
	return ios.ascii();
}

/// @{
/// Set the foreground colour on ANSI terminals.
/// @ingroup io
inline IOStream&
black(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('0');
	ios.write('m');
	return ios;
}

inline IOStream&
red(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('1');
	ios.write('m');
	return ios;
}

inline IOStream&
green(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('2');
	ios.write('m');
	return ios;
}

inline IOStream&
yellow(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('3');
	ios.write('m');
	return ios;
}

inline IOStream&
blue(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('4');
	ios.write('m');
	return ios;
}

inline IOStream&
magenta(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('1');
	ios.write('m');
	return ios;
}

inline IOStream&
cyan(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('6');
	ios.write('m');
	return ios;
}

inline IOStream&
white(IOStream& ios)
{
	ios.write('\033');
	ios.write('[');
	ios.write('3');
	ios.write('7');
	ios.write('m');
	return ios;
}
/// @}

}	// namespace modm

#endif // MODM_IOSTREAM_HPP
