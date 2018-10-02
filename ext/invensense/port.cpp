/*
 * Copyright (c) 2018 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/clock.hpp>
#include "../eMD.hpp"

class EMPL_Transaction : public modm::I2cWriteReadTransaction
{
	uint8_t addressBuffer[1];
	bool writeAddress;
public:
	inline EMPL_Transaction() :
		modm::I2cWriteReadTransaction(0b1101000), writeAddress(false)
	{}

	inline bool
	configureWrite(uint8_t address, const uint8_t *buffer, std::size_t size)
	{
		if (I2cWriteReadTransaction::configureWrite(buffer, size))
		{
			addressBuffer[0] = address;
			writeAddress = true;
			return true;
		}
		return false;
	}

	inline bool
	configureRead(uint16_t address, uint8_t *buffer, std::size_t size)
	{
		addressBuffer[0] = address;
		writeAddress = false;
		return I2cWriteReadTransaction::configureWriteRead(addressBuffer, 1, buffer, size);
	}

protected:
	virtual modm::I2cTransaction::Writing
	writing() override
	{
		if (writeAddress)
		{
			writeAddress = false;
			return Writing(addressBuffer, 1, OperationAfterWrite::Write);
		}
		return I2cWriteReadTransaction::writing();
	}
} transaction;

modm::emd::start_t i2c_start = nullptr;

bool modm::emd::set_i2c_start_function(start_t start)
{
	if (i2c_start == nullptr) {
		i2c_start = start;
		return true;
	}
	return false;
}

extern "C"
{

unsigned char *mpl_key = (unsigned char *)"eMPL 5.1";

int inv_i2c_write(uint8_t /*slave_addr*/,
				   uint8_t reg_addr,
				   uint8_t length,
				   uint8_t const *data)
{
	if (i2c_start) {
		transaction.configureWrite(reg_addr, data, length);
		i2c_start(&transaction);
		while(transaction.isBusy()) ;
		return (transaction.getState() == transaction.TransactionState::Error) ? 1 : 0;
	}
	return -1;
}

int inv_i2c_read(uint8_t /*slave_addr*/,
				  uint8_t reg_addr,
				  uint8_t length,
				  uint8_t *data)
{
	if (i2c_start) {
		transaction.configureRead(reg_addr, data, length);
		i2c_start(&transaction);
		while(transaction.isBusy()) ;
		return (transaction.getState() == transaction.TransactionState::Error) ? 1 : 0;
	}
	return -1;
}

void inv_delay_ms(uint32_t num_ms)
{
	modm::delayMilliseconds(num_ms);
}

void inv_get_ms(uint32_t *count)
{
	*count = modm::Clock::now().getTime();
}

}