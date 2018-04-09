// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
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
 * \brief	Block device with SPI Flash (Microchip SST26VF064B)
 *
 * 64MBit flash chip in SOIJ-8, WDFN-8 or SOIC-16
 *
 * \tparam Spi			The SpiMaster interface
 * \tparam Cs			The GpioOutput pin connected to the flash chip select
 * \tparam flashSize	Flash chip size in byte
 *
 * The `read()`, `erase()`,`program()` and `write()` methodes wait for
 * the chip to finish writing to the flash.
 *
 * \ingroup	driver_storage
 * \author	Raphael Lehmann
 */
template <typename Spi, typename Cs, uint32_t flashSize>
class BdSpiFlash : public modm::BlockDevice, public modm::SpiDevice< Spi >, protected NestedResumable<5>
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
	static constexpr bd_size_t BlockSizeRead = 1;
	static constexpr bd_size_t BlockSizeWrite = 256;
	static constexpr bd_size_t BlockSizeErase = 4 * 1024;
	static constexpr bd_size_t DeviceSize = flashSize;

private:
	uint8_t instructionBuffer[7];
	uint8_t i;

	uint8_t resultBuffer[3];
	uint32_t result;

	size_t index;

	// See http://ww1.microchip.com/downloads/en/DeviceDoc/20005119G.pdf p.13
	enum class
	Instruction : uint8_t
	{
		//Configuration
		Nop		= 0x00, /// No Operation
		RstEn	= 0x66, /// Reset Enable
		Rst		= 0x99, /// Reset Memory
		//EQio	= 0x38, /// Enable Quad IO
		//RstQio	= 0xff, /// reste Quad IO
		RdSr	= 0x05, /// Read Status Register
		WrSr	= 0x01, /// Write Status Register
		RdCr	= 0x35, /// Read Configuration Register
		// Read
		Read	= 0x03, /// Read
		ReadHS	= 0x0B, /// Read High Speed
		SQOR	= 0x6B, /// SPI Quad Output Read
		SQIOR	= 0xEB, /// SPI Quad I/O Read
		SDOR	= 0x3B, /// SPI Dual Output Read
		SDIOR	= 0xBB, /// SPI Dual I/O Read
		SB		= 0xC0, /// Set Burst Length
		RBSQI	= 0x0C, /// SQI Read Burst with Wrap
		RBSPI	= 0xEC, /// SPI Read Burst with Wrap
		// Identification
		JedecId	= 0x9f, /// JEDEC-ID Read
		QuadJId	= 0xAF, /// Quad I/O J-ID Read
		SFDP	= 0x5A, /// Serial Flash Discoverable Parameters
		// Write
		WrEn	= 0x06, /// Write Enable
		WrDi	= 0x04, /// Write Disable
		SE		= 0x20, /// Erase 4 KBytes of Memory Array
		BE		= 0xD8, /// Erase 64, 32 or 8 KBytes of Memory Array
		CE		= 0xC7, /// Erase Full Array
		PP		= 0x02, /// Page Program
		SQPP	= 0x32, /// SQI Quad Page Program
		WRSU	= 0xB0, /// Suspends Program/Erase
		WRRE	= 0x30, /// Resumes Program/Erase
		// Protection
		RBpR	= 0x72, /// Read Block-Protection Register
		WBpR	= 0x42, /// Write Block-Protection Register
		LBpR	= 0x8D, /// Lock Down Block-Protection Register
		nVWLDR	= 0xE8, /// non-Volatile Write Lock-Down Register
		ULBPR	= 0x98, /// Global Block Protection Unlock
		RSID	= 0x88, /// Read Security ID
		PSID	= 0xA5, /// Program User Security ID area
		LSID	= 0x85, /// Lockout Security ID Programming
	};

private:
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

	/** Send an operation instruction to the spi flash chip
	 */
	modm::ResumableResult<void>
	spiOperation(Instruction instruction, size_t dataLength = 0, uint8_t* rxData = nullptr, const uint8_t* txData = nullptr, uint32_t address = UINT32_MAX, uint8_t nrDummyCycles = 0);
};

}
#include "block_device_spiflash_impl.hpp"

#endif // MODM_BLOCK_DEVICE_SPIFLASH_HPP
