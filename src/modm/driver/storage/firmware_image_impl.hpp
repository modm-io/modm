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

template<class StorageBlockDevice>
modm::ResumableResult<bool>
FirmwareImage<StorageBlockDevice>::initialize(bd_address_t address, size_t size)
{
  RF_BEGIN();
	data_address_ = address;
	rw_address_ = address;
	size_ = 0;
//  flash_.initialize();
  device_->initialize();
  inverted_crc_ = modm::math::crc32_init;
  RF_END_RETURN_CALL(device_->erase(data_address_, size));
}

template<class StorageBlockDevice>
modm::ResumableResult<bool>
FirmwareImage<StorageBlockDevice>::append(uint8_t* buffer, size_t size)
{
  bool success;
  RF_BEGIN();
  success = RF_CALL(device_->program(buffer, data_address_ + size_, size));
  for (size_t index = 0; index < size; ++index)
    inverted_crc_ = modm::math::crc32_update(inverted_crc_, buffer[index]);
  size_ += size;
  RF_END_RETURN(success);
}

template<class StorageBlockDevice>
modm::ResumableResult<typename StorageBlockDevice::bd_size_t>
FirmwareImage<StorageBlockDevice>::serialize(uint8_t* buffer, bd_size_t buffer_size)
{
  RF_BEGIN();
  rw_bytes_ = 0;
  // 1. Store the image size.
  if (state_ == 0) {
    // Initialize variables.
    rw_address_ = data_address_;
    // Store size.
    *(uint32_t*) buffer = size_;
    rw_bytes_ += sizeof(uint32_t);
    state_ = 1;
    reader_.initialize(data_address_);
    if (sizeof(uint32_t) == buffer_size) {
      RF_RETURN(rw_bytes_);
    }
  }
  // 2. Store the image data.
  if (state_ == 1) {
    // Not having any local variables is hurting the following code badly...
    serialize_bytes_to_read_ = std::min((bd_address_t) size_ + data_address_ - rw_address_, buffer_size - rw_bytes_);
    //serialize_bytes_to_read_ = 8;
    RF_CALL(reader_.read(buffer + rw_bytes_, serialize_bytes_to_read_));
    rw_address_ += serialize_bytes_to_read_;
    rw_bytes_ += serialize_bytes_to_read_;
    // We are done when size_ bytes has been read.
    if (rw_address_ < data_address_ + size_) {
      RF_RETURN(rw_bytes_);
    } else {
      state_ = 2;
    }
  }
  // 3. Store the CRC.
  if (state_ == 2) {
    *(uint32_t*) (buffer + rw_bytes_) = crc();
    rw_bytes_ += sizeof(uint32_t);
    state_ = 0;
  }
  RF_END_RETURN(rw_bytes_);
}

template<class StorageBlockDevice>
modm::ResumableResult<typename StorageBlockDevice::bd_size_t>
FirmwareImage<StorageBlockDevice>::deserialize(uint8_t* buffer, bd_size_t buffer_size)
{
  RF_BEGIN();
  rw_bytes_ = 0;
  // 1. Parse the image size.
  if (state_ == 0) {
    size_ = *(size_t*) buffer;
    rw_bytes_ += sizeof(uint32_t);
    state_ = 1;
    rw_address_ = data_address_;
    writer_.initialize(data_address_);
    if (rw_bytes_ == buffer_size) {
      RF_RETURN(rw_bytes_);
    }
  }
  // 2. Parse the image data and concurrently compute the CRC.
  if (state_ == 1) {
    // Not having any local variables is hurting the following code badly...
    serialize_bytes_to_read_ = std::min((bd_address_t) size_ + data_address_ - rw_address_, buffer_size - rw_bytes_);
    rw_address_ += RF_CALL(writer_.write(buffer + rw_bytes_, serialize_bytes_to_read_));
    // Compute CRC
    for (size_t index = 0; index < serialize_bytes_to_read_; ++index) {
      inverted_crc_ = modm::math::crc32_update(inverted_crc_, buffer[rw_bytes_ + index]);
    }
    rw_bytes_ += serialize_bytes_to_read_;
    // We are done when size_ bytes has been written.
    if (rw_address_ < data_address_ + size_) {
      RF_RETURN(rw_bytes_);
    } else {
      state_ = 2;
    }
  }
  // 3. Compare the CRC.
  if (state_ == 2) {
    crc_ok_ = crc() == *(uint32_t*) (buffer + rw_bytes_);
    if (!crc_ok_)  {
      MODM_LOG_DEBUG << "[FirmwareImage::deserialize] CRC verification failed! " << modm::hex
        << "computed: " << crc() << ", parsed: " << *(uint32_t*) (buffer + rw_bytes_) << modm::endl;
    }
    rw_bytes_ += sizeof(uint32_t);
    state_ = 0;
  }
  RF_END_RETURN(rw_bytes_);
}
/*
template<class StorageBlockDevice>
modm_ramcode void
FirmwareImage<StorageBlockDevice>::apply()
{
  __disable_irq();
  RF_CALL_BLOCKING(flash_.erase(flash_address_, size_));
  while (data_address_ != rw_address_) {
    RF_CALL_BLOCKING(device_.read(block_buffer_, data_address_, BlockBufferSize));
    RF_CALL_BLOCKING(flash_.program(block_buffer_, flash_address_, BlockBufferSize));
    data_address_ += BlockBufferSize;
    flash_address_ += BlockBufferSize;
  }
  NVIC_SystemReset();
}
*/

} // namespace modm
