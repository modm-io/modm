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
#include <modm/driver/adc/ads101x.hpp>

using Scl = modm::platform::GpioC6;
using Sda = modm::platform::GpioC7;
using I2cMaster = modm::platform::I2cMaster4;

using namespace Board;

class AdcThread : public modm::pt::Protothread
{

public:
    AdcThread() : adc(data)
    {
    }

    inline bool
    run()
    {
        PT_BEGIN();

        // test of communication and initialization
        while (not PT_CALL(adc.ping()))
        {
            MODM_LOG_ERROR << "Pinging Ads101x failed" << modm::endl;

            shortTimeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(shortTimeout.isExpired());
        }

        while (not PT_CALL(adc.initialize()))
        {
            MODM_LOG_ERROR << "Initialising Ads101x failed" << modm::endl;

            shortTimeout.restart(std::chrono::milliseconds(1000));
            PT_WAIT_UNTIL(shortTimeout.isExpired());
        }

        MODM_LOG_INFO << "Ads101x successfully initialised" << modm::endl;

        while (true)
        {
            // Single shot and full scale range test
            MODM_LOG_INFO << "Started single conversion test" << modm::endl;
            timeout.restart(std::chrono::seconds(10));
            while (not timeout.isExpired())
            {
                // Test single shoot mode with full scale range 0.256
                PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V0_256));
                PT_CALL(adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4));
                while (PT_CALL(adc.isBusy()))
                {
                    PT_YIELD();
                }

                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Single Conversion result in range 0.256V: " << data.getVoltage() << modm::endl;

                // Test single shoot mode with full scale range 0.512
                PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V0_512));
                PT_CALL(adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4));
                while (PT_CALL(adc.isBusy()))
                {
                    PT_YIELD();
                }

                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Single Conversion result in range 0.512V: " << data.getVoltage() << modm::endl;

                // Test single shoot mode with full scale range 1.024
                PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V1_024));
                PT_CALL(adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4));
                while (PT_CALL(adc.isBusy()))
                {
                    PT_YIELD();
                }

                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Single Conversion result in range 1.024V: " << data.getVoltage() << modm::endl;

                // Test single shoot mode with full scale range 2.048
                PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V2_048));
                PT_CALL(adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4));
                while (PT_CALL(adc.isBusy()))
                {
                    PT_YIELD();
                }

                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Single Conversion result in range 2.048V: " << data.getVoltage() << modm::endl;

                // Test single shoot mode with full scale range 4.096
                PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V4_096));
                PT_CALL(adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4));
                while (PT_CALL(adc.isBusy()))
                {
                    PT_YIELD();
                }

                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Single Conversion result in range 4.096V: " << data.getVoltage() << modm::endl;

                // Test single shoot mode with full scale range 6.144
                PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V6_144));
                PT_CALL(adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4));
                while (PT_CALL(adc.isBusy()))
                {
                    PT_YIELD();
                }

                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Single Conversion result in range 6.144V: " << data.getVoltage() << modm::endl;

                shortTimeout.restart(std::chrono::milliseconds(1000));
                PT_WAIT_UNTIL(shortTimeout.isExpired());
            }

            // Continuous mode test
            MODM_LOG_INFO << "Started continuous conversion test" << modm::endl;
            PT_CALL(adc.setFullScaleRange(modm::ads101x::FullScaleRange::V6_144));
            PT_CALL(adc.startContinuousConversion(modm::ads101x::InputMultiplexer::Input4));

            timeout.restart(std::chrono::seconds(10));
            while (not timeout.isExpired())
            {
                shortTimeout.restart(std::chrono::milliseconds(1000));
                PT_WAIT_UNTIL(shortTimeout.isExpired());
                PT_CALL(adc.readConversionResult());
                MODM_LOG_INFO << "Continuous Conversion result in range 6.144V: " << data.getVoltage() << modm::endl;
            }
        }
        PT_END();
    }

private:
    modm::ads101x::Data data;
    modm::Ads101x<I2cMaster> adc;

    modm::Timeout timeout;
    modm::ShortTimeout shortTimeout;

} adcThread;

int
main()
{
    Board::initialize();

    I2cMaster::connect<Scl::Scl, Sda::Sda>(I2cMaster::PullUps::Internal);
    I2cMaster::initialize<Board::SystemClock, 100_kHz>();

    MODM_LOG_INFO       << "==========ADS101x Test==========" << modm::endl;
    MODM_LOG_DEBUG      << "Debug logging here" << modm::endl;
    MODM_LOG_INFO       << "Info logging here" << modm::endl;
    MODM_LOG_WARNING    << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR      << "Error logging here" << modm::endl;
    MODM_LOG_INFO       << "===============================" << modm::endl;

    while (true)
    {
        adcThread.run();
    }

    return 0;
}