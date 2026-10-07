/*
 * Copyright (c) 2024, Zühlke Engineering (Austria) GmbH
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EXAMPLE_ADCDMA_HPP
#define EXAMPLE_ADCDMA_HPP

#include <modm/platform.hpp>
#include <span>

template<class Adc, class DmaChannel>
class AdcDma
{
	struct Dma
	{
		using AdcChannel =
			typename DmaChannel::template RequestMapping<modm::platform::Peripheral::Adc1>::Channel;
		static constexpr modm::platform::DmaBase::Request AdcRequest =
			DmaChannel::template RequestMapping<modm::platform::Peripheral::Adc1>::Request;
	};

public:
	/**
	 * \brief initialize both adc and dma
	 * @tparam SystemClock
	 */
	template<class SystemClock>
	static void
	initialize(std::span<uint16_t> buffer,
			   modm::platform::DmaBase::Priority priority = modm::platform::DmaBase::Priority::Low,
			   modm::platform::DmaBase::CircularMode circularMode =
				   modm::platform::DmaBase::CircularMode::Enabled,
			   modm::platform::DmaBase::IrqHandler transferErrorCallback = nullptr,
			   modm::platform::DmaBase::IrqHandler halfCompletedCallback = nullptr,
			   modm::platform::DmaBase::IrqHandler completedCallback = nullptr)
	{
		Dma::AdcChannel::configure(
			modm::platform::DmaBase::DataTransferDirection::PeripheralToMemory,
			modm::platform::DmaBase::MemoryDataSize::HalfWord,
			modm::platform::DmaBase::PeripheralDataSize::HalfWord,
			modm::platform::DmaBase::MemoryIncrementMode::Increment,
			modm::platform::DmaBase::PeripheralIncrementMode::Fixed, priority, circularMode);
		Dma::AdcChannel::setPeripheralAddress(Adc::getDataRegisterAddress());
		Dma::AdcChannel::setDataLength(buffer.size());
		Dma::AdcChannel::setMemoryAddress(reinterpret_cast<uintptr_t>(buffer.data()));

		setTransferErrorCallback(transferErrorCallback);
		setHalfCompletedConversionCallback(halfCompletedCallback);
		setCompletedConversionCallback(completedCallback);

		Dma::AdcChannel::template setPeripheralRequest<Dma::AdcRequest>();
		Adc::setDmaMode(Adc::DmaMode::Disabled);
	}

	static void
	startDma()
	{
		Adc::setDmaMode(Adc::DmaMode::Circular);
		DmaChannel::start();
	}

	static void
	setTransferErrorCallback(modm::platform::DmaBase::IrqHandler transferErrorCallback)
	{
		if (transferErrorCallback == nullptr) { return; }
		Dma::AdcChannel::enableInterruptVector();
		Dma::AdcChannel::enableInterrupt(modm::platform::DmaBase::InterruptEnable::TransferError);
		Dma::AdcChannel::setTransferErrorIrqHandler(transferErrorCallback);
	}

	static void
	setHalfCompletedConversionCallback(modm::platform::DmaBase::IrqHandler halfCompletedCallback)
	{
		if (halfCompletedCallback == nullptr) { return; }
		Dma::AdcChannel::enableInterruptVector();
		Dma::AdcChannel::enableInterrupt(modm::platform::DmaBase::InterruptEnable::HalfTransfer);
		Dma::AdcChannel::setHalfTransferCompleteIrqHandler(halfCompletedCallback);
	}

	static void
	setCompletedConversionCallback(modm::platform::DmaBase::IrqHandler completedCallback)
	{
		if (completedCallback == nullptr) { return; }
		Dma::AdcChannel::enableInterruptVector();
		Dma::AdcChannel::enableInterrupt(
			modm::platform::DmaBase::InterruptEnable::TransferComplete);
		Dma::AdcChannel::setTransferCompleteIrqHandler(completedCallback);
	}
};

#endif  // EXAMPLE_ADCDMA_HPP
