/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/storage/firmware_image.hpp>

// Workaround for that GCC does not honor section attribute on templated functions.
// See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70435
// template
// modm_ramcode void
// modm::FirmwareImage<Flash>::apply();

using namespace Board;
using modm::platform::Flash;

//#define MODM_LOG_LEVEL modm::log::ERROR

constexpr uint32_t firmware_addr = 0x10000u; // In my case, a blink sketch.
constexpr uint32_t firmware_size = 0x2000u; // Must be rounded up to multiple of 64 bytes.

Flash flash;

modm::FirmwareImage<Flash> image1(&flash);
modm::FirmwareImage<Flash> image2(&flash);

uint8_t buffer[64]; // This is the minimum buffer size because of the flash write block size.

int
main()
{
    Board::initialize();
    MODM_LOG_INFO << "Reboot" << modm::endl;
    MODM_LOG_INFO << "Firmware CRC: " << modm::hex
        << modm::math::crc32((uint8_t*) firmware_addr, firmware_size) << modm::endl;
    // Give image1 8KB memory backed by flash at address 0x12000. In a sensor network schenario,
    // this would reside on the gateway side.
    if (!RF_CALL_BLOCKING(image1.initialize(/*address=*/0x12000u, /*size=*/0x2000u)))
        MODM_LOG_ERROR << "Failed to initialize image1." << modm::endl;
    // Write some firmware that happens to be stored at firmware_addr to image1.
    if (!RF_CALL_BLOCKING(image1.append((uint8_t*) firmware_addr, firmware_size)))
        MODM_LOG_ERROR << "Failed to append data to image1." << modm::endl;
    MODM_LOG_INFO << "Image1 CRC: " << modm::hex << image1.crc() << modm::endl;
    // Give image2 8KB memory backed by flash at address 0x14000.
    if (!RF_CALL_BLOCKING(image2.initialize(/*address=*/0x14000u, /*size=*/0x2000u)))
        MODM_LOG_ERROR << "Failed to initialize image2." << modm::endl;
    // Simulates sending the image over some IO with block size of 64 bytes.
    for (size_t index = 0; index < image1.serialized_size(); index += sizeof(buffer)) {
        RF_CALL_BLOCKING(image1.serialize(buffer, sizeof(buffer)));
        RF_CALL_BLOCKING(image2.deserialize(buffer, sizeof(buffer)));
    }
    MODM_LOG_INFO << "Image2 CRC: " << modm::hex << image2.crc() << modm::endl;
    // Verify that the image2's CRC is good.
    MODM_LOG_INFO << "Image2 CRC valid: " << image2.crcOk() << modm::endl;
    if (image2.crcOk()) {
        // This will overwrite the firmware and reset the CPU.
        MODM_LOG_INFO << "Programming..." << modm::endl;
        //image2.apply();
    }
    // Will never get here.
    MODM_LOG_INFO << "Loop" << modm::endl;
    while(1);
    return 0;
}
