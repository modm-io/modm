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

#include <modm/debug/logger.hpp>

namespace modm {

template <class BlockDevice>
modm::ResumableResult<bool>
modm::BlockDeviceBufferedWriter<BlockDevice>::initialize(bd_address_t addr)
{
  RF_BEGIN();
  address_ = addr;
  pos_ = 0;
  RF_END_RETURN(true);
}

template <class BlockDevice>
modm::ResumableResult<typename modm::BlockDeviceBufferedWriter<BlockDevice>::bd_size_t>
modm::BlockDeviceBufferedWriter<BlockDevice>::write(const uint8_t* src, bd_size_t size)
{
  RF_BEGIN();
  write_bytes_left_ = size;
  while (1) {
    if (pos_ != BufferSize) {
      if (write_bytes_left_ + pos_ < BufferSize) {
        memcpy(buffer_ + pos_, src, write_bytes_left_);
        pos_ += write_bytes_left_;
        RF_RETURN(size - write_bytes_left_);
      } else {
        memcpy(
          buffer_ + pos_,
          src + (size - write_bytes_left_),
          BufferSize - pos_);
        pos_ = BufferSize;
      }
    }
    // Invariant: pos_ == BufferSize
    if (!RF_CALL(device_->program(buffer_, address_, BufferSize))) {
      RF_RETURN(size - write_bytes_left_);
    }
    pos_ = 0;
    address_ += BufferSize;
    if (write_bytes_left_ < BufferSize) {
      RF_RETURN(BufferSize + size - write_bytes_left_);
    }
    write_bytes_left_ -= BufferSize;
  }
  RF_END_RETURN(size - write_bytes_left_);
}

template <class BlockDevice>
modm::ResumableResult<typename modm::BlockDeviceBufferedWriter<BlockDevice>::bd_size_t>
modm::BlockDeviceBufferedWriter<BlockDevice>::flush()
{
  RF_BEGIN();
  if (!RF_CALL(device_->program(buffer_, address_, BufferSize))) {
    RF_RETURN(0);
  }
  RF_END_RETURN(BufferSize);
}

template <class BlockDevice>
modm::ResumableResult<bool>
modm::BlockDeviceBufferedReader<BlockDevice>::initialize(bd_address_t addr)
{
  RF_BEGIN();
  address_ = addr;
  pos_ = BufferSize;
  RF_END_RETURN(true);
}

template <class BlockDevice>
modm::ResumableResult<typename modm::BlockDeviceBufferedReader<BlockDevice>::bd_size_t>
modm::BlockDeviceBufferedReader<BlockDevice>::read(uint8_t* dest, bd_size_t size)
{
  RF_BEGIN();
  read_bytes_left_ = size;
  while (read_bytes_left_) {
    if (pos_ != BufferSize) {
      if (read_bytes_left_ + pos_ <= BufferSize) {
        memcpy(dest + (size - read_bytes_left_), buffer_ + pos_, read_bytes_left_);
        pos_ += read_bytes_left_;
        RF_RETURN(size);
      } else {
        memcpy(dest + (size - read_bytes_left_), buffer_ + pos_, BufferSize - pos_);
        read_bytes_left_ -= BufferSize - pos_;
      }
    }
    // Invariant: pos_ == BufferSize
    if (!RF_CALL(device_->read(buffer_, address_, BufferSize))) {
      RF_RETURN(size - read_bytes_left_);
    }
    pos_ = 0;
    address_ += BufferSize;
  }
  RF_END_RETURN(size - read_bytes_left_);
}

} // namespace modm