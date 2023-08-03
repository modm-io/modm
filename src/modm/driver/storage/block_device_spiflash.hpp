// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_SPIFLASH_HPP
#define MODM_BLOCK_DEVICE_SPIFLASH_HPP

#include <modm/architecture/interface/block_device.hpp>

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{


/**
 * \brief	Block device with SPI Flash
 *
 * \tparam Spi			The SpiMaster interface
 * \tparam Cs			The GpioOutput pin connected to the flash chip select
 * \tparam flashSize	Flash chip size in byte
 *
 * The `read()`, `erase()`,`program()` and `write()` methodes wait for
 * the chip to finish writing to the flash.
 *
 * \ingroup	modm_driver_block_device_spi_flash
 * \author	Raphael Lehmann
 * \author  Rasmus Kleist Hørlyck Sørensen
 */
template <typename Spi, typename Cs, uint32_t flashSize>
class BdSpiFlash : public modm::BlockDevice, public modm::SpiDevice< Spi >, protected NestedResumable<6>
{
public:
	/// Initializes the storage hardware
	modm::ResumableResult<bool>
	initialize();

	/// Deinitializes the storage hardware
	modm::ResumableResult<bool>
	deinitialize();

	/** Read data from one or more blocks
	 *
	 *  @param buffer	Buffer to read data into
	 *  @param address	Address to begin reading from
	 *  @param size		Size to read in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	modm::ResumableResult<bool>
	read(uint8_t* buffer, bd_address_t address, bd_size_t size);

	/** Program blocks with data
	 *
	 *  Any block has to be erased prior to being programmed
	 *
	 *  @param buffer	Buffer of data to write to blocks
	 *  @param address	Address of first block to begin writing to
	 *  @param size		Size to write in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	modm::ResumableResult<bool>
	program(const uint8_t* buffer, bd_address_t address, bd_size_t size);

	/** Erase blocks
	 *
	 *  The state of an erased block is undefined until it has been programmed
	 *
	 *  @param address	Address of block to begin erasing
	 *  @param size		Size to erase in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	modm::ResumableResult<bool>
	erase(bd_address_t address, bd_size_t size);

	/** Writes data to one or more blocks after erasing them
	*
	*  The blocks are erased prior to being programmed
	*
	*  @param buffer	Buffer of data to write to blocks
	*  @param address	Address of first block to begin writing to
	*  @param size		Size to write in bytes (multiple of read block size)
	*  @return			True on success
	*/
	modm::ResumableResult<bool>
	write(const uint8_t* buffer, bd_address_t address, bd_size_t size);

public:
	struct
	JedecId
	{
		uint8_t manufacturerId;
		uint8_t deviceType;
		uint8_t deviceId;

		JedecId() {
			manufacturerId = 0;
			deviceType = 0;
			deviceId = 0;
		}

		JedecId(uint8_t mi, uint8_t dt, uint8_t di) {
			manufacturerId = mi;
			deviceType = dt;
			deviceId = di;
		}
	};

	/** Reads the Jedec ID from the flash chip
	*
	*  @return The Jedec Id returned from the flash chip
	*/
	modm::ResumableResult<JedecId>
	readId();

public:
	enum class
	StatusRegister : uint8_t
	{
		Busy					= Bit0,
		WriteEnabled			= Bit1,
		EraseSuspended			= Bit2,
		ProgramSuspended		= Bit3,
		WriteProtectionLockDown	= Bit4,
		SecurityIdLocked		= Bit5,
		//Reserved				= Bit6,
		//Busy					= Bit7, // see Bit0
	};
	MODM_FLAGS8(StatusRegister);

	/** Read status register of thew Spi Flash chip
	*
	*  @return	Status register
	*/
	modm::ResumableResult<StatusRegister>
	readStatus();

public:

	/** Software Die Select
	*
	*  @param die 	The pre-assigned “Die ID#” of the die to select
	*/
	modm::ResumableResult<void>
	selectDie(uint8_t die);

public:
	static constexpr bd_size_t BlockSizeRead = 1;
	static constexpr bd_size_t BlockSizeWrite = 256;
	static constexpr bd_size_t BlockSizeErase = 4 * 1'024;
	static constexpr bd_size_t DeviceSize = flashSize;
	static constexpr bd_size_t ExtendedAddressThreshold = 16 * 1'024 * 1'024;

private:
	uint8_t instructionBuffer[7];
	uint8_t i;

	uint8_t resultBuffer[3];
	uint32_t result;

	size_t index;

	enum class
	Instruction : uint8_t
	{
		SDS		= 0xC2, ///< Software Die Select
		WE 		= 0x06,	///< Write Enable
		VSRWE 	= 0x50, ///< Volatile Status Register Write Enable
		WD 		= 0x04, ///< Write Disable
		RDI		= 0xAB, ///< Read Device ID
		RMDI	= 0x90, ///< Read Manufactuerer/Device ID
		RJI		= 0x9F, ///< Read JEDEC ID
		RUI		= 0x4B, ///< Read Unique ID
		RD		= 0x03, ///< Read Data
		RD4BA	= 0x13, ///< Read Data with 4-Byte Address
		FR		= 0x0B, ///< Fast Read
		FR4BA 	= 0x0C, ///< Fast Read with 4-Byte Address
		PP		= 0x02, ///< Page Program
		PP4BA	= 0x12, ///< Page Program with 4-Byte Address
		SE		= 0x20, ///< 4KB Sector Erase
		SE4B	= 0x21, ///< 4KB Sector Erase with 4-Byte Address
		BE32	= 0x52, ///< 32KB Block Erase
		BE		= 0xD8, ///< 64KB Block Erase
		BE4B	= 0xDC, ///< 64KB Block Erase with 4-Byte Address
		CE		= 0xC7, ///< Chip Erase
		RSR1 	= 0x05, ///< Read Status Register-1
		WSR1 	= 0x01, ///< Write Status Register-1
		RSR2 	= 0x35, ///< Read Status Register-2
		WSR2 	= 0x31, ///< Write Status Register-2
		RSR3 	= 0x15, ///< Read Status Register-3
		WSR3 	= 0x11, ///< Write Status Register-3
		RSFDPR	= 0x5A, ///< Read SFDP Register
		ESR		= 0x44,	///< Erase Security Register
		PSR 	= 0x42, ///< Program Security Register
		RSR		= 0x48, ///< Read Security Register
		GBL     = 0x7E, ///< Global Block Lock
		GBU     = 0x98, ///< Global Block Unlock
		RBL     = 0x3D, ///< Read Block Lock
		IBL		= 0x36,	///< Individual Block Lock
		IBU 	= 0x39, ///< Individual Block Unlock
		EPS 	= 0x75, ///< Erase / Program Suspend
		EPR		= 0x7A, ///< Erase / Program Resume
		En4BAM  = 0xB7, ///< Enter 4-Byte Address Mode
		Ex4BAM  = 0xE9, ///< Exit 4-Byte Address Mode
		RstEn	= 0x66, ///< Enable Reset
		Rst		= 0x99, ///< Reset Device
	};

public:
	/** Check if device is busy
	 *
	 * @return True if device is busy.
	 */
	modm::ResumableResult<bool>
	isBusy();

	/** This function can be used in another resumable function
	 * to wait until the flash operation is finished.
	 */
	modm::ResumableResult<void>
	waitWhileBusy();

private:
	/** Send a non-addressed operation instruction to the spi flash chip
	 */
	modm::ResumableResult<void>
	spiOperation(Instruction instruction, const uint8_t* tx = nullptr, uint8_t* rx = nullptr, std::size_t length = 0, uint8_t dummyCycles = 0);

	/** Send an addressed operation instruction to the spi flash chip
	 */
	modm::ResumableResult<void>
	spiOperation(Instruction instruction, bd_address_t address, const uint8_t* tx = nullptr, uint8_t* rx = nullptr, std::size_t length = 0, uint8_t dummyCycles = 0);
};

}
#include "block_device_spiflash_impl.hpp"

#endif // MODM_BLOCK_DEVICE_SPIFLASH_HPP
