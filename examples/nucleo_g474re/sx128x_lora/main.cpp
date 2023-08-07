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

#include <atomic>
#include <cstring>

#include <modm/board.hpp>
#include <modm/debug/logger/logger.hpp>
#include <modm/driver/radio/sx128x.hpp>
#include <modm/platform.hpp>
#include <modm/processing.hpp>

using Sck = GpioA5;
using Miso = GpioA6;
using Mosi = GpioA7;
using SpiMaster = modm::platform::SpiMaster1;

namespace
{

namespace rx
{
    std::atomic_bool dio1 = false;
    std::atomic_bool dio2 = false;
    std::atomic_bool dio3 = false;
}

namespace tx
{
    std::atomic_bool dio1 = false;
    std::atomic_bool dio2 = false;
    std::atomic_bool dio3 = false;
}

static constexpr modm::sx128x::LoRa::ModulationParams modulationParams = {
    .spreadingFactor = modm::sx128x::LoRa::ModulationParams::SpreadingFactor::Sf9,
    .bandwidth = modm::sx128x::LoRa::ModulationParams::Bandwidth::Bw400,
    .codingRate = modm::sx128x::LoRa::ModulationParams::CodingRate::Cr_Li_4_7
};

static constexpr modm::sx128x::LoRa::PacketParams packetParams = {
    .preambleLength = 12,
    .headerType = modm::sx128x::LoRa::PacketParams::HeaderType::Explicit,
    .payloadLength = 4,
    .crc = modm::sx128x::LoRa::PacketParams::Crc::Enable,
    .invertIq = modm::sx128x::LoRa::PacketParams::InvertIq::Standard
};

}

class RxThread : public modm::sx128x, public modm::pt::Protothread
{
    using Reset = GpioB3;
    using Busy = GpioB4;
    using Dio1 = GpioB5;
    using Dio2 = GpioB6;
    using Dio3 = GpioB7;

    using Nss = GpioD2;
    using Transport = modm::Sx128xTransportSpi<SpiMaster, Nss>;

public:
    RxThread() {}

    inline bool
    run()
    {
        PT_BEGIN();

        Nss::setOutput(modm::Gpio::High);
        Reset::setOutput(modm::Gpio::Low);
        Busy::setInput(modm::platform::Gpio::InputType::PullDown);

        Dio1::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Dio1>(Exti::Trigger::RisingEdge, [](uint8_t) { rx::dio1 = true; });

        Dio2::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Dio2>(Exti::Trigger::RisingEdge, [](uint8_t) { rx::dio2 = true; });

        Dio3::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Dio3>(Exti::Trigger::RisingEdge, [](uint8_t) { rx::dio3 = true; });

        PT_CALL(radio.reset());
        PT_CALL(radio.setStandby());

        // Initialize the sx128x
        PT_CALL(radio.setPacketType(PacketType::LoRa));
        PT_CALL(radio.setRfFrequency(2457_MHz / radio.frequencyLsb));
        PT_CALL(radio.setRegulatorMode(RegulatorMode::Ldo));
        PT_CALL(radio.setBufferBaseAddress(0, 0));
        PT_CALL(radio.setModulationParams(modulationParams));
        PT_CALL(radio.writeRegister(Register::SfAdditionalConfiguration, 0x32));
        PT_CALL(radio.writeRegister(Register::FrequencyErrorCorrection, 0x01));
        PT_CALL(radio.setPacketParams(packetParams));
        PT_CALL(radio.setDioIrqParams(Irq::RxDone | Irq::RxTxTimeout, Irq::RxDone, Irq::RxTxTimeout));
        PT_CALL(radio.setRx(sx128x::PeriodBase::ms1, 1000));

        MODM_LOG_DEBUG << "Sx128x initialization complete!" << modm::endl;

        while (true)
        {
            if (rx::dio1.exchange(false))
            {
                PT_CALL(radio.getIrqStatus(&irqStatus));
                if (irqStatus.any(Irq::RxDone))
                {
                    PT_CALL(radio.clearIrqStatus(Irq::RxDone | Irq::RxTxTimeout));
                    PT_CALL(radio.setRx(sx128x::PeriodBase::ms1, 1000));

                    PT_CALL(radio.getRxBufferStatus(&rxBufferStatus));
                    PT_CALL(radio.getPacketStatus(&packetStatus));
                    PT_CALL(radio.readBuffer(rxBufferStatus.rxStartBufferPointer, std::span{buffer, rxBufferStatus.rxPayloadLength}));

                    if (rxBufferStatus.rxPayloadLength > 0)
                    {
                        uint32_t counter;
                        std::memcpy((uint8_t *) &counter, buffer, sizeof(counter));
                        MODM_LOG_DEBUG << "Received Message" << modm::endl;
                        MODM_LOG_DEBUG << "Counter: " << counter << modm::endl;
                    }
                }
            }

            if (rx::dio2.exchange(false))
            {
                PT_CALL(radio.getIrqStatus(&irqStatus));
                if (irqStatus.any(Irq::RxTxTimeout))
                {
                    // RxTxTimeout Interrupt received!
                    // Clear irq and set to rx again.
                    PT_CALL(radio.clearIrqStatus(Irq::RxTxTimeout));
                    PT_CALL(radio.setRx(sx128x::PeriodBase::ms1, 1000));
                    MODM_LOG_DEBUG << "RxTxTimeout Interrupt!" << modm::endl;
                }
            }
            PT_YIELD();
        }
        PT_END();
    }

private:
    Irq_t irqStatus;
    PacketStatus packetStatus;
    RxBufferStatus rxBufferStatus;

    modm::Sx128x< Transport, Reset, Busy > radio;

private:
    static constexpr size_t bufferSize = 256;
    uint8_t buffer[bufferSize];
} rxThread;

class TxThread : public modm::sx128x, public modm::pt::Protothread
{
    using Reset = modm::platform::GpioC2;
    using Busy = modm::platform::GpioC3;
    using Dio1 = modm::platform::GpioA0;
    using Dio2 = modm::platform::GpioA1;
    using Dio3 = modm::platform::GpioA2;

    using Nss = modm::platform::GpioC1;
    using Transport = modm::Sx128xTransportSpi<SpiMaster, Nss>;

public:
    TxThread() : timer(std::chrono::milliseconds(500)) {}

    inline bool
    run()
    {
        PT_BEGIN();

        Nss::setOutput(modm::Gpio::High);
        Reset::setOutput(modm::Gpio::Low);
        Busy::setInput(modm::platform::Gpio::InputType::PullDown);

        Dio1::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Dio1>(Exti::Trigger::RisingEdge, [](uint8_t) { tx::dio1 = true; });

        Dio2::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Dio2>(Exti::Trigger::RisingEdge, [](uint8_t) { tx::dio2 = true; });

        Dio3::setInput(modm::platform::Gpio::InputType::PullDown);
        Exti::connect<Dio3>(Exti::Trigger::RisingEdge, [](uint8_t) { tx::dio3 = true; });


        PT_CALL(radio.reset());
        PT_CALL(radio.setStandby());

        // Initialize the sx128x
        PT_CALL(radio.setPacketType(PacketType::LoRa));
        PT_CALL(radio.setRfFrequency(2457_MHz / radio.frequencyLsb));
        PT_CALL(radio.setRegulatorMode(RegulatorMode::Ldo));
        PT_CALL(radio.setBufferBaseAddress(0, 0));
        PT_CALL(radio.setModulationParams(modulationParams));
        PT_CALL(radio.writeRegister(Register::SfAdditionalConfiguration, 0x32));
        PT_CALL(radio.writeRegister(Register::FrequencyErrorCorrection, 0x01));
        PT_CALL(radio.setPacketParams(packetParams));
        PT_CALL(radio.setDioIrqParams(Irq::TxDone | Irq::RxTxTimeout, Irq::TxDone, Irq::RxTxTimeout));

        MODM_LOG_DEBUG << "Sx128x initialization complete!" << modm::endl;

        while (true)
        {
            if (tx::dio1.exchange(false))
            {
                PT_CALL(radio.getIrqStatus(&irqStatus));
                if (irqStatus.any(Irq::TxDone))
                {
                    PT_CALL(radio.clearIrqStatus(Irq::TxDone));
                    irqStatus.reset(Irq::TxDone);

                    MODM_LOG_DEBUG << "Message sent" << modm::endl;
                    MODM_LOG_DEBUG << "Counter: " << counter << modm::endl;
                    counter++;
                }
            }

            if (tx::dio2.exchange(false))
            {
                PT_CALL(radio.getIrqStatus(&irqStatus));
                if (irqStatus.any(Irq::RxTxTimeout))
                {
                    PT_CALL(radio.clearIrqStatus(Irq::RxTxTimeout));
                    irqStatus.reset(Irq::RxTxTimeout);
                    MODM_LOG_DEBUG << "Received a timeout" << modm::endl;
                }
            }

            if (timer.execute())
            {
                PT_CALL(radio.writeBuffer(0, std::span<const uint8_t>{(uint8_t *) &counter, sizeof(counter)}));
                PT_CALL(radio.setTx(PeriodBase::ms1, 100));
            }
            PT_YIELD();
        }

        PT_END();
    }

private:
    Irq_t irqStatus;
    modm::Sx128x< Transport, Reset, Busy > radio;

    uint32_t counter = 0;
    modm::PeriodicTimer timer;
};

int
main()
{
    Board::initialize();

    SpiMaster::connect<Mosi::Mosi, Mosi::Mosi, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 21.5_MHz>();

    MODM_LOG_INFO       << "==========SX128x Test==========" << modm::endl;
    MODM_LOG_DEBUG      << "Debug logging here" << modm::endl;
    MODM_LOG_INFO       << "Info logging here" << modm::endl;
    MODM_LOG_WARNING    << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR      << "Error logging here" << modm::endl;
    MODM_LOG_INFO       << "===============================" << modm::endl;

    while (true)
    {
        rxThread.run();
    }

    return 0;
}