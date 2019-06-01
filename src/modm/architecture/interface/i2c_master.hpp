/*
 * Copyright (c) 2014-2015, 2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2C_MASTER_HPP
#define MODM_INTERFACE_I2C_MASTER_HPP

#include "i2c.hpp"
#include "i2c_transaction.hpp"
#include "peripheral.hpp"

namespace modm
{

/**
 * Interface of a I2C master.
 *
 * Performing transfers in background allows the use of DMA etc. and
 * enables the program to do other things while the operation is
 * going on.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup modm_architecture_i2c
 */
class I2cMaster : public ::modm::PeripheralDriver, public ::modm::I2c
{
public:
	/// Errors that can happen during master operation
	enum class
	Error : uint8_t
	{
		NoError,			///< No Error occurred
		SoftwareReset,		///< The master was reset in software
		AddressNack,		///< Address was transmitted and NACK received
		DataNack,			///< Data was transmitted and NACK received
		ArbitrationLost,	///< Arbitration was lost during writing or reading
		BusCondition,		///< Misplaced Start or Stop condition
		BusBusy,			///< Bus is busy during Start condition
		Unknown				///< Unknown error condition
	};

	enum class
	PullUps
	{
		External,	///< External pull-ups exists in SDA and SCL
		Internal	///< Use weak internal pull-ups on SDA and SCL
	};

	enum class
	ResetDevices : uint32_t
	{
		NoReset  =        0,	///< Do not reset devices
		LowSpeed =   10'000,	///< Low-Speed datarate of 10kHz
		Standard =  100'000,	///< Standard datarate of 100kHz
		Fast     =  400'000,	///< Fast datarate of 400kHz
	};

#ifdef __DOXYGEN__
public:
	/// Number of transactions which can be queued in this driver.
	static constexpr size_t TransactionBufferSize = 8;

public:
	/**
	 * Configures the Scl and Sda signals and connects them.
	 *
	 * This configures the Scl and Sda signals as open-drain outputs with optional
	 * weak internal pullups and optionally resets all slave devices on the bus.
	 *
	 * @warning Using weak internal pullups is not recommended and will require
	 *			a slow baudrate. You must use external pullups for a reliable bus.
	 *
	 * @tparam	Signals
	 *		One Scl and one Sda signal are required and can be passed out-of-order.
	 * @tparam	reset
	 * 		Choose a speed to reset the I2C devices with. Can be disabled with `ResetDevices::NoReset`.
	 * @param	pullups
	 * 		Use external or weak internal pullups.
	 */
	template< class... Signals, ResetDevices reset = ResetDevices::Standard >
	static void
	connect(PullUps pullups = PullUps::External);

	/**
	 * Initializes the hardware and sets the datarate.
	 *
	 * @tparam	SystemClock
	 * 		the currently active system clock
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting baudrate
	 */
	template< class SystemClock, baudrate_t baudrate=100_kBd, percent_t tolerance=5_pct >
	static void
	initialize();

	/**
	 * Requests transaction attachment.
	 *
	 * @param	transaction
	 *		object that inherits from the I2cTransaction class.
	 * @param	configuration
	 *		function which configures the master for this transaction.
	 * @return	Caller gains control if `true`. Call has no effect if `false`.
	 */
	static bool
	start(I2cTransaction *transaction, ConfigurationHandler handler = nullptr);

	/**
	 * Perform a software reset of the driver in case of an error and detach
	 * the transaction object.
	 *
	 * It is strongly recommended to reset the slave devices on the bus
	 * after a master reset. You can use the connect() method for that.
	 */
	static void
	reset();

	/**
	 * Check the error state of the driver.
	 *
	 * The error states are hardware and implementation specific.
	 */
	static Error
	getErrorState();
#endif
};

}	// namespace modm

#endif // MODM_INTERFACE_I2C_MASTER_HPP
