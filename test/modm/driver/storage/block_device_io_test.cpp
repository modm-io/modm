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

#include "block_device_io_test.hpp"

#include <modm/driver/storage/block_device_io.hpp>
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

void BlockDeviceIOTest::testWrite() {
  TestBlockDevice device;
  device.initialize();
  modm::BlockDeviceBufferedWriter writer(&device);
  writer.initialize(0);
  {
    std::string s("he");
    TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(writer.write((uint8_t*) s.data(), s.size())), 0u);
    uint8_t actual[4] = {};
    RF_CALL_BLOCKING(device.read(actual, 0, sizeof(actual)));
    std::string expected("\0\0\0\0");
    TEST_ASSERT_EQUALS_ARRAY(expected.data(), (char*) actual, sizeof(actual));
  }
  {
    std::string s("ll");
    TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(writer.write((uint8_t*) s.data(), s.size())), 4u);
    uint8_t actual[4] = {};
    RF_CALL_BLOCKING(device.read(actual, 0, sizeof(actual)));
    std::string expected("hell");
    TEST_ASSERT_EQUALS_ARRAY(expected.data(), (char*) actual, sizeof(actual));
  }
  {
    std::string s("o world!");
    TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(writer.write((uint8_t*) s.data(), s.size())), 8u);
    uint8_t actual[12] = {};
    RF_CALL_BLOCKING(device.read(actual, 0, sizeof(actual)));
    std::string expected("hello world!");
    TEST_ASSERT_EQUALS_ARRAY(expected.data(), (char*) actual, sizeof(actual));
  }
}

void BlockDeviceIOTest::testFlush() {
  TestBlockDevice device;
  device.initialize();
  modm::BlockDeviceBufferedWriter writer(&device);
  writer.initialize(0);
  std::string s("tst");
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(writer.write((uint8_t*) s.data(), s.size())), 0u);
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(writer.flush()), 4u);
  uint8_t actual[3] = {};
  RF_CALL_BLOCKING(device.read(actual, 0, sizeof(actual)));
  std::string expected = s;
  TEST_ASSERT_EQUALS_ARRAY(expected.data(), (char*) actual, sizeof(actual));
}

void BlockDeviceIOTest::testRead() {
  TestBlockDevice device;
  device.initialize();
  std::string data("hello world!");
  RF_CALL_BLOCKING(device.write((uint8_t*) data.data(), 0, data.size()));
  modm::BlockDeviceBufferedReader reader(&device);
  reader.initialize(0);
  uint8_t actual[5] = {};
  TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(reader.read(actual, sizeof(actual))), sizeof(actual));
  std::string expected("hello");
  TEST_ASSERT_EQUALS_ARRAY(expected.data(), (char*) actual, sizeof(actual));
}