// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA8574_HPP
#define XPCC_PCA8574_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>

namespace xpcc
{

/**
 * PCA8574: Remote 8-bit I/O expander for I2C-bus with interrupt
 *
 * @author  strongly-typed
 */
template < class I2cMaster >
class Pca8574 : public xpcc::I2cDevice< I2cMaster, 2 >
{
public:
	Pca8574(uint8_t address=0x27);

public:
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	write(uint8_t value);

	xpcc::ResumableResult<bool> ALWAYS_INLINE
	read(uint8_t &value);

private:
	// Internal write buffer
	uint8_t i2cBuffer[1];
};

} // namespace xpcc

#include "pca8574_impl.hpp"

#endif // XPCC_PCA8574_HPP
