/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/driver/storage/block_device_heap.hpp>
#include <modm/driver/storage/firmware_image.hpp>

#include <fstream>


modm::FirmwareImage<modm::BdHeap<0x2000>> image1, image2;

int
main()
{
  image1.initialize(0, 0x2000);
	std::ifstream firmwareFile("firmware.bin", std::ifstream::binary);
  uint8_t buffer[64];
  while (firmwareFile.read((char*) buffer, sizeof(buffer))) {
    if (!RF_CALL_BLOCKING(image1.append(buffer, 64))) {
      MODM_LOG_ERROR << "Failed to append data to image1." << modm::endl;
      break;
    };
  }
  MODM_LOG_INFO << "Image1 CRC: " << modm::hex << image1.crc() << modm::endl;
  MODM_LOG_INFO << "buffer addr: " << modm::hex << buffer << modm::endl;
  MODM_LOG_INFO << "image1 addr: " << modm::hex << &image1 << modm::endl;

  image2.initialize(0, 0x2000);
  for (size_t index = 0; index < image1.serialized_size(); index += sizeof(buffer)) {
    RF_CALL_BLOCKING(image1.serialize(buffer, sizeof(buffer)));
    RF_CALL_BLOCKING(image2.deserialize(buffer, sizeof(buffer)));
  }
  MODM_LOG_INFO << "Image2 CRC: " << modm::hex << image2.crc() << modm::endl;
  MODM_LOG_INFO << "Image2 CRC valid: " << image2.crcOk() << modm::endl;

  return 0;
}
