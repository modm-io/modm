// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TCS3414_HPP
#	error	"Don't include this file directly, use 'tcs3414.hpp' instead!"
#endif

template<typename I2cMaster>
typename xpcc::Tcs3414<I2cMaster>::Data xpcc::Tcs3414<I2cMaster>::data;

template<typename I2cMaster>
typename xpcc::tcs3414::Rgb xpcc::Tcs3414<I2cMaster>::color;

template < typename I2cMaster >
xpcc::Tcs3414<I2cMaster>::Tcs3414(uint8_t address)
: I2cDevice<I2cMaster>(),
  i2cTask(I2cTask::Idle),
  i2cSuccess(0),
  adapter(address, i2cTask, i2cSuccess)
{
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Tcs3414<I2cMaster>::initialize(void *ctx)
{
	return writeRegister(ctx, RegisterAddress::CONTROL, 0b11);	// control to power up and start conversion
};

// ----------------------------------------------------------------------------
template<typename I2cMaster>
xpcc::co::Result<bool>
xpcc::Tcs3414<I2cMaster>::configure(
		void *ctx,
		const Gain gain,
		const Prescaler prescaler,
		const IntegrationMode mode,
		const uint8_t time)
{
	CO_BEGIN(ctx);

	CO_CALL(setGain(ctx, gain, prescaler));

	CO_CALL(setIntegrationTime(ctx, mode, time));

	CO_RETURN(true);

	CO_END();
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Tcs3414<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() && this->startTransaction(&adapter));

	i2cTask = I2cTask::Ping;

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	if (i2cSuccess == I2cTask::Ping)
		CO_RETURN(true);

	CO_END();
}

template<typename I2cMaster>
xpcc::co::Result<bool>
xpcc::Tcs3414<I2cMaster>::writeRegister(
		void *ctx,
		const RegisterAddress address,
		const uint8_t value)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] =
							0x80							// write command
						|	0x40							// with SMB read/write protocol
						|	static_cast<uint8_t>(address),	// at this address
					commandBuffer[2] = value,
					adapter.configureWrite(commandBuffer, 3) &&
					this->startTransaction(&adapter)
			)
	);

	i2cTask = I2cTask::WriteRegister;

	CO_WAIT_WHILE(i2cTask == I2cTask::WriteRegister);

	if (i2cSuccess == I2cTask::WriteRegister) {
		CO_RETURN(true);
	}

	CO_END();
}

template<typename I2cMaster>
xpcc::co::Result<bool>
xpcc::Tcs3414<I2cMaster>::readRegisters(
		void* ctx,
		const RegisterAddress address,
		uint8_t* const values,
		const uint8_t count)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] =
							  static_cast<uint8_t>(0x80)		// write command
							| static_cast<uint8_t>(0x40)		// with SMB read/write protocol
							| static_cast<uint8_t>(address),	// at this address
					adapter.configureWriteRead(commandBuffer, 1, values, count) &&
					this->startTransaction(&adapter)
			)
	);

	i2cTask = I2cTask::ReadRegister;

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadRegister);

	if (i2cSuccess == I2cTask::ReadRegister) {
		CO_RETURN(true);
	}

	CO_END();
}
