// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HD44780_BASE_HPP
	#error	"Don't include this file directly, use 'hd44780_base.hpp' instead!"
#endif

#include <xpcc/architecture/driver/delay.hpp>

// ----------------------------------------------------------------------------
template <typename DATA, typename RW, typename RS, typename E>
void
xpcc::Hd44780Base<DATA, RW, RS, E>::initialize(LineMode lineMode)
{
	E::setOutput(E_Disable);
	RW::setOutput(RW_Write);
	RS::setOutput(RS_Command);

	Bus<DATA, E, DATA::width>::writeHighNibble(Set8BitBus);

	xpcc::delayMilliseconds(5);

	Bus<DATA, E, DATA::width>::writeHighNibble(Set8BitBus);

	xpcc::delayMicroseconds(100);

	Bus<DATA, E, DATA::width>::writeHighNibble(Set8BitBus);

	if (DATA::width == 4) {
		while(isBusy())
			;
		Bus<DATA, E, DATA::width>::writeHighNibble(Set4BitBus);
	}

	while(!writeCommand(static_cast<InternalCommand>(lineMode) |
			Bus<DATA, E, DATA::width>::Mode))
		;
	while(!writeCommand(DisableDisplay))
		;
	while(!writeCommand(ClearDisplay))
		;
	while(!writeCommand(IncrementCursor))
		;
	while(!writeCommand(EnableDisplay | DisableCursor | DisableCursorBlink))
		;
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::clear()
{
	return writeCommand(ClearDisplay);
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::resetCursor()
{
	return writeCommand(ResetCursor);
}

// MARK: write
template <typename DATA, typename RW, typename RS, typename E>
void
xpcc::Hd44780Base<DATA, RW, RS, E>::write(uint8_t data)
{
	RW::set(RW_Write);
	Bus<DATA, E, DATA::width>::write(data);
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::writeAddress(uint8_t address)
{
	if (isBusy())
		return false;

	RS::set(RS_Command);
	RW::set(RW_Write);

	write((SetDDRAM_Address | (address & DDRAM_AddressMask)));

	return true;
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::writeCommand(uint8_t command)
{
	if (isBusy())
		return false;

	RS::set(RS_Command);
	write(command);

	return true;
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::writeRAM(uint8_t data)
{
	if (isBusy())
		return false;

	RS::set(RS_RAM);
	write(data);

	return true;
}

// MARK: read

template <typename DATA, typename RW, typename RS, typename E>
uint8_t
xpcc::Hd44780Base<DATA, RW, RS, E>::read()
{
	RW::set(RW_Read);
	return Bus<DATA, E, DATA::width>::read();
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::readAddress(uint8_t &address)
{
	RS::set(RS_Command);
	address = (read() & CGRAM_AddressMask);

	return true;
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::readRAM(uint8_t &data)
{
	if (isBusy())
		return false;

	RS::set(RS_RAM);
	data = read();

	return true;
}

// MARK: status
template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::isBusy()
{
	RS::set(RS_Command);

	if (read() & BusyFlagMask)
	{
		xpcc::delayMicroseconds(2);
		return true;
	}
	return false;
}

template <typename DATA, typename RW, typename RS, typename E>
bool
xpcc::Hd44780Base<DATA, RW, RS, E>::writeCGRAM(uint8_t character, uint8_t *cg)
{
	while(not writeCommand(SetCGRAM_Address | (character << 3)))
		;
	for (std::size_t ii = 0; ii < 8; ++ii) {
		writeRAM(cg[ii]);
	}
	return true;
}


// MARK: bus specialisation of 4bit port
template <typename DATA, typename RW, typename RS, typename E>
template <typename Data, typename Enable>
void
xpcc::Hd44780Base<DATA, RW, RS, E>::Bus<Data, Enable, 4>::write(uint8_t data)
{
	DATA::setOutput();
	DATA::write(data >> 4);

	E::set();
	xpcc::delayMicroseconds(1);
	E::reset();

	DATA::write(data);

	E::set();
	xpcc::delayMicroseconds(1);
	E::reset();
}

template <typename DATA, typename RW, typename RS, typename E>
template <typename Data, typename Enable>
void
xpcc::Hd44780Base<DATA, RW, RS, E>::Bus<Data, Enable, 4>::writeHighNibble(uint8_t data)
{
	Bus<DATA, E, 8>::write(data);
}

template <typename DATA, typename RW, typename RS, typename E>
template <typename Data, typename Enable>
uint8_t
xpcc::Hd44780Base<DATA, RW, RS, E>::Bus<Data, Enable, 4>::read()
{
	uint8_t data;
	DATA::setInput();

	E::set();
	xpcc::delayMicroseconds(1);
	data = DATA::read();
	E::reset();

	data <<= 4;

	E::set();
	xpcc::delayMicroseconds(1);
	data |= DATA::read();
	E::reset();

	return data;
}

// MARK: bus specialisation of 8bit port
template <typename DATA, typename RW, typename RS, typename E>
template <typename Data, typename Enable>
void
xpcc::Hd44780Base<DATA, RW, RS, E>::Bus<Data, Enable, 8>::write(uint8_t data)
{
	DATA::setOutput();
	DATA::write(data);

	E::set();
	xpcc::delayMicroseconds(1);
	E::reset();
}

template <typename DATA, typename RW, typename RS, typename E>
template <typename Data, typename Enable>
void
xpcc::Hd44780Base<DATA, RW, RS, E>::Bus<Data, Enable, 8>::writeHighNibble(uint8_t data)
{
	write(data);
}

template <typename DATA, typename RW, typename RS, typename E>
template <typename Data, typename Enable>
uint8_t
xpcc::Hd44780Base<DATA, RW, RS, E>::Bus<Data, Enable, 8>::read()
{
	uint8_t data;
	DATA::setInput();

	E::set();
	xpcc::delayMicroseconds(1);
	data = DATA::read();
	E::reset();

	return data;
}



