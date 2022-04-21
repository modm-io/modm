// coding: utf-8
/*
 * Copyright (c) 2022, Sarah Vilete
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
#include <modm/driver/temperature/max31855.hpp>

using SpiMaster = modm::platform::SpiMaster2;

using Cs = modm::platform::GpioA10;
using Miso = modm::platform::GpioB14;
using Sck = modm::platform::GpioB13;

using namespace Board;

class ThermocoupleThread : public modm::pt::Protothread
{
public:
    ThermocoupleThread() : thermocouple(data) {}

    bool
    run()
    {
        PT_BEGIN();

        thermocouple.initialize();

        MODM_LOG_INFO << "Max38155 initialized succeded" << modm::endl;

        while (true)
        {
            PT_CALL(thermocouple.readout());

            switch (data.getFault())
            {
            case modm::max31855::Fault::ShortCircuitVcc:
                MODM_LOG_ERROR << "Thermocouple error short circuit vcc" << modm::endl;
                break;

            case modm::max31855::Fault::ShortCircuitGnd:
                MODM_LOG_ERROR << "Thermocouple error short circuit gnd" << modm::endl;
                break;

            case modm::max31855::Fault::OpenCircuit:
                MODM_LOG_ERROR << "Thermocouple error open circuit" << modm::endl;
                break;

            default:
                MODM_LOG_INFO << "Thermocouple Temperature: " << data.getThermocoupleTemperature() << " degrees Centigrade" << modm::endl;
                MODM_LOG_INFO << "Internal Temperature: " << data.getReferenceJunctionTemperature() << " degrees Centigrade" << modm::endl;
                break;
            }

            timeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(timeout.isExpired());
        }

        PT_END();
    }

private:
    modm::max31855::Data data;
    modm::Max31855<SpiMaster, Cs> thermocouple;

    modm::ShortTimeout timeout;
} thermocoupleThread;

int
main()
{
    Board::initialize();
    Cs::setOutput(modm::Gpio::High);

    SpiMaster::connect<Miso::Miso, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 656250>();

    MODM_LOG_INFO       << "==========MAX 31855 Test==========" << modm::endl;
    MODM_LOG_DEBUG      << "Debug logging here" << modm::endl;
    MODM_LOG_INFO       << "Info logging here" << modm::endl;
    MODM_LOG_WARNING    << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR      << "Error logging here" << modm::endl;
    MODM_LOG_INFO       << "===============================" << modm::endl;

    while (true)
    {
        thermocoupleThread.run();
    }

    return 0;
}