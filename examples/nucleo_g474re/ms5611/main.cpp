// coding: utf-8
/*
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/pressure/ms5611.hpp>

using namespace Board;

using Cs = modm::platform::GpioC13;
using Mosi = modm::platform::GpioA7;
using Miso = modm::platform::GpioA6;
using Sck = modm::platform::GpioA5;

using SpiMaster = modm::platform::SpiMaster1;

class BarometerThread : public modm::pt::Protothread
{
public:
    BarometerThread() : barometer(data) {}

    bool
    run()
    {
        PT_BEGIN();

        while (PT_CALL(barometer.initialize()) == false)
        {
            MODM_LOG_ERROR << "Ms5611 PROM CRC failed" << modm::endl;
            timeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(timeout.isExpired());
        }

        MODM_LOG_INFO << "Ms5611 initialized and PROM CRC succeded" << modm::endl;

        prom = data.getProm();
        MODM_LOG_INFO << "MS5611 PROM Contents" << modm::endl;
        MODM_LOG_INFO << "C0: " << prom.data[0] << modm::endl;
        MODM_LOG_INFO << "C1: " << prom.data[1] << modm::endl;
        MODM_LOG_INFO << "C2: " << prom.data[2] << modm::endl;
        MODM_LOG_INFO << "C3: " << prom.data[3] << modm::endl;
        MODM_LOG_INFO << "C4: " << prom.data[4] << modm::endl;
        MODM_LOG_INFO << "C5: " << prom.data[5] << modm::endl;
        MODM_LOG_INFO << "C6: " << prom.data[6] << modm::endl;
        MODM_LOG_INFO << "C7: " << prom.data[7] << modm::endl;

        while (true)
        {
            PT_CALL(barometer.readout());

            data.getPressure(pressure);
            MODM_LOG_INFO << "Pressure: " << pressure << " mbar" << modm::endl;

            data.getTemperature(temperature);
            MODM_LOG_INFO << "Temperature: " << temperature << " degrees Centigrade" << modm::endl;

            timeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(timeout.isExpired());
        }

        PT_END();
    }

private:
    modm::ms5611data::Data data;
    modm::ms5611data::Prom prom;
    modm::Ms5611<SpiMaster, Cs> barometer;

    float pressure;
    float temperature;

    modm::ShortTimeout timeout;
} barometerThread;

int
main()
{
    Board::initialize();
    Cs::setOutput(modm::Gpio::High);

    SpiMaster::connect<Miso::Miso, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 21500000>();

    MODM_LOG_INFO       << "==========MS5611 Test==========" << modm::endl;
    MODM_LOG_DEBUG      << "Debug logging here" << modm::endl;
    MODM_LOG_INFO       << "Info logging here" << modm::endl;
    MODM_LOG_WARNING    << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR      << "Error logging here" << modm::endl;
    MODM_LOG_INFO       << "===============================" << modm::endl;

    while (true)
    {
        barometerThread.run();
    }

    return 0;
}