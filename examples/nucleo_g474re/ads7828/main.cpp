// coding: utf-8
/*
 * Copyright (c) 2022, Jonas Kazem  Andersen
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
#include <modm/driver/adc/ads7828.hpp>

using Scl = modm::platform::GpioC6;
using Sda = modm::platform::GpioC7;
using I2cMaster = modm::platform::I2cMaster4;

using namespace Board;

class AdcThread : public modm::pt::Protothread
{

public:
    AdcThread() : adc(data, 0x48)
    {
    }

    inline bool
    run()
    {
        PT_BEGIN();

        while (PT_CALL(adc.ping()) == false)
        {
            MODM_LOG_ERROR << "Could not ping Ads7828" << modm::endl;

            timeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(timeout.isExpired());
        }

        while (true)
        {
            MODM_LOG_INFO << "-------------------------------" << modm::endl << modm::endl;
            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch0));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch0 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch1));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch1 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch2));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch2 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch3));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch3 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch4));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch4 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch5));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch5 measuremnt is  \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch6));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch6 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch7));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch7 measuremnt is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            MODM_LOG_INFO << "----Diff Inputs-------------" << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch0Ch1));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch0 - Ch1 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch2Ch3));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch2 - Ch3 is\t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch4Ch5));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch4 - Ch5 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch6Ch7));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch6 - Ch7 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch1Ch0));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch1 - Ch0 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch3Ch2));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch3 - Ch2 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch5Ch4));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch5 - Ch4 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch7Ch6));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Ch7 - Ch6 is \t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            MODM_LOG_INFO << "---Toggling Power Down and Internal Ref----" << modm::endl;

            PT_CALL(adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOffAdcConverterOff));
            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch0));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Default: \t\t\t\t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOnAdcConverterOff));
            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch0));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Internal ref on:  \t\t\t %.4f", data.getVoltage(2.5f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOffAdcConverterOn));
            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch0));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("No power down \t\t\t\t %.4f", data.getVoltage(3.3f));
            MODM_LOG_INFO << modm::endl;

            PT_CALL(adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOnAdcConverterOn));
            PT_CALL(adc.startMeasurement(modm::ads7828::InputChannel::Ch0));
            PT_CALL(adc.readConversionResult());
            MODM_LOG_INFO.printf("Internal ref on, no power down: \t %.4f", data.getVoltage(2.5f));
            MODM_LOG_INFO << modm::endl;

            MODM_LOG_INFO << "-------------------------------" << modm::endl << modm::endl;

            timeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(timeout.isExpired());
        }
        PT_END();
    }

private:
    modm::ads7828::Data data;
    modm::Ads7828<I2cMaster> adc;

    modm::ShortTimeout timeout;
} adcThread;

int
main()
{
    Board::initialize();

    I2cMaster::connect<Scl::Scl, Sda::Sda>(I2cMaster::PullUps::Internal);
    I2cMaster::initialize<Board::SystemClock, 100_kHz>();


    MODM_LOG_INFO << "==========Ads7828 Test==========" << modm::endl;
    MODM_LOG_DEBUG << "Debug logging here" << modm::endl;
    MODM_LOG_INFO << "Info logging here" << modm::endl;
    MODM_LOG_WARNING << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR << "Error logging here" << modm::endl;
    MODM_LOG_INFO << "===============================" << modm::endl;

    while (true)
    {
        adcThread.run();
    }
    return 0;
}
