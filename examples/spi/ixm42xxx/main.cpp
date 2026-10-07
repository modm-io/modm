// coding: utf-8
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/inertial/ixm42xxx.hpp>
#include <modm/math.hpp>
#include <modm/platform.hpp>
#include <modm/processing.hpp>

using SpiMaster = modm::platform::SpiMaster1;
using Mosi = GpioA7;
using Miso = GpioA6;
using Sck = GpioA5;
using Cs = GpioC5;
using Transport = modm::Ixm42xxxTransportSpi< SpiMaster, Cs >;

modm::ixm42xxxdata::Data data;
modm::Ixm42xxx< Transport > imu{data};

modm::Fiber fiber_sensor([]
{
    /// Initialize the IMU and verify that it is connected
    imu.initialize();
    while (not imu.ping())
    {
        MODM_LOG_ERROR << "Cannot ping IXM42xxx" << modm::endl;
        modm::this_fiber::sleep_for(100ms);
    }
    using ixm = modm::ixm42xxx;

    /// Configure data sensors
    imu.updateRegister(ixm::Register::GYRO_CONFIG0,
                       ixm::GyroFs_t(ixm::GyroFs::dps2000) | ixm::GyroOdr_t(ixm::GyroOdr::kHz1));
    imu.updateRegister(ixm::Register::ACCEL_CONFIG0,
                       ixm::AccelFs_t(ixm::AccelFs::g16) | ixm::AccelOdr_t(ixm::AccelOdr::kHz1));
    imu.updateRegister(ixm::Register::PWR_MGMT0,
                       ixm::GyroMode_t(ixm::GyroMode::LowNoise) | ixm::AccelMode_t(ixm::AccelMode::LowNoise));

    while (true)
    {
        modm::this_fiber::sleep_for(500ms);
        imu.readSensorData();

        float temp;
        modm::Vector3f accel;
        modm::Vector3f gyro;
        data.getTemp(&temp);
        data.getAccel(&accel);
        data.getGyro(&gyro);

        MODM_LOG_INFO.printf("Temp: %.3f\n", temp);
        MODM_LOG_INFO.printf("Accel: (%.3f, %.3f, %.3f)\n", accel.x, accel.y, accel.z);
        MODM_LOG_INFO.printf("Gyro: (%.3f, %.3f, %.3f)\n", gyro.x, gyro.y, gyro.z);
    }
});

modm::Fiber fiber_blink([]
{
    Board::LedD13::setOutput();
    while(true)
    {
        Board::LedD13::toggle();
        modm::this_fiber::sleep_for(0.5s);
    }
});

int
main()
{
    Board::initialize();

    Cs::setOutput(modm::Gpio::High);
    SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 21.5_MHz>();

    MODM_LOG_INFO       << "==========IXM-42xxx Test==========" << modm::endl;

    modm::fiber::Scheduler::run();

    return 0;
}
