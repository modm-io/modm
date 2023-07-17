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
#include <modm/platform.hpp>
#include <modm/processing.hpp>

namespace
{
    volatile bool interrupt = false;
}

using SpiMaster = modm::platform::SpiMaster1;
using Mosi = GpioA7;
using Miso = GpioA6;
using Sck = GpioA5;
using Cs = GpioC5;

using Int1 = GpioC3;

class ImuThread : public modm::pt::Protothread, public modm::ixm42xxx
{
    using Transport = modm::Ixm42xxxTransportSpi< SpiMaster, Cs >;

public:
    ImuThread() : imu(fifoData), timer(std::chrono::milliseconds(500)) {}

    bool
    run()
    {
        PT_BEGIN();

        Int1::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Int1>(Exti::Trigger::RisingEdge, [](uint8_t)
        {
            interrupt = true;
        });

        /// Initialize the IMU and verify that it is connected
        PT_CALL(imu.initialize());
        while (not PT_CALL(imu.ping()))
        {
            MODM_LOG_ERROR << "Cannot ping IXM-42xxx" << modm::endl;
            PT_WAIT_UNTIL(timer.execute());
        }

        MODM_LOG_INFO << "IXM-42xxx Initialized" << modm::endl;
        MODM_LOG_INFO << "Fifo Buffer Size: " << fifoData.getFifoBufferSize() << modm::endl;

        /// Configure FIFO
        PT_CALL(imu.updateRegister(Register::FIFO_CONFIG, FifoMode_t(FifoMode::StopOnFull)));
        PT_CALL(imu.updateRegister(Register::FIFO_CONFIG1, FifoConfig1::FIFO_HIRES_EN | FifoConfig1::FIFO_TEMP_EN | FifoConfig1::FIFO_GYRO_EN | FifoConfig1::FIFO_ACCEL_EN));
        PT_CALL(imu.writeFifoWatermark(1024));

        /// Configure interrupt
        PT_CALL(imu.updateRegister(Register::INT_CONFIG, IntConfig::INT1_MODE | IntConfig::INT1_DRIVE_CIRCUIT | IntConfig::INT1_POLARITY));
        PT_CALL(imu.updateRegister(Register::INT_CONFIG1, IntConfig1::INT_ASYNC_RESET));
        PT_CALL(imu.updateRegister(Register::INT_SOURCE0, IntSource0::FIFO_THS_INT1_EN | IntSource0::FIFO_FULL_INT1_EN, IntSource0::UI_DRDY_INT1_EN));

        /// Configure data sensors
        PT_CALL(imu.updateRegister(Register::GYRO_CONFIG0, GyroFs_t(GyroFs::dps2000) | GyroOdr_t(GyroOdr::kHz1)));
        PT_CALL(imu.updateRegister(Register::ACCEL_CONFIG0, AccelFs_t(AccelFs::g16) | AccelOdr_t(AccelOdr::kHz1)));
        PT_CALL(imu.updateRegister(Register::PWR_MGMT0, GyroMode_t(GyroMode::LowNoise) | AccelMode_t(AccelMode::LowNoise)));

        while (true)
        {
            if (interrupt)
            {
                PT_CALL(imu.readRegister(Register::INT_STATUS, &intStatus.value));
                interrupt = false;
            }

            if (intStatus.any(IntStatus::FIFO_FULL_INT | IntStatus::FIFO_THS_INT))
            {
                if (PT_CALL(imu.readFifoData()))
                {
                    // Count packets in FIFO buffer and print contents of last packet
                    uint16_t count = 0;
                    modm::ixm42xxxdata::FifoPacket fifoPacket;
                    for (const auto &packet : fifoData)
                    {
                        fifoPacket = packet;
                        count++;
                    }

                    float temp;
                    modm::Vector3f accel;
                    modm::Vector3f gyro;

                    fifoPacket.getTemp(&temp);
                    fifoPacket.getAccel(&accel, fifoData.getAccelScale());
                    fifoPacket.getGyro(&gyro, fifoData.getGyroScale());

                    MODM_LOG_INFO.printf("Temp: %f\n", temp);
                    MODM_LOG_INFO.printf("Accel: (%f, %f, %f)\n", accel.x, accel.y, accel.z);
                    MODM_LOG_INFO.printf("Gyro: (%f, %f, %f)\n", gyro.x, gyro.y, gyro.z);
                    MODM_LOG_INFO.printf("FIFO packet count: %u\n", count);
                }
                intStatus.reset(IntStatus::FIFO_FULL_INT | IntStatus::FIFO_THS_INT);
            }
        }
        PT_END();
    }

private:

    /// Due to the non-deterministic nature of system operation, driver memory allocation should always be the largest size of 2080 bytes.
    modm::ixm42xxxdata::FifoData<2080> fifoData;
    modm::ixm42xxx::IntStatus_t intStatus;
    modm::Ixm42xxx< Transport > imu;

    modm::PeriodicTimer timer;

} imuThread;

int
main()
{
    Board::initialize();

    Cs::setOutput(modm::Gpio::High);
    SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 21.5_MHz>();

    MODM_LOG_INFO       << "==========IXM-42xxx Test==========" << modm::endl;
    MODM_LOG_DEBUG      << "Debug logging here" << modm::endl;
    MODM_LOG_INFO       << "Info logging here" << modm::endl;
    MODM_LOG_WARNING    << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR      << "Error logging here" << modm::endl;
    MODM_LOG_INFO       << "==================================" << modm::endl;

    while (true)
    {
        imuThread.run();
    }

    return 0;
}
