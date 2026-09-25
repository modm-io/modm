/*
 * Copyright (c) 2026, Luca Feggi
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "eth_test.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <modm/driver/ethernet/lan8720a.hpp>
#include <modm/driver/ethernet/lan8742a.hpp>
#include <modm/platform/eth/eth_ring.hpp>
#include <span>

namespace
{
using namespace modm::platform::stm32::dwmac;

struct AddressEncoder
{
	uint32_t next{0x2000'0000};

	uint32_t
	operator()(const void*)
	{
		const uint32_t address = next;
		next += 0x1000;
		return address;
	}
};

struct CacheSpy
{
	enum class Operation : uint8_t
	{
		Clean,
		Invalidate,
		CleanInvalidate,
		MemoryBarrier,
		SyncBarrier,
	};

	static inline std::array<Operation, 128> operations{};
	static inline std::size_t count{0};
	static inline const volatile uint32_t* watchedStatus{nullptr};
	static inline bool ownAtMemoryBarrier{false};

	static void
	reset(const volatile uint32_t* status = nullptr)
	{
		count = 0;
		watchedStatus = status;
		ownAtMemoryBarrier = false;
	}
	static void
	record(Operation operation)
	{ operations[count++] = operation; }
	static void
	clean(const void*, std::size_t)
	{ record(Operation::Clean); }
	static void
	invalidate(void*, std::size_t)
	{ record(Operation::Invalidate); }
	static void
	cleanInvalidate(void*, std::size_t)
	{ record(Operation::CleanInvalidate); }
	static void
	dataMemoryBarrier()
	{
		if (watchedStatus != nullptr)
			ownAtMemoryBarrier = (*watchedStatus & descriptor::tx::Own) != 0;
		record(Operation::MemoryBarrier);
	}
	static void
	dataSyncBarrier()
	{ record(Operation::SyncBarrier); }
};

struct FakeMdio
{
	struct Write
	{
		uint8_t address;
		uint8_t reg;
		uint16_t value;
	};

	static inline std::array<uint16_t, 32> registers{};
	static inline std::array<Write, 16> writes{};
	static inline std::size_t writeCount{0};
	static inline bool failReads{false};
	static inline bool failWrites{false};

	static void
	reset()
	{
		registers.fill(0);
		writeCount = 0;
		failReads = false;
		failWrites = false;
	}

	static modm::ethernet::MdioStatus
	read(uint8_t, uint8_t reg, uint16_t& value)
	{
		if (failReads) return modm::ethernet::MdioStatus::Timeout;
		value = registers[reg];
		return modm::ethernet::MdioStatus::Ok;
	}

	static modm::ethernet::MdioStatus
	write(uint8_t address, uint8_t reg, uint16_t value)
	{
		if (failWrites) return modm::ethernet::MdioStatus::Timeout;
		registers[reg] = value;
		writes[writeCount++] = {address, reg, value};
		return modm::ethernet::MdioStatus::Ok;
	}
};

template<typename Ring, typename Storage>
void
completeTransmit(Ring& ring, Storage& storage, std::size_t index, bool error = false)
{
	storage.txDescriptors[index].words[3] &= ~descriptor::tx::Own;
	if (error) storage.txDescriptors[index].words[3] |= descriptor::tx::ErrorSummary;
	ring.reclaimTransmit();
}

}  // namespace

void
EthernetTest::testDescriptorEncoding()
{
	static_assert(sizeof(Descriptor) == 32);
	static_assert(alignof(Descriptor) == 32);
	Storage<2, 2> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);

	TEST_ASSERT_EQUALS(static_cast<uint32_t>(storage.txDescriptors[0].words[0]), 0x2000'0000u);
	TEST_ASSERT_EQUALS(static_cast<uint32_t>(storage.txDescriptors[1].words[0]), 0x2000'1000u);
	TEST_ASSERT_EQUALS(static_cast<uint32_t>(storage.rxDescriptors[0].words[0]), 0x2000'2000u);
	TEST_ASSERT_EQUALS(
		static_cast<uint32_t>(storage.rxDescriptors[0].words[3]),
		descriptor::rx::Own | descriptor::rx::InterruptOnCompletion | descriptor::rx::Buffer1Valid);
	TEST_ASSERT_TRUE(ring.initialRxTailDescriptor() == &storage.rxDescriptors[1]);
}

void
EthernetTest::testTransmitOwnershipAndWraparound()
{
	Storage<2, 2> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	const std::array<uint8_t, 60> frame{};

	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
	TEST_ASSERT_EQUALS(static_cast<uint32_t>(storage.txDescriptors[0].words[2]),
					   descriptor::tx::InterruptOnCompletion | 60u);
	TEST_ASSERT_EQUALS(static_cast<uint32_t>(storage.txDescriptors[0].words[3]),
					   descriptor::tx::Own | descriptor::tx::First | descriptor::tx::Last | 60u);
	TEST_ASSERT_TRUE(ring.txTailDescriptor() == &storage.txDescriptors[1]);

	completeTransmit(ring, storage, 0);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
	completeTransmit(ring, storage, 1);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
	TEST_ASSERT_TRUE(ring.txTailDescriptor() == &storage.txDescriptors[1]);
	TEST_ASSERT_EQUALS(ring.diagnostics().transmittedFrames, 2u);
}

void
EthernetTest::testReceiveLeaseAndRecycle()
{
	Storage<2, 2> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	storage.rxBuffers[0].data[0] = 0x12;
	storage.rxBuffers[0].data[1] = 0x34;
	storage.rxDescriptors[0].words[3] = descriptor::rx::First | descriptor::rx::Last | 2u;

	auto frame = ring.receive();
	TEST_ASSERT_TRUE(frame.has_value());
	TEST_ASSERT_EQUALS(frame->size(), 2u);
	TEST_ASSERT_EQUALS(frame->data()[0], 0x12u);
	TEST_ASSERT_FALSE(ring.receive().has_value());

	auto moved = std::move(frame.value());
	TEST_ASSERT_FALSE(static_cast<bool>(frame.value()));
	TEST_ASSERT_EQUALS(frame->size(), 0u);
	moved.release();
	TEST_ASSERT_FALSE(static_cast<bool>(moved));
	TEST_ASSERT_TRUE((storage.rxDescriptors[0].words[3] & descriptor::rx::Own) != 0);
	TEST_ASSERT_TRUE(ring.takeRxTailDescriptor() == &storage.rxDescriptors[0]);
	TEST_ASSERT_TRUE(ring.takeRxTailDescriptor() == nullptr);
}

void
EthernetTest::testReceiveErrorsAndErratum()
{
	Storage<2, 2> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	storage.rxDescriptors[0].words[3] = descriptor::rx::First | descriptor::rx::Last |
										descriptor::rx::ErrorSummary | descriptor::rx::CrcError |
										64u;
	storage.rxDescriptors[1].words[3] = descriptor::rx::Own;
	TEST_ASSERT_FALSE(ring.receive().has_value());
	TEST_ASSERT_EQUALS(ring.diagnostics().droppedFrames, 1u);

	ring.initialize(addresses);
	ring.setRmii10MbitErratumWorkaround(true);
	storage.rxDescriptors[0].words[3] = descriptor::rx::First | descriptor::rx::Last |
										descriptor::rx::ErrorSummary | descriptor::rx::CrcError |
										descriptor::rx::DribbleError | 64u;
	TEST_ASSERT_TRUE(ring.receive().has_value());

	storage.rxDescriptors[1].words[3] =
		descriptor::rx::First | descriptor::rx::Last |
		(static_cast<uint32_t>(modm::ethernet::MaximumFrameSize) + 1u);
	TEST_ASSERT_FALSE(ring.receive().has_value());
}

void
EthernetTest::testFrameLengthAndExhaustion()
{
	Storage<2, 1, 128> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	const std::array<uint8_t, 1> emptyBacking{};
	const std::array<uint8_t, 60> frame{};
	const std::array<uint8_t, 129> tooLarge{};

	TEST_ASSERT_TRUE(ring.tryTransmit(std::span(emptyBacking).first(0), true, true, false) ==
					 modm::ethernet::TransmitResult::InvalidLength);
	TEST_ASSERT_TRUE(ring.tryTransmit(tooLarge, true, true, false) ==
					 modm::ethernet::TransmitResult::FrameTooLarge);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, false, false) ==
					 modm::ethernet::TransmitResult::LinkDown);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, true) ==
					 modm::ethernet::TransmitResult::DmaError);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Busy);
	TEST_ASSERT_EQUALS(ring.diagnostics().transmitBusy, 1u);
}

void
EthernetTest::testStoppedTransmitAndFrameLimit()
{
	Storage<2, 1, 2048> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	const std::array<uint8_t, 60> frame{};
	const std::array<uint8_t, modm::ethernet::MaximumFrameSize> maximumFrame{};
	const std::array<uint8_t, modm::ethernet::MaximumFrameSize + 1> oversizedFrame{};

	TEST_ASSERT_TRUE(ring.tryTransmit(frame, false, true, false) ==
					 modm::ethernet::TransmitResult::NotStarted);
	TEST_ASSERT_TRUE(ring.tryTransmit(oversizedFrame, true, true, false) ==
					 modm::ethernet::TransmitResult::FrameTooLarge);
	TEST_ASSERT_TRUE(storage.txState[0] == DescriptorState::Free);
	TEST_ASSERT_TRUE(ring.tryTransmit(maximumFrame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
}

void
EthernetTest::testCacheOrdering()
{
	Storage<2, 2> storage;
	DescriptorRing<decltype(storage), CacheSpy> ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	CacheSpy::reset(&storage.txDescriptors[0].words[3]);
	const std::array<uint8_t, 60> frame{};
	ring.tryTransmit(frame, true, true, false);

	TEST_ASSERT_EQUALS(CacheSpy::count, 4u);
	TEST_ASSERT_TRUE(CacheSpy::operations[0] == CacheSpy::Operation::Clean);
	TEST_ASSERT_TRUE(CacheSpy::operations[1] == CacheSpy::Operation::MemoryBarrier);
	TEST_ASSERT_TRUE(CacheSpy::operations[2] == CacheSpy::Operation::Clean);
	TEST_ASSERT_TRUE(CacheSpy::operations[3] == CacheSpy::Operation::SyncBarrier);
	TEST_ASSERT_FALSE(CacheSpy::ownAtMemoryBarrier);

	storage.rxDescriptors[0].words[3] = descriptor::rx::First | descriptor::rx::Last | 60u;
	auto received = ring.receive();
	TEST_ASSERT_TRUE(received.has_value());
	CacheSpy::reset(&storage.rxDescriptors[0].words[3]);
	received->release();
	TEST_ASSERT_EQUALS(CacheSpy::count, 4u);
	TEST_ASSERT_TRUE(CacheSpy::operations[0] == CacheSpy::Operation::Invalidate);
	TEST_ASSERT_TRUE(CacheSpy::operations[1] == CacheSpy::Operation::MemoryBarrier);
	TEST_ASSERT_TRUE(CacheSpy::operations[2] == CacheSpy::Operation::Clean);
	TEST_ASSERT_TRUE(CacheSpy::operations[3] == CacheSpy::Operation::SyncBarrier);
	TEST_ASSERT_FALSE(CacheSpy::ownAtMemoryBarrier);
}

void
EthernetTest::testRingRecovery()
{
	Storage<2, 1, 128> storage;
	DescriptorRing ring(storage);
	AddressEncoder addresses;
	ring.initialize(addresses);
	const std::array<uint8_t, 60> frame{};

	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
	completeTransmit(ring, storage, 0, true);
	TEST_ASSERT_EQUALS(ring.diagnostics().transmitErrors, 1u);

	ring.initialize(addresses, false);
	TEST_ASSERT_EQUALS(ring.diagnostics().transmitErrors, 1u);
	TEST_ASSERT_TRUE(storage.txState[0] == DescriptorState::Free);
	TEST_ASSERT_TRUE((storage.rxDescriptors[0].words[3] & descriptor::rx::Own) != 0);
	TEST_ASSERT_TRUE(ring.tryTransmit(frame, true, true, false) ==
					 modm::ethernet::TransmitResult::Ok);
}

void
EthernetTest::testPhyIdentificationAndAddress()
{
	FakeMdio::reset();
	FakeMdio::registers[2] = 0x0007;
	FakeMdio::registers[3] = 0xc0f1;
	modm::Lan8720aPhy<FakeMdio, 3> lan8720;
	modm::ethernet::PhyIdentifier identifier;
	TEST_ASSERT_TRUE(lan8720.readIdentifier(identifier) == modm::ethernet::PhyResult::Ok);
	TEST_ASSERT_TRUE(lan8720.configureAdvertisement() == modm::ethernet::PhyResult::Ok);
	TEST_ASSERT_EQUALS(FakeMdio::writes[0].address, 3u);
	TEST_ASSERT_EQUALS(FakeMdio::writes[0].reg, 4u);
	TEST_ASSERT_EQUALS(FakeMdio::writes[0].value, 0x01e1u);
	TEST_ASSERT_TRUE(lan8720.configureForcedLink(modm::ethernet::LinkSpeed::Mbit100,
												 modm::ethernet::DuplexMode::Full) ==
					 modm::ethernet::PhyResult::Ok);
	TEST_ASSERT_EQUALS(FakeMdio::registers[0], 0x2100u);

	FakeMdio::registers[3] = 0xc131;
	modm::Lan8742aPhy<FakeMdio, 7> lan8742;
	TEST_ASSERT_TRUE(lan8742.readIdentifier(identifier) == modm::ethernet::PhyResult::Ok);
	TEST_ASSERT_TRUE(lan8720.readIdentifier(identifier) == modm::ethernet::PhyResult::WrongDevice);
}

void
EthernetTest::testPhyResetAndNegotiation()
{
	using namespace modm::ethernet;
	FakeMdio::reset();
	modm::Lan8742aPhy<FakeMdio, 1> phy;
	TEST_ASSERT_TRUE(phy.startSoftwareReset(100) == PhyResult::InProgress);
	TEST_ASSERT_TRUE((FakeMdio::registers[0] & 0x8000u) != 0);
	TEST_ASSERT_TRUE(phy.pollSoftwareReset(101) == PhyResult::InProgress);
	FakeMdio::registers[0] = 0;
	TEST_ASSERT_TRUE(phy.pollSoftwareReset(102) == PhyResult::Ok);

	TEST_ASSERT_TRUE(phy.startAutoNegotiation(200) == PhyResult::InProgress);
	TEST_ASSERT_TRUE((FakeMdio::registers[0] & 0x1200u) == 0x1200u);
	FakeMdio::registers[1] = 0x0024;
	FakeMdio::registers[31] = 0x1018;
	TEST_ASSERT_TRUE(phy.pollAutoNegotiation(201) == PhyResult::Ok);
	TEST_ASSERT_TRUE(phy.linkState().up);
	TEST_ASSERT_TRUE(phy.linkState().speed == LinkSpeed::Mbit100);
	TEST_ASSERT_TRUE(phy.linkState().duplex == DuplexMode::Full);
	TEST_ASSERT_TRUE(phy.linkState().autoNegotiated);
}

void
EthernetTest::testPhyTimeoutAndMdioError()
{
	using namespace modm::ethernet;
	FakeMdio::reset();
	modm::Lan8720aPhy<FakeMdio> phy;
	TEST_ASSERT_TRUE(phy.startSoftwareReset(0, 5) == PhyResult::InProgress);
	TEST_ASSERT_TRUE(phy.pollSoftwareReset(5) == PhyResult::Timeout);

	FakeMdio::failReads = true;
	PhyIdentifier identifier;
	TEST_ASSERT_TRUE(phy.readIdentifier(identifier) == PhyResult::MdioError);
}
