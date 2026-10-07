/*
 * Copyright (c) 2023, Zühlke Engineering (Austria) GmbH
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
	initialize(uintptr_t destination_ptr, size_t length,
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
		Dma::AdcChannel::setDataLength(length);
		Dma::AdcChannel::setMemoryAddress(destination_ptr);

		setTransferErrorCallback(transferErrorCallback);
		setHalfCompletedConversionCallback(halfCompletedCallback);
		setCompletedConversionCallback(completedCallback);

		Dma::AdcChannel::template setPeripheralRequest<Dma::AdcRequest>();
		Adc::disableDmaMode();
		// Start Conversion if adc is enabled
		if (Adc::getAdcEnabled())
		{
			Adc::enableDmaMode();
			Adc::enableDmaRequests();
			Dma::AdcChannel::start();
		}
	}

	static void
	setTransferErrorCallback(modm::platform::DmaBase::IrqHandler transferErrorCallback)
	{
		if (transferErrorCallback == nullptr) { return; }
		Dma::AdcChannel::enableInterruptVector();
		Dma::AdcChannel::enableInterrupt(modm::platform::DmaBase::InterruptEnable::TransferError |
										 modm::platform::DmaBase::InterruptEnable::DirectModeError);
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
