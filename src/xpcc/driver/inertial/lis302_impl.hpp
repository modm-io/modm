// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LIS302DL_HPP
#	error  "Don't include this file directly, use 'lis302.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
// MARK: LIS302 DRIVER
template < class Transport >
xpcc::Lis302<Transport>::Lis302(Data &data, uint8_t address)
:	Transport(address), data(data), rawBuffer{0x47,0,0,0,0,0,0,0,0,0}
{
}


template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302<Transport>::initialize(void *ctx, Scale scale, MeasurementRate rate)
{
	return writeControlRegister(ctx, 0, i(scale) | i(rate), Control1::DR | Control1::FS);
}

template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302<Transport>::writeControlRegister(void *ctx, uint8_t index, uint8_t setMask, uint8_t clearMask)
{
	CO_BEGIN(ctx);

	if (index < 2)
	{
		rawBuffer[index] = (rawBuffer[index] & ~clearMask) | setMask;
		if (index == 0)
			data.getPointer()[3] = (rawBuffer[0] & Control1::FS);

		CO_RETURN_CALL(this->write(ctx, i(Register::CtrlReg1) + index, rawBuffer[index]));
	}

	CO_END_RETURN(false);
}

template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302<Transport>::readAcceleration(void *ctx)
{
	CO_BEGIN(ctx);

	if (CO_CALL(this->read(ctx, i(Register::Status), rawBuffer + 3, 7)))
	{
		data.getPointer()[0] = rawBuffer[5];
		data.getPointer()[1] = rawBuffer[7];
		data.getPointer()[2] = rawBuffer[9];
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: I2C TRANSPORT
template < class I2cMaster >
xpcc::Lis302I2cTransport<I2cMaster>::Lis302I2cTransport(uint8_t address)
:	i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess)
{
}

// MARK: ping
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Lis302I2cTransport<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

// MARK: - register access
// MARK: write register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Lis302I2cTransport<I2cMaster>::write(void *ctx, uint8_t reg, uint8_t value)
{
	CO_BEGIN(ctx);

	buffer[0] = reg | AddressStatic;
	buffer[1] = value;

	CO_WAIT_UNTIL(
			adapter.configureWrite(buffer, 2) &&
			(i2cTask = reg, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == reg);

	CO_END_RETURN(i2cSuccess == reg);
}

// MARK: read register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Lis302I2cTransport<I2cMaster>::read(void *ctx, uint8_t reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN(ctx);

	this->buffer[0] = reg | AddressIncrement;

	CO_WAIT_UNTIL(
			adapter.configureWriteRead(this->buffer, 1, buffer, length) &&
			(i2cTask = reg, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == reg);

	CO_END_RETURN(i2cSuccess == reg);
}

// ----------------------------------------------------------------------------
// MARK: SPI TRANSPORT
template < class SpiMaster, class Cs >
xpcc::Lis302SpiTransport<SpiMaster, Cs>::Lis302SpiTransport(uint8_t /*address*/)
:	lengthBuffer(0), whoAmI(0)
{
	Cs::setOutput(xpcc::Gpio::High);
}

// MARK: ping
template < class SpiMaster, class Cs >
xpcc::co::Result<bool>
xpcc::Lis302SpiTransport<SpiMaster, Cs>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_CALL(read(ctx, static_cast<uint8_t>(lis302::Register::WhoAmI), whoAmI));

	CO_END_RETURN(whoAmI == 0x3B);
}

// MARK: - register access
// MARK: write register
template < class SpiMaster, class Cs >
xpcc::co::Result<bool>
xpcc::Lis302SpiTransport<SpiMaster, Cs>::write(void *ctx, uint8_t reg, uint8_t value)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(this->aquireMaster(ctx));
	Cs::reset();

	CO_CALL(SpiMaster::transfer(reg | Write | AddressStatic));
	CO_CALL(SpiMaster::transfer(value));

	if (this->releaseMaster(ctx))
		Cs::set();

	CO_END_RETURN(true);
}

// MARK: read register
template < class SpiMaster, class Cs >
xpcc::co::Result<bool>
xpcc::Lis302SpiTransport<SpiMaster, Cs>::read(void *ctx, uint8_t reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(this->aquireMaster(ctx));
	Cs::reset();

	CO_CALL(SpiMaster::transfer(reg | Read | AddressIncrement));

	for (lengthBuffer = 0; lengthBuffer < length; lengthBuffer++)
	{
		whoAmI = CO_CALL(SpiMaster::transfer(0));
		buffer[lengthBuffer] = whoAmI;
	}

	if (this->releaseMaster(ctx))
		Cs::set();

	CO_END_RETURN(true);
}

