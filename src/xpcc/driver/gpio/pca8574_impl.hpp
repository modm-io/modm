// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA8574_HPP
#	error "Don't include this file directly, use 'pca8574.hpp' instead!"
#endif

template < class I2cMaster >
xpcc::Pca8574<I2cMaster>::Pca8574(/* Data &data, */ uint8_t address)
: I2cDevice<I2cMaster, 2>(address) /* , data(data) */
{
}

template < class I2cMaster >
xpcc::ResumableResult<bool> ALWAYS_INLINE
xpcc::Pca8574<I2cMaster>::write(uint8_t value)
{
	RF_BEGIN();

	i2cBuffer[0] = value;
	this->transaction.configureWrite(i2cBuffer, /* length */ 1);

	RF_END_RETURN_CALL( this->runTransaction() )
};

template < class I2cMaster >
xpcc::ResumableResult<bool> ALWAYS_INLINE
xpcc::Pca8574<I2cMaster>::read(uint8_t &value)
{
	RF_BEGIN();

	this->transaction.configureRead(i2cBuffer, 1);
	value = i2cBuffer[0];

	RF_END_RETURN_CALL( this->runTransaction() )
};
