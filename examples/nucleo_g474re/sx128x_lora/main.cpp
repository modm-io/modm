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

class RxThread : public modm::sx128x, public modm::Fiber<>
{
	using Reset = GpioB3;
	using Busy = GpioB4;
	using Dio1 = GpioB5;
	using Dio2 = GpioB6;
	using Dio3 = GpioB7;

	using Nss = GpioD2;
	using Transport = modm::Sx128xTransportSpi<SpiMaster, Nss>;

public:
	RxThread() : Fiber([this]{ run(); }) {}

private:
	bool
	run()
	{
		Nss::setOutput(modm::Gpio::High);
		Reset::setOutput(modm::Gpio::Low);
		Busy::setInput(modm::platform::Gpio::InputType::PullDown);

		Dio1::setInput(modm::platform::Gpio::InputType::PullDown);
		Exti::connect<Dio1>(Exti::Trigger::RisingEdge, [](uint8_t) { rx::dio1 = true; });

		Dio2::setInput(modm::platform::Gpio::InputType::PullDown);
		Exti::connect<Dio2>(Exti::Trigger::RisingEdge, [](uint8_t) { rx::dio2 = true; });

		Dio3::setInput(modm::platform::Gpio::InputType::PullDown);
		Exti::connect<Dio3>(Exti::Trigger::RisingEdge, [](uint8_t) { rx::dio3 = true; });

		radio.reset();
		radio.setStandby();

		// Initialize the sx128x
		radio.setPacketType(PacketType::LoRa);
		radio.setRfFrequency(2457_MHz / radio.frequencyLsb);
		radio.setRegulatorMode(RegulatorMode::Ldo);
		radio.setBufferBaseAddress(0, 0);
		radio.setModulationParams(modulationParams);
		radio.writeRegister(Register::SfAdditionalConfiguration, 0x32);
		radio.writeRegister(Register::FrequencyErrorCorrection, 0x01);
		radio.setPacketParams(packetParams);
		radio.setDioIrqParams(Irq::RxDone | Irq::RxTxTimeout, Irq::RxDone, Irq::RxTxTimeout);
		radio.setRx(sx128x::PeriodBase::ms1, 1000);

		MODM_LOG_DEBUG << "Sx128x initialization complete!" << modm::endl;

		while (true)
		{
			if (rx::dio1.exchange(false))
			{
				radio.getIrqStatus(&irqStatus);
				if (irqStatus.any(Irq::RxDone))
				{
					radio.clearIrqStatus(Irq::RxDone | Irq::RxTxTimeout);
					radio.setRx(sx128x::PeriodBase::ms1, 1000);

					PacketStatus packetStatus;
					RxBufferStatus rxBufferStatus;
					radio.getRxBufferStatus(&rxBufferStatus);
					radio.getPacketStatus(&packetStatus);
					radio.readBuffer(rxBufferStatus.rxStartBufferPointer, std::span{buffer, rxBufferStatus.rxPayloadLength});

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
				radio.getIrqStatus(&irqStatus);
				if (irqStatus.any(Irq::RxTxTimeout))
				{
					// RxTxTimeout Interrupt received!
					// Clear irq and set to rx again.
					radio.clearIrqStatus(Irq::RxTxTimeout);
					radio.setRx(sx128x::PeriodBase::ms1, 1000);
					MODM_LOG_DEBUG << "RxTxTimeout Interrupt!" << modm::endl;
				}
			}
			modm::this_fiber::yield();
		}
	}

	modm::Sx128x< Transport, Reset, Busy > radio;
	static constexpr size_t bufferSize = 256;
	uint8_t buffer[bufferSize];
	Irq_t irqStatus;
} rxFiber;

class TxThread : public modm::sx128x, public modm::Fiber<>
{
	using Reset = modm::platform::GpioC2;
	using Busy = modm::platform::GpioC3;
	using Dio1 = modm::platform::GpioA0;
	using Dio2 = modm::platform::GpioA1;
	using Dio3 = modm::platform::GpioA2;

	using Nss = modm::platform::GpioC1;
	using Transport = modm::Sx128xTransportSpi<SpiMaster, Nss>;

public:
	TxThread() : Fiber([this]{ run(); }) {}

private:
	bool
	run()
	{
		Nss::setOutput(modm::Gpio::High);
		Reset::setOutput(modm::Gpio::Low);
		Busy::setInput(modm::platform::Gpio::InputType::PullDown);

		Dio1::setInput(modm::platform::Gpio::InputType::PullDown);
		Exti::connect<Dio1>(Exti::Trigger::RisingEdge, [](uint8_t) { tx::dio1 = true; });

		Dio2::setInput(modm::platform::Gpio::InputType::PullDown);
		Exti::connect<Dio2>(Exti::Trigger::RisingEdge, [](uint8_t) { tx::dio2 = true; });

		Dio3::setInput(modm::platform::Gpio::InputType::PullDown);
		Exti::connect<Dio3>(Exti::Trigger::RisingEdge, [](uint8_t) { tx::dio3 = true; });


		radio.reset();
		radio.setStandby();

		// Initialize the sx128x
		radio.setPacketType(PacketType::LoRa);
		radio.setRfFrequency(2457_MHz / radio.frequencyLsb);
		radio.setRegulatorMode(RegulatorMode::Ldo);
		radio.setBufferBaseAddress(0, 0);
		radio.setModulationParams(modulationParams);
		radio.writeRegister(Register::SfAdditionalConfiguration, 0x32);
		radio.writeRegister(Register::FrequencyErrorCorrection, 0x01);
		radio.setPacketParams(packetParams);
		radio.setDioIrqParams(Irq::TxDone | Irq::RxTxTimeout, Irq::TxDone, Irq::RxTxTimeout);

		MODM_LOG_DEBUG << "Sx128x initialization complete!" << modm::endl;

		modm::PeriodicTimer timer{500ms};
		uint32_t counter{};
		while (true)
		{
			if (tx::dio1.exchange(false))
			{
				radio.getIrqStatus(&irqStatus);
				if (irqStatus.any(Irq::TxDone))
				{
					radio.clearIrqStatus(Irq::TxDone);
					irqStatus.reset(Irq::TxDone);

					MODM_LOG_DEBUG << "Message sent" << modm::endl;
					MODM_LOG_DEBUG << "Counter: " << counter << modm::endl;
					counter++;
				}
			}

			if (tx::dio2.exchange(false))
			{
				radio.getIrqStatus(&irqStatus);
				if (irqStatus.any(Irq::RxTxTimeout))
				{
					radio.clearIrqStatus(Irq::RxTxTimeout);
					irqStatus.reset(Irq::RxTxTimeout);
					MODM_LOG_DEBUG << "Received a timeout" << modm::endl;
				}
			}

			if (timer.execute())
			{
				radio.writeBuffer(0, std::span<const uint8_t>{(uint8_t *) &counter, sizeof(counter)});
				radio.setTx(PeriodBase::ms1, 100);
			}
			modm::this_fiber::yield();
		}
	}

private:
	Irq_t irqStatus;
	modm::Sx128x< Transport, Reset, Busy > radio;
} txFiber;

int
main()
{
	Board::initialize();

	SpiMaster::connect<Mosi::Mosi, Mosi::Mosi, Sck::Sck>();
	SpiMaster::initialize<Board::SystemClock, 21.5_MHz>();

	MODM_LOG_INFO << "==========SX128x Test==========" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
