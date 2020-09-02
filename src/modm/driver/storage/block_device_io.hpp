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
#include <modm/processing/resumable.hpp>

#include <algorithm>

namespace modm {

/**
 * \brief	Allows reading arbitrary block sizes from a BlockDevice.
 *
 * \ingroup	modm_driver_firmware_image
 * \author Erik Henriksson
 */
template<class BlockDevice_t>
class BlockDeviceBufferedReader : protected modm::NestedResumable<4> {
private:
  using bd_address_t = typename BlockDevice_t::bd_address_t;
  using bd_size_t = typename BlockDevice_t::bd_size_t;
  static constexpr uint32_t BufferSize =
      std::max(BlockDevice_t::BlockSizeRead, BlockDevice_t::BlockSizeWrite);

public:
  BlockDeviceBufferedReader(BlockDevice_t* device) : device_(device) {};

  /** Initializes the reader at address addr. */
  modm::ResumableResult<bool>
  initialize(bd_address_t addr);

  /** Reads size bytes into dest and increments the data pointer. */
  modm::ResumableResult<bd_size_t>
  read(uint8_t* dest, bd_size_t size);

private:
  BlockDevice_t* device_;
  bd_address_t address_;
  uint8_t buffer_[BufferSize];
  uint8_t pos_;
  bd_size_t read_bytes_left_;
};

/**
 * \brief	Allows writing arbitrary block sizes to a BlockDevice.
 *
 * \ingroup	modm_driver_firmware_image
 * \author Erik Henriksson
 */
template<class BlockDevice_t>
class BlockDeviceBufferedWriter : protected modm::NestedResumable<4> {
private:
  using bd_address_t = typename BlockDevice_t::bd_address_t;
  using bd_size_t = typename BlockDevice_t::bd_size_t;
  static constexpr uint32_t BufferSize =
      std::max(BlockDevice_t::BlockSizeRead, BlockDevice_t::BlockSizeWrite);

public:
  BlockDeviceBufferedWriter(BlockDevice_t* device) : device_(device) {};

  /** Initializes the writer at address addr. */
  modm::ResumableResult<bool>
  initialize(bd_address_t addr);

  /** Writes size bytes from src and increments the data pointer.
   *
   * @return number of bytes written to the BlockDevice.
  */
  modm::ResumableResult<bd_size_t>
  write(const uint8_t* src, bd_size_t size);

  /** Fills remaining space in page buffer with undefined data and writes it to the BlockDevice.
   *
   * @return number of bytes written to the BlockDevice.
   */
  modm::ResumableResult<bd_size_t>
  flush();

private:
  BlockDevice_t* device_;
  bd_address_t address_;
  uint8_t buffer_[BufferSize];
  uint8_t pos_;
  bd_size_t write_bytes_left_;
};

} // namespace modm

#include "block_device_io_impl.hpp"
