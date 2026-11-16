/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
 * Randomized stress test of the STM32 bxCAN and FDCAN drivers in internal
 * loopback mode, so that it needs no transceiver. CANRX (PA11, or PB8 on H5, L5
 * and U5) has a pull-up, since bxCAN needs a recessive level to leave the
 * initialization mode, and the bus-off phase needs it for the recovery.
 *
 * Every frame belongs to a stream with its own identifier and sequence number.
 * Length, flags and payload are derived from (stream, sequence), so the
 * receiver checks every frame exactly against this model. The frames of an
 * accepted stream must arrive complete, once, in order and unchanged. Frames of
 * a lossy stream may be lost, but must not be duplicated, reordered or changed.
 * Frames of a rejected stream must never arrive.
 *
 * Every round runs these phases:
 * - order:    one stream, frames sent back-to-back with the same identifier.
 * - mixed:    16 streams with random standard and extended identifiers.
 * - priority: a burst of low priority frames followed by one high priority
 *             frame, measures how many low priority frames are sent before.
 * and one of these, chosen at random:
 * - filter:   standard, extended, mask and 16-bit bxCAN filters, which must
 *             accept exactly the frames of the matching streams.
 * - refilter: filters are rewritten while frames are received, which must not
 *             lose frames on FDCAN. bxCAN deactivates the reception meanwhile.
 * - overload: frames are received without reading them, in FIFO overwrite
 *             and blocking mode.
 * - mode:     setMode() and setAutomaticRetransmission() while sending, which
 *             empty the hardware queues, but must not stop the transmission.
 * - bus-off:  the normal mode without transceiver gives bit errors, so the
 *             controller must enter the bus-off state and recover repeatedly.
 *
 * Lines starting with "R " are results, lines starting with "E " are errors.
 * Any error fails the test.
 */

#include <modm/board.hpp>
#include <modm/platform.hpp>
#include <optional>
#include <cstring>
#include "../integration_test.hpp"

using namespace modm::literals;
using namespace std::chrono_literals;

#if defined(FDCAN1)
#define CAN_IS_FDCAN 1
using Can = modm::platform::Fdcan1;
static constexpr bool HasFd = true;
static constexpr auto LoopbackMode = Can::Mode::TestInternalLoopback;
#else
#if __has_include(<modm/platform/can/can_1.hpp>)
using Can = modm::platform::Can1;
#else
using Can = modm::platform::Can;
#endif
static constexpr bool HasFd = false;
static constexpr auto LoopbackMode = Can::Mode::ListenOnlyLoopBack;
#endif

#ifndef CAN_TEST_SECONDS
#define CAN_TEST_SECONDS 60
#endif

#define LOG(...) do { MODM_LOG_INFO.printf(__VA_ARGS__); MODM_LOG_INFO.flush(); } while(0)

static uint32_t errors{0};
static const char* phaseName{"init"};
#define ERROR(...) do { errors++; if (errors < 40) { LOG("E %s: ", phaseName); LOG(__VA_ARGS__); \
	LOG(" (overruns %lu)\n", hwOverruns); } } while(0)

// The drivers signal received frames lost by a full hardware FIFO with assertions
static volatile uint32_t hwOverruns{0};
static modm::Abandonment
canAssertions(const modm::AssertionInfo &info)
{
	if (std::strncmp(info.name, "can.rx.hw", 9) == 0) { hwOverruns++; return modm::Abandonment::Ignore; }
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(canAssertions);

static uint32_t rng_state{0x2545F491};
static uint32_t
rng()
{
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 17;
	rng_state ^= rng_state << 5;
	return rng_state;
}

static constexpr uint32_t
mix(uint32_t a, uint32_t b)
{
	uint32_t h = a * 0x9E3779B1u ^ (b + 0x7F4A7C15u + (a << 6) + (a >> 2));
	h ^= h >> 15; h *= 0x85EBCA77u; h ^= h >> 13;
	return h;
}

// ----------------------------------------------------------------------------
enum class Expect : uint8_t { Accept, Lossy, Reject };

struct Stream
{
	uint32_t id;
	bool extended;
	uint16_t tx;	// next sequence number to send
	uint16_t rx;	// next sequence number expected
	Expect expect;
};

static constexpr size_t MaxStreams = 18;
static Stream streams[MaxStreams];
static size_t streamCount{0};
static uint32_t txCount{0}, rxCount{0}, refusedCount{0}, lostCount{0};

// Frames of the priority phase in the order they arrive
static int16_t arrivals[64];
static size_t arrivalCount{0};
static bool recordArrivals{false};

static uint8_t
lengthOf(size_t stream, uint16_t seq)
{
	// at least 3 bytes for the stream index and sequence number
	static constexpr uint8_t classic[] = {3, 4, 5, 6, 7, 8};
	static constexpr uint8_t fd[] = {3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
	const uint32_t h = mix(stream, seq);
	if constexpr (HasFd) {
		if (h & 0x100) return fd[h % std::size(fd)];
	}
	return classic[h % std::size(classic)];
}

static bool
isFd(size_t stream, uint16_t seq)
{
	if constexpr (HasFd) return mix(stream, seq) & 0x100;
	return false;
}

static modm::can::Message
frameOf(size_t index)
{
	const Stream& s = streams[index];
	modm::can::Message msg(s.id, lengthOf(index, s.tx));
	msg.setExtended(s.extended);
	if (isFd(index, s.tx)) {
		msg.setFlexibleData();
		msg.flags.brs = mix(index, s.tx) & 0x200;
	}
	msg.data[0] = index;
	msg.data[1] = s.tx;
	msg.data[2] = s.tx >> 8;
	for (uint8_t ii = 3; ii < msg.getLength(); ++ii)
		msg.data[ii] = mix(index ^ (ii << 8), s.tx);
	return msg;
}

static void
check(const modm::can::Message& msg)
{
	rxCount++;
	if (msg.getLength() < 3) {
		ERROR("rx short frame id=%lx len=%u", msg.getIdentifier(), msg.getLength());
		return;
	}
	const size_t index = msg.data[0];
	if (index >= streamCount or streams[index].id != msg.getIdentifier()
		or streams[index].extended != msg.isExtended())
	{
		ERROR("rx unknown frame id=%lx ext=%u stream=%u", msg.getIdentifier(), msg.isExtended(), index);
		return;
	}
	Stream& s = streams[index];
	if (s.expect == Expect::Reject) {
		ERROR("rx stream %u id=%lx ext=%u passed the filters", index, s.id, s.extended);
		return;
	}
	const uint16_t seq = msg.data[1] | (msg.data[2] << 8);
	if (recordArrivals and arrivalCount < std::size(arrivals))
		arrivals[arrivalCount++] = index;
	if (seq != s.rx) {
		const int16_t gap = int16_t(seq - s.rx);
		if (gap < 0 or s.expect == Expect::Accept) {
			ERROR("rx stream %u id=%lx seq=%u expected %u (%s)", index, s.id, seq, s.rx,
				  gap < 0 ? "duplicate or reordered" : "lost");
		}
		else lostCount += gap;
	}
	s.rx = seq + 1;
	const uint8_t length = lengthOf(index, seq);
	if (msg.getLength() != length) {
		ERROR("rx stream %u seq=%u length=%u expected %u", index, seq, msg.getLength(), length);
		return;
	}
	if (msg.isFlexibleData() != isFd(index, seq)) {
		ERROR("rx stream %u seq=%u len=%u fd=%u expected %u", index, seq, length,
			  msg.isFlexibleData(), isFd(index, seq));
	}
	for (uint8_t ii = 3; ii < length; ++ii) {
		if (msg.data[ii] != uint8_t(mix(index ^ (ii << 8), seq))) {
			ERROR("rx stream %u seq=%u data[%u]=%02x corrupted", index, seq, ii, msg.data[ii]);
			break;
		}
	}
}

static bool
pump()
{
	modm::can::Message msg;
	bool received{false};
	while (Can::getMessage(msg)) { check(msg); received = true; }
	return received;
}

static bool
send(size_t index, bool pumping=true)
{
	const modm::can::Message msg = frameOf(index);
	const auto deadline = modm::Clock::now() + 200ms;
	while (true)
	{
		const bool ready = Can::isReadyToSend();
		if (ready and Can::sendMessage(msg)) break;
		// FDCAN in priority mode refuses frames while one with the same ID is pending
		if (ready) refusedCount++;
		if (pumping) pump();
		if (modm::Clock::now() > deadline) {
			ERROR("tx stuck at stream %u seq=%u, bus state %u", index, streams[index].tx,
				  uint8_t(Can::getBusState()));
			return false;
		}
	}
	streams[index].tx++;
	txCount++;
	// a slow CPU may never wait for the transmit queue, so read here too
	if (pumping) pump();
	return true;
}

static constexpr uint32_t MarkerId = 0x1FFF'FFFF;
static void setStream(size_t index, uint32_t id, bool extended, Expect expect);

static bool
drain()
{
	// The marker frame has the lowest priority, so it is sent after all other
	// frames, including those that the filters reject.
	if (streamCount < MaxStreams) {
		setStream(streamCount++, MarkerId, true, Expect::Accept);
		send(streamCount - 1);
	}
	// Wait for all frames of the accepted streams, then until nothing arrives
	// for a while, so that late frames of the other streams are checked too.
	const auto deadline = modm::Clock::now() + 300ms;
	auto quiet = modm::Clock::now();
	bool done{false};
	while (modm::Clock::now() < deadline)
	{
		if (pump()) quiet = modm::Clock::now();
		done = true;
		for (size_t ii = 0; ii < streamCount; ++ii)
			if (streams[ii].expect == Expect::Accept) done &= (streams[ii].rx == streams[ii].tx);
		if (done and modm::Clock::now() - quiet > 10ms) break;
	}
	for (size_t ii = 0; ii < streamCount; ++ii) {
		if (streams[ii].rx != streams[ii].tx) {
			if (streams[ii].expect == Expect::Accept)
				ERROR("lost stream %u id=%lx: received %u of %u", ii, streams[ii].id,
					  streams[ii].rx, streams[ii].tx);
			else if (streams[ii].expect == Expect::Lossy)
				lostCount += uint16_t(streams[ii].tx - streams[ii].rx);
		}
		streams[ii].rx = streams[ii].tx;
	}
	return done;
}

static uint32_t
randomId(bool extended)
{
	// skip the IDs of the priority phase
	return extended ? (0x100 + rng() % 0x1FFFFE00) : (0x10 + rng() % 0x7E0);
}

static bool
isUsed(size_t count, uint32_t id, bool extended)
{
	for (size_t jj = 0; jj < count; ++jj)
		if (streams[jj].id == id and streams[jj].extended == extended) return true;
	return false;
}

static void
newStreams(size_t count, Expect expect=Expect::Accept)
{
	streamCount = count;
	for (size_t ii = 0; ii < count; ++ii)
	{
		Stream& s = streams[ii];
		do {
			s.extended = rng() & 1;
			s.id = randomId(s.extended);
		} while (isUsed(ii, s.id, s.extended));
		s.tx = s.rx = rng();
		s.expect = expect;
	}
}

static void
setStream(size_t index, uint32_t id, bool extended, Expect expect)
{
	streams[index].id = id;
	streams[index].extended = extended;
	streams[index].tx = streams[index].rx = rng();
	streams[index].expect = expect;
}

// ----------------------------------------------------------------------------
// Standard identifiers are received through RX FIFO 1, extended identifiers
// through RX FIFO 0. On bxCAN the filters share the banks, so the extended
// filter uses bank 0 and the standard filter bank 1.
static void
acceptAll()
{
	Can::clearStandardFilters();
	Can::clearExtendedFilters();
	Can::setExtendedFilter(0, Can::FilterConfig::Fifo0,
			modm::can::ExtendedIdentifier(0), modm::can::ExtendedMask(0));
	Can::setStandardFilter(1, Can::FilterConfig::Fifo1,
			modm::can::StandardIdentifier(0), modm::can::StandardMask(0));
}

static void
initialize(bool overwrite, bool fd=true)
{
#ifdef CAN_IS_FDCAN
	// the bit timing calculator needs 15-21 time quanta per bit
	constexpr modm::bitrate_t dataBitrate = (Board::SystemClock::Fdcan1 >= 15'000'000) ? 1_Mbps : 500_kbps;
	if (fd) Can::initialize<Board::SystemClock, 500_kbps, 2_pct, dataBitrate>(9, LoopbackMode, overwrite);
	else Can::initialize<Board::SystemClock, 500_kbps, 2_pct>(9, LoopbackMode, overwrite);
#else
	(void) fd;
	if (not Can::initialize<Board::SystemClock, 500_kbps>(9, LoopbackMode, overwrite))
		ERROR("initialize failed");
	// initialize() resets the interrupt enables
	Can::enableStatusChangeInterrupt(CAN_IER_ERRIE | CAN_IER_BOFIE | CAN_IER_EPVIE | CAN_IER_EWGIE, 10);
#endif
	acceptAll();
}

// ----------------------------------------------------------------------------
static void
phaseOrder()
{
	phaseName = "order";
	newStreams(1);
	streams[0].extended = true;
	for (int ii = 0; ii < 500; ++ii) if (not send(0)) break;
	drain();
}

static void
phaseMixed()
{
	phaseName = "mixed";
	newStreams(16);
	for (int ii = 0; ii < 2000; ++ii) if (not send(rng() % streamCount)) break;
	drain();
}

static size_t worstPriorityPosition{0};
static void
phasePriority()
{
	phaseName = "priority";
	// Only meaningful with a software buffer deeper than the hardware queue
	if constexpr (Can::TxBufferSize < 16) return;
	newStreams(2);
	streams[0] = {0x1FFFFFF0, true, 0, 0, Expect::Accept};	// low priority, same ID for all
	streams[1] = {0x00000008, true, 0, 0, Expect::Accept};	// high priority
	constexpr size_t burst = 24;
	recordArrivals = true;
	arrivalCount = 0;
	// prepare the frames first, so that the burst is queued much faster than
	// the frames are sent
	static modm::can::Message frames[burst + 1];
	for (size_t ii = 0; ii <= burst; ++ii) {
		const size_t index = (ii == burst) ? 1 : 0;
		frames[ii] = frameOf(index);
		streams[index].tx++;
	}
	for (const auto& frame : frames) {
		if (Can::sendMessage(frame)) txCount++;
		else ERROR("tx refused in priority burst");
	}
	drain();
	recordArrivals = false;
	size_t position = burst;
	for (size_t ii = 0; ii < arrivalCount; ++ii)
		if (arrivals[ii] == 1) { position = ii; break; }
	worstPriorityPosition = std::max(worstPriorityPosition, position);
}

// Model of the configured mask filters
struct FilterSpec { bool extended; uint32_t id; uint32_t mask; };
static FilterSpec filters[8];
static size_t filterCount{0};

static Expect
expectOf(uint32_t id, bool extended)
{
	for (size_t ii = 0; ii < filterCount; ++ii)
		if (filters[ii].extended == extended and ((id ^ filters[ii].id) & filters[ii].mask) == 0)
			return Expect::Accept;
	return Expect::Reject;
}

static void
phaseFilter()
{
	phaseName = "filter";
	using namespace modm::can;
	Can::clearStandardFilters();
	Can::clearExtendedFilters();
	const uint32_t extId = randomId(true);
	const uint32_t stdId = randomId(false);
	const uint32_t group = randomId(true) & 0x1FFF'0000;
	filterCount = 0;
	filters[filterCount++] = {true, extId, 0x1FFF'FFFF};
	Can::setExtendedFilter(0, Can::FilterConfig::Fifo0, ExtendedIdentifier(extId), ExtendedMask(0x1FFF'FFFF));
	filters[filterCount++] = {false, stdId, 0x7FF};
	Can::setStandardFilter(1, Can::FilterConfig::Fifo1, StandardIdentifier(stdId), StandardMask(0x7FF));
	filters[filterCount++] = {true, group, 0x1FFF'0000};
	Can::setExtendedFilter(2, Can::FilterConfig::Fifo0, ExtendedIdentifier(group), ExtendedMask(0x1FFF'0000));
	// for the marker frame of drain()
	filters[filterCount++] = {true, MarkerId, 0x1FFF'FFFF};
	Can::setExtendedFilter(5, Can::FilterConfig::Fifo0, ExtendedIdentifier(MarkerId), ExtendedMask(0x1FFF'FFFF));

	uint32_t ids[16][2] = {
		{extId, 1}, {extId ^ 1, 1},
		// the standard filter must not accept extended frames with the same upper 11 bits
		{stdId, 0}, {stdId ^ 1, 0}, {(stdId << 18) | (rng() & 0x3FFFF), 1},
		{group | (rng() & 0xFFFF), 1}, {group | (rng() & 0xFFFF), 1},
		{(group ^ 0x0001'0000) | (rng() & 0xFFFF), 1},
	};
	size_t count = 8;
#ifndef CAN_IS_FDCAN
	// The 16-bit filters of CanFilter compare the upper 14 bits of extended
	// identifiers. Bits 18-21 are set, since they overlapped RTR and IDE.
	const uint32_t shortId = (randomId(true) & 0x1FC0'0000) | 0x003C'0000;
	filters[filterCount++] = {true, shortId, 0x1FFF'8000};
	CanFilter::setFilterShort(3, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifierShort(shortId), CanFilter::ExtendedFilterMaskShort(0x1FFF'8000),
			CanFilter::ExtendedIdentifierShort(shortId), CanFilter::ExtendedFilterMaskShort(0x1FFF'8000));
	// the 32-bit standard filter of CanFilter must not accept extended frames
	const uint32_t stdId2 = randomId(false);
	filters[filterCount++] = {false, stdId2, 0x7FF};
	CanFilter::setFilter(4, CanFilter::FIFO1,
			CanFilter::StandardIdentifier(stdId2), CanFilter::StandardFilterMask(0x7FF));
	ids[count][0] = shortId | (rng() & 0x7FFF); ids[count++][1] = 1;
	ids[count][0] = (shortId ^ 0x8000) | (rng() & 0x7FFF); ids[count++][1] = 1;
	ids[count][0] = stdId2; ids[count++][1] = 0;
	ids[count][0] = (stdId2 << 18) | (rng() & 0x3FFFF); ids[count++][1] = 1;
#endif
	streamCount = 0;
	uint32_t accepted{0};
	for (size_t ii = 0; ii < count; ++ii) {
		const bool extended = ids[ii][1];
		if (isUsed(streamCount, ids[ii][0], extended)) continue;
		const Expect expect = expectOf(ids[ii][0], extended);
		accepted += (expect == Expect::Accept);
		setStream(streamCount++, ids[ii][0], extended, expect);
	}
	if (accepted < 3) ERROR("filter model accepts only %lu streams", accepted);
	for (int ii = 0; ii < 300; ++ii) if (not send(rng() % streamCount)) break;
	drain();
	acceptAll();
}

static void
phaseRefilter()
{
	phaseName = "refilter";
	// bxCAN deactivates the reception while a filter is changed
	newStreams(8, HasFd ? Expect::Accept : Expect::Lossy);
	for (int ii = 0; ii < 1000; ++ii)
	{
		if (not send(rng() % streamCount)) break;
		if (rng() % 4) continue;
		// filters behind the accept-all filters, which match first on FDCAN
		if (rng() & 1) {
			Can::setExtendedFilter(2 + rng() % 3, (rng() & 1) ? Can::FilterConfig::Fifo0 : Can::FilterConfig::Fifo1,
					modm::can::ExtendedIdentifier(rng()), modm::can::ExtendedMask(rng()));
		} else {
			Can::setStandardFilter(5 + rng() % 3, (rng() & 1) ? Can::FilterConfig::Fifo0 : Can::FilterConfig::Fifo1,
					modm::can::StandardIdentifier(rng()), modm::can::StandardMask(rng()));
		}
	}
	drain();
	acceptAll();
}

static uint32_t overloadReceived{0};
static void
phaseOverload(bool overwrite)
{
	phaseName = overwrite ? "overwrite" : "blocking";
	initialize(overwrite);
	newStreams(4, Expect::Lossy);
	const uint32_t received = rxCount;
	// many more frames than fit into the software and hardware queues
	const size_t count = Can::RxBufferSize + 80;
	for (size_t ii = 0; ii < count; ++ii) if (not send(rng() % streamCount, false)) break;
	modm::delay(20ms);
	drain();
	if (rxCount - received < std::min<size_t>(Can::RxBufferSize, count))
		ERROR("overload %u received only %lu of %u frames", overwrite, rxCount - received, count);
	overloadReceived += rxCount - received;
	initialize(false);
	// the reception continues normally
	phaseOrder();
}

static uint32_t modeChanges{0};
static void
phaseMode()
{
	phaseName = "mode";
	// The initialization mode empties the hardware queues
	newStreams(8, Expect::Lossy);
	const uint32_t lost = lostCount;
	uint32_t changes{0};
	for (int ii = 0; ii < 1000; ++ii)
	{
		if (not send(rng() % streamCount)) break;
		if (rng() % 32) continue;
		if (rng() & 1) Can::setMode(LoopbackMode);
		else Can::setAutomaticRetransmission(true);
		changes++;
	}
	drain();
	modeChanges += changes;
	// The hardware queues hold only a few frames each
	if (lostCount - lost > changes * (Can::TxHwQueueSize + 64))
		ERROR("mode lost %lu frames in %lu changes", lostCount - lost, changes);
	phaseOrder();
}

static volatile uint32_t statusInterrupts{0};
#ifndef CAN_IS_FDCAN
#ifdef STM32F0
// shares the vector with the other CAN interrupts, which calls this handler
MODM_ISR_DECL(CAN_SCE)
{
	CAN->MSR = CAN_MSR_ERRI | CAN_MSR_WKUI | CAN_MSR_SLAKI;
	statusInterrupts++;
}
#else
MODM_ISR(CAN1_SCE)
{
	CAN1->MSR = CAN_MSR_ERRI | CAN_MSR_WKUI | CAN_MSR_SLAKI;
	statusInterrupts++;
}
#endif
#endif

static uint32_t busOffs{0}, recoveries{0};
static void
phaseBusOff()
{
	phaseName = "busoff";
	// Without transceiver CANRX stays recessive while the controller sends, so
	// every frame has a bit error until the controller enters the bus-off
	// state, from which it recovers after 128 x 11 recessive bits.
	Can::setMode(Can::Mode::Normal);
	newStreams(1, Expect::Lossy);
	if (not send(0)) return;
	bool off{false};
	uint32_t offs{0}, recovered{0};
	const auto end = modm::Clock::now() + 300ms;
	while (modm::Clock::now() < end)
	{
		const bool now = (Can::getBusState() == Can::BusState::Off);
		if (now and not off) offs++;
		if (off and not now) recovered++;
		off = now;
	}
	Can::setMode(LoopbackMode);
	drain();
	busOffs += offs;
	recoveries += recovered;
	if (offs < 3 or recovered < 3)
		ERROR("bus-off %lu times and recovered %lu times in 300ms", offs, recovered);
	phaseOrder();
}

template< class T >
constexpr bool
isTxHwQueuePriority()
{
	if constexpr (requires { T::TxHwQueuePriority; }) return T::TxHwQueuePriority;
	else return false;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LOG("\nCAN loopback stress test: %s, tx buffer %u, rx buffer %u, %u s\n",
		HasFd ? "FDCAN" : "bxCAN", Can::TxBufferSize, Can::RxBufferSize, CAN_TEST_SECONDS);

#if defined(STM32H5) || defined(STM32U5) || defined(STM32L5)
	// PA11 and PA12 need the USB supply domain, which only USB enables
	Can::connect<GpioB8::Rx, GpioB9::Tx>(Gpio::InputType::PullUp);
#else
	Can::connect<GpioA11::Rx, GpioA12::Tx>(Gpio::InputType::PullUp);
#endif
#ifdef CAN_IS_FDCAN
	// FD frames are refused without CAN FD mode
	initialize(false, false);
	{
		newStreams(1);
		modm::can::Message msg(streams[0].id, 12);
		msg.setFlexibleData();
		if (Can::sendMessage(msg)) ERROR("FD frame accepted without CAN FD mode");
	}
	static uint32_t errorCallbacks{0};
	Can::setErrorInterruptCallback([]{ errorCallbacks++; });
#endif
	initialize(false);

	const auto start = modm::Clock::now();
	auto report = start;
	uint32_t rounds{0};
	uint32_t phases[6]{};
	while (modm::Clock::now() - start < std::chrono::seconds(CAN_TEST_SECONDS))
	{
		phaseOrder();
		phaseMixed();
		phasePriority();
		const uint32_t phase = rng() % 6;
		phases[phase]++;
		switch (phase)
		{
			case 0: phaseFilter(); break;
			case 1: phaseRefilter(); break;
			case 2: phaseOverload(true); break;
			case 3: phaseOverload(false); break;
			case 4: phaseMode(); break;
			case 5: phaseBusOff(); break;
		}
		rounds++;
		if (modm::Clock::now() - report >= 10s) {
			report = modm::Clock::now();
			LOG("R round %lu tx %lu rx %lu lost %lu refused %lu errors %lu priority %u\n",
				rounds, txCount, rxCount, lostCount, refusedCount, errors, worstPriorityPosition);
		}
	}
	if (Can::getBusState() != Can::BusState::Connected)
		ERROR("bus state %u", uint8_t(Can::getBusState()));
	// Only the frames already in the hardware queue may be sent first, plus the
	// few that are sent while the burst is queued. In priority mode only one
	// low priority frame is pending, since they all have the same identifier.
	constexpr bool hwPriority = isTxHwQueuePriority<Can>();
	const size_t maxPosition = (hwPriority ? 1 : Can::TxHwQueueSize) + 2;
	if (Can::TxBufferSize >= 16 and worstPriorityPosition > maxPosition)
		ERROR("high priority frame sent after %u low priority frames, expected <= %u",
			  worstPriorityPosition, maxPosition);
#ifdef CAN_IS_FDCAN
	if (phases[5] and not errorCallbacks) ERROR("no error callback in the bus-off phase");
	LOG("R error callbacks %lu\n", errorCallbacks);
#else
	if (phases[5] and not statusInterrupts) ERROR("no status change interrupt in the bus-off phase");
	LOG("R status interrupts %lu\n", statusInterrupts);
#endif

	LOG("R phases filter %lu refilter %lu overwrite %lu blocking %lu mode %lu bus-off %lu\n",
		phases[0], phases[1], phases[2], phases[3], phases[4], phases[5]);
	LOG("R overload received %lu, mode changes %lu, bus-off %lu recovered %lu\n",
		overloadReceived, modeChanges, busOffs, recoveries);
	LOG("R receive overruns %lu\n", hwOverruns);
	LOG("R done rounds %lu tx %lu rx %lu lost %lu refused %lu errors %lu priority %u\n",
		rounds, txCount, rxCount, lostCount, refusedCount, errors, worstPriorityPosition);
	return finishTest(errors == 0);
}
