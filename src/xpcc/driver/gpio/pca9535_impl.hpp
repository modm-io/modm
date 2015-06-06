// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA9535_HPP
#	error "Don't include this file directly, use 'pca9535.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Pca9535<I2cMaster>::Pca9535(uint8_t address)
:	I2cDevice<I2cMaster,2>(address), memory()
{
}

// MARK: - Tasks
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::setOutput(Pins pins)
{
	RF_BEGIN();

	// output is 0, input is 1
	memory.configuration.reset(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Configuration) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::set(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.set(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::reset(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.reset(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::toggle(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.toggle(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::update(Pins pins, bool value)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.update(pins, value);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::setInput(Pins pins)
{
	RF_BEGIN();

	// output is 0, input is 1
	memory.configuration.set(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Configuration) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::setInvertInput(Pins pins)
{
	RF_BEGIN();

	// inverted is 1, normal is 0
	memory.polarity.set(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Polarity) );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::resetInvertInput(Pins pins)
{
	RF_BEGIN();

	// inverted is 1, normal is 0
	memory.polarity.reset(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Polarity) );
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::writeMemory(Index index)
{
	RF_BEGIN();

	this->transaction.configureWrite(buffer + uint8_t(index), 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: read multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca9535<I2cMaster>::readMemory(Index index)
{
	RF_BEGIN();

	this->transaction.configureWriteRead(
			buffer + uint8_t(index)    , 1,
			buffer + uint8_t(index) + 1, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}
