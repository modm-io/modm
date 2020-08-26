/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/block_device.hpp>
#include <modm/driver/storage/block_device_io.hpp>
#include <modm/math/utils/crc.hpp>
#include <modm/processing/resumable.hpp>

#include <algorithm>

namespace modm {

/**
 * \brief	Stores a firmware image for use in In-Application Programming (IAP).
 *
 * \ingroup	modm_driver_firmware_image
 * \author Erik Henriksson
 */
template<class StorageBlockDevice>
class FirmwareImage : NestedResumable<2> {
	using bd_address_t = typename StorageBlockDevice::bd_address_t;
	using bd_size_t = typename StorageBlockDevice::bd_size_t;
  static constexpr uint32_t BlockBufferSize =
      std::max(StorageBlockDevice::BlockSizeRead, StorageBlockDevice::BlockSizeWrite);
public:
	FirmwareImage(StorageBlockDevice* device) : device_(device), reader_(device), writer_(device) {}
	/** Initializes the Image.
	 *
	 *  @param address	The address in StorageBlockDevice to store the image.
	 *  @param size			Size of the storage where image data are saved.
	 *  @return					True on success
	 */
	modm::ResumableResult<bool>
	initialize(bd_address_t address, size_t size);

	/** Appends data to the image.
	 *
	 *  @param buffer	Buffer of data to write
	 *  @param size		Size to write in bytes, must be multiple of StorageBlockDevice::BlockSizeWrite.
	 *  @return				True on success
	 */
	modm::ResumableResult<bool>
	append(uint8_t* buffer, size_t size);

	/** Size of the Image data in bytes. */
	size_t size() { return size_; }

	/** CRC32 of the Image data. */
	uint32_t crc() { return ~inverted_crc_; }

  bool crcOk() { return crc_ok_; }

	/** Serializes the image, computes CRC and stores it in the buffer.
   *
   *  This can be called multiple times which will increment an internal read pointer.
	 *
	 *  The (internal) format is:
   *  1. Size of the image data in bytes: 4 bytes
   *  2. Image data: size() bytes
   *  3. CRC of the image data: 4 bytes
	 *
	 *  @param buffer	The address to store the image.
	 *  @param size	The size of the buffer.
	 *  @return			Number of bytes written
	 */
  modm::ResumableResult<bd_size_t>
  serialize(uint8_t* buffer, bd_size_t size);

	/** Size of the serialized image in bytes. */
	size_t serialized_size() { return size_ + 2 * sizeof(uint32_t); }

	/** Parses the image from the buffer.
   *
   *  This can be called multiple times which will increment an internal write pointer.
	 *
	 *  @param buffer	The address to store the image.
	 *  @param size	The size of the buffer.
	 *  @return			Number of bytes read
	 */
  modm::ResumableResult<bd_size_t>
  deserialize(uint8_t* buffer, bd_size_t size);

	/** Writes the image to flash and resets the CPU.
	 *
	 *  @param image	The firmware image to write.
	 */
//  void apply();

private:
	StorageBlockDevice* device_;
	BlockDeviceBufferedReader<StorageBlockDevice> reader_;
	BlockDeviceBufferedWriter<StorageBlockDevice> writer_;
//  Flash flash_;
	uint32_t size_;
	bd_address_t data_address_;
	uint32_t inverted_crc_;
	bool crc_ok_;
	bd_address_t rw_address_; // Tmp address into data used by R/W functions.
//	bd_address_t flash_address_ = Flash::DeviceStart;
  bd_size_t rw_bytes_;
  size_t serialize_bytes_to_read_;
  int state_ = 0;

  // modm::ResumableResult<bd_size_t>
  // programBuffered(bd_address_t dest, const uint8_t* src, bd_size_t size);
  // modm::ResumableResult<bd_size_t>
  // readBuffered(uint8_t* dest, bd_address_t src, bd_size_t size);
};

} // namespace modm

#include "firmware_image_impl.hpp"
