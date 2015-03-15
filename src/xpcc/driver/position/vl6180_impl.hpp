// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_VL6180_HPP
#	error "Don't include this file directly, use 'vl6180.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Vl6180<I2cMaster>::Vl6180(Data &data, uint8_t address)
:	data(data), i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess),
	configuration(vl6180_private::configuration),
	rawBuffer{0,0,0, 0,0,0}
{
}

// MARK: - i2cTasks
// MARK: ping
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Vl6180<I2cMaster>::ping()
{
	CO_BEGIN();

	CO_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Vl6180<I2cMaster>::initialize()
{
	CO_BEGIN();

	// rawBuffer[3] => success of config
	// rawBuffer[4] => index of array

	rawBuffer[3] = true;
	for (rawBuffer[4] = 0; rawBuffer[4] < 39; rawBuffer[4]++)
	{
		rawBuffer[2] = configuration[rawBuffer[4]].value;
		rawBuffer[3] &= CO_CALL(write(Register(configuration[rawBuffer[4]].reg), rawBuffer[2]));
	}

	CO_END_RETURN(rawBuffer[3]);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Vl6180<I2cMaster>::readDistance()
{
	// for this relatively complicated sequence, see the datasheet page 17
	CO_BEGIN();

	// Write 0x01 to the SYSRANGE__START register{0x18}.
	// The start bit auto-clears after completion.
	rawBuffer[2] = uint8_t(Start::StartStop);
	if ( CO_CALL(write(Register::SYSRANGE__START, rawBuffer[2])) )
	{
		// Measurement will take 7.5ms + convergence time (< ~10ms)
		timeout.restart(8);
		CO_WAIT_UNTIL(timeout.isExpired());

		// When the measurement is completed, bit 2 of RESULT__INTERRUPT_STATUS_GPIO{0x4F} will be set.
		while(true)
		{
			// read the byte
			if ( !CO_CALL(read(Register::RESULT__INTERRUPT_STATUS_GPIO, rawBuffer[2])) )
				CO_RETURN(false);

			// break if the correct interrupt source is set
			if ( RangeInterruptStatus_t::get(InterruptStatus_t(rawBuffer[2])) == InterruptSource::NewSampleReady )
				break;

			// otherwise wait 1ms and try again
			timeout.restart(1);
			CO_WAIT_UNTIL(timeout.isExpired());
		}

		// The range result is read from RESULT__RANGE_VAL{0x62}.
		if ( CO_CALL(read(Register::RESULT__RANGE_VAL, rawBuffer[2])) )
		{
			data.data = rawBuffer[2];

			// Interrupt status flags are cleared by writing a ‘1’ to the appropriate bit of SYSTEM__INTERRUPT_CLEAR{0x15}.
			rawBuffer[2] = uint8_t(InterruptClear::Range);
			if ( CO_CALL(write(Register::SYSTEM__INTERRUPT_CLEAR, rawBuffer[2])) )
			{
				// Bit 0 of RESULT__RANGE_STATUS{0x4D} indicate when either sensor is ready for the next operation.
				while(true)
				{
					// read the byte
					if ( !CO_CALL(read(Register::RESULT__RANGE_STATUS, rawBuffer[2])) )
						CO_RETURN(false);

					// Error codes are indicated in bits [7:4] of the status registers
					if ( RangeStatus_t(rawBuffer[2]) & RangeStatus::DeviceReady )
					{
						rangeError = RangeErrorCode_t::get(RangeStatus_t(rawBuffer[2]));
						break;
					}

					// otherwise wait 1ms and try again
					timeout.restart(1);
					CO_WAIT_UNTIL(timeout.isExpired());
				}

				// the sequence was executed successfully
				CO_RETURN(true);
			}
		}
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: update register
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Vl6180<I2cMaster>::updateControlRegister(Register reg, Control_t setMask, Control_t clearMask)
{
	CO_BEGIN();

	if (clearMask.value != 0xff)
		if (!CO_CALL(read(reg, rawBuffer[2])))
			CO_RETURN(false);

	rawBuffer[2] = (rawBuffer[2] & ~clearMask.value) | setMask.value;

	CO_END_RETURN_CALL(write(reg, rawBuffer[2]));
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Vl6180<I2cMaster>::write(Register reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN();

	if (length > 4)
		CO_RETURN(false);

	rawBuffer[0] = uint16_t(reg) >> 8;
	rawBuffer[1] = uint8_t(reg);
	std::memcpy(rawBuffer+2, buffer, length);

	CO_WAIT_UNTIL(
			adapter.configureWrite(rawBuffer, length+2) and
					(i2cTask = I2cTask::WriteRegister, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::WriteRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::WriteRegister);
}

// MARK: read multilength register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Vl6180<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN();

	rawBuffer[0] = uint16_t(reg) >> 8;
	rawBuffer[1] = uint8_t(reg);

	CO_WAIT_UNTIL(
			adapter.configureWriteRead(rawBuffer, 2, buffer, length) and
					(i2cTask = I2cTask::ReadRegister, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::ReadRegister);
}
