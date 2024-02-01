/*
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2024, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <array>

int main()
{
    Board::initialize();

    MODM_LOG_INFO << "STM32G4 ADC regular conversion sequence example" << modm::endl;

    // max. ADC clock for STM32G474: 60 MHz
    // 170 MHz AHB clock / 4 = 42.5 MHz
    Adc1::initialize(
        Adc1::ClockMode::SynchronousPrescaler4,
        Adc1::ClockSource::SystemClock,
        Adc1::Prescaler::Disabled,
        Adc1::CalibrationMode::SingleEndedInputsMode,
        true);

    Adc1::connect<Board::A0::In1, Board::A1::In2, Board::A3::In15, Board::A4::In7>();

    constexpr auto sampleTime = Adc1::SampleTime::Cycles641;
    constexpr std::array<Adc1::SequenceChannel, 5> sequence{{
        // Perform dummy conversion (ADC errata 2.7.7 and 2.7.8)
        {Adc1::getPinChannel<Board::A0::In1>(), sampleTime},
        // Read A0, A1, A3, A4
        {Adc1::getPinChannel<Board::A0::In1>(), sampleTime},
        {Adc1::getPinChannel<Board::A1::In2>(), sampleTime},
        {Adc1::getPinChannel<Board::A3::In15>(), sampleTime},
        {Adc1::getPinChannel<Board::A4::In7>(), sampleTime}
    }};
    Adc1::setChannelSequence(sequence);
    Adc1::setDmaMode(Adc1::DmaMode::OneShot);

    Dma1::enable();
    using DmaChannel = Dma1::Channel1;
    DmaChannel::configure(Dma1::DataTransferDirection::PeripheralToMemory, Dma1::MemoryDataSize::Bit16,
                          Dma1::PeripheralDataSize::Bit16, Dma1::MemoryIncrementMode::Increment,
                          Dma1::PeripheralIncrementMode::Fixed);
    DmaChannel::setPeripheralAddress(reinterpret_cast<uintptr_t>(Adc1::dataRegister()));
    constexpr auto request = DmaChannel::RequestMapping<Peripheral::Adc1>::Request;
    DmaChannel::setPeripheralRequest<request>();

    std::array<uint16_t, 5> samples{};

    while (true)
    {
        DmaChannel::setMemoryAddress(reinterpret_cast<uintptr_t>(samples.data()));
        DmaChannel::setDataLength(samples.size());
        DmaChannel::start();
        Adc1::startConversion();

        while (!(DmaChannel::getInterruptFlags() & Dma1::InterruptFlags::TransferComplete));
        DmaChannel::stop();

        const double factor = 3.3 / 4095;
        MODM_LOG_INFO.printf("A0: %1.3f V\n", samples[1] * factor);
        MODM_LOG_INFO.printf("A1: %1.3f V\n", samples[2] * factor);
        MODM_LOG_INFO.printf("A3: %1.3f V\n", samples[3] * factor);
        MODM_LOG_INFO.printf("A4: %1.3f V\n", samples[4] * factor);
        MODM_LOG_INFO << '\n';
        modm::delay(500ms);
    }

    return 0;
}
