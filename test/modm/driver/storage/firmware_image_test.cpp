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

#include "firmware_image_test.hpp"

#include <modm/driver/storage/firmware_image.hpp>
#include <modm/driver/storage/block_device_heap.hpp>

#include <string>

namespace {

class TestBlockDevice : public modm::BdHeap<128> {
public:
	static constexpr bd_size_t BlockSizeRead = 2;
	static constexpr bd_size_t BlockSizeWrite = 4;
	static constexpr bd_size_t BlockSizeErase = 8;
};

}

void FirmwareImageTest::testSerializeEmpty() {
  TestBlockDevice device;
  device.initialize();
  modm::FirmwareImage<TestBlockDevice> image(&device);
  image.initialize(0, 0);
  uint32_t actual[2];
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.serialize((uint8_t*) actual, sizeof(actual))), sizeof(actual));
  uint32_t expected[] = {0x0, 0x0};
  TEST_ASSERT_EQUALS_ARRAY(expected, actual, sizeof(actual) / sizeof(uint32_t));
}

void FirmwareImageTest::testSerializeData() {
  TestBlockDevice device;
  device.initialize();
  modm::FirmwareImage<TestBlockDevice> image(&device);
  image.initialize(0, 0);
  uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
  image.append(data, sizeof(data));
  uint32_t actual[4];
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.serialize((uint8_t*) actual, sizeof(actual))), sizeof(actual));
  uint32_t crc = modm::math::crc32(data, sizeof(data));
  uint32_t expected[] = {/*size:*/ 8, /*data:*/ 0x04030201, 0x08070605, /*crc=*/ crc};
  TEST_ASSERT_EQUALS_ARRAY(expected, actual, sizeof(actual) / sizeof(uint32_t));
}


void FirmwareImageTest::testSerializeTwoSteps() {
  TestBlockDevice device;
  device.initialize();
  modm::FirmwareImage<TestBlockDevice> image(&device);
  image.initialize(0, 0);
  uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
  image.append(data, sizeof(data));
  {
    uint32_t actual[2];
    TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.serialize((uint8_t*) actual, sizeof(actual))), sizeof(actual));
    uint32_t expected[] = {/*size:*/ 8, /*data:*/ 0x04030201};
    TEST_ASSERT_EQUALS_ARRAY(expected, actual, sizeof(actual) / sizeof(uint32_t));
  }
  {
    uint32_t actual[2];
    TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.serialize((uint8_t*) actual, sizeof(actual))), sizeof(actual));
    uint32_t crc = modm::math::crc32(data, sizeof(data));
    uint32_t expected[] = {0x08070605, /*crc=*/ crc};
    TEST_ASSERT_EQUALS_ARRAY(expected, actual, sizeof(actual) / sizeof(uint32_t));
  }
}

void FirmwareImageTest::testDeserializeEmpty() {
  TestBlockDevice device;
  device.initialize();
  modm::FirmwareImage<TestBlockDevice> image(&device);
  image.initialize(0, 0);
  uint32_t data[] = {/*size:*/ 0, /*crc=*/ 0};
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.deserialize((uint8_t*) data, sizeof(data))), sizeof(data));
  TEST_ASSERT_EQUALS(image.size(), 0u);
  TEST_ASSERT_TRUE(image.crcOk());
}


void FirmwareImageTest::testDeserializeData() {
  TestBlockDevice device;
  device.initialize();
  modm::FirmwareImage<TestBlockDevice> image(&device);
  image.initialize(0, 0);
  uint32_t data[] = {/*size:*/ 8, /*data:*/ 0x04030201, 0x08070605, /*crc=*/ 0x0};
  data[3] = modm::math::crc32((uint8_t*) &data[1], 8);
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.deserialize((uint8_t*) data, sizeof(data))), sizeof(data));
  TEST_ASSERT_EQUALS(image.size(), 8u);
  TEST_ASSERT_TRUE(image.crcOk());
}


void FirmwareImageTest::testDeserializeTwoSteps() {
  TestBlockDevice device;
  device.initialize();
  modm::FirmwareImage<TestBlockDevice> image(&device);
  image.initialize(0, 0);
  uint32_t data[] = {/*size:*/ 8, /*data:*/ 0x04030201, 0x08070605, /*crc=*/ 0x0};
  data[3] = modm::math::crc32((uint8_t*) &data[1], 8);
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.deserialize((uint8_t*) data, 8u)), 8u);
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(image.deserialize((uint8_t*) data + 8u, 8u)), 8u);
  TEST_ASSERT_EQUALS(image.size(), 8u);
  TEST_ASSERT_TRUE(image.crcOk());
}
