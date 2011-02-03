
#include "nds3.hpp"

enum Command
{
	CMD_RESET = 0x00,
	CMD_MOVE = 0x01,
	CMD_MEASURE = 0x02,
	CMD_READ = 0x03,
	CMD_GETPOS = 0x04,
	CMD_GETDIST = 0x05,
	CMD_GETBUSY = 0x06,
	CMD_SETDDR = 0x10,
	CMD_SETPORT = 0x11,
	CMD_GETPIN = 0x12,
	CMD_GETANALOG = 0x13,
	CMD_MOVESERVO = 0x20,
	CMD_GETSERVOCALIB = 0x21,
	CMD_SETSERVOCALIB = 0x22,
	CMD_GETDISTCALIB = 0x23,
	CMD_SETDISTCALIB = 0x24
};

uint8_t Nds3::buffer[16];

// ----------------------------------------------------------------------------
bool
Nds3::isInterfaceReady()
{
	return !Nibo2::twi.isBusy();
}

// ----------------------------------------------------------------------------
void
Nds3::reset()
{
	buffer[0] = CMD_RESET;
	
	Nibo2::twi.write(nds3Address, buffer, 1);
}

// ----------------------------------------------------------------------------
void
Nds3::readBusyStatus()
{
	buffer[0] = CMD_GETBUSY;
	
	Nibo2::twi.writeRead(nds3Address, buffer, 1, 1);
}

bool
Nds3::isBusy()
{
	return (buffer[0] != 0);
}

// ----------------------------------------------------------------------------
void
Nds3::move(uint8_t position)
{
	buffer[0] = CMD_MOVE;
	buffer[1] = position;
	
	Nibo2::twi.write(nds3Address, buffer, 2);
}

// ----------------------------------------------------------------------------
void
Nds3::readPosition()
{
	buffer[0] = CMD_GETPOS;
	
	Nibo2::twi.writeRead(nds3Address, buffer, 1, 1);	
}

uint8_t
Nds3::getPosition()
{
	return buffer[0];
}

// ----------------------------------------------------------------------------
void
Nds3::readDistance()
{
	buffer[0] = CMD_GETDIST;
	
	Nibo2::twi.writeRead(nds3Address, buffer, 1, 1);
}

uint8_t
Nds3::getDistance()
{
	return buffer[0];
}

// ----------------------------------------------------------------------------
void
Nds3::scan(uint8_t endPosition, int8_t stepWidth)
{
	buffer[0] = CMD_MEASURE;
	buffer[1] = endPosition;
	buffer[2] = stepWidth;
	
	Nibo2::twi.write(nds3Address, buffer, 3);
}

void
Nds3::readScanResult(uint8_t position, uint8_t step)
{
	buffer[0] = CMD_READ;
	buffer[1] = position;
	buffer[2] = step;
	
	Nibo2::twi.writeRead(nds3Address, buffer, 3, 16);
}

void
Nds3::getScanResult()
{
	
}
