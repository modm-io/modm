/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2015, 2017-2018, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP2515_HPP
	#error	"Don't include this file directly, use 'mcp2515.hpp' instead!"
#endif
#include "mcp2515_bit_timings.hpp"
#include "mcp2515_definitions.hpp"
#include <modm/architecture/interface/assert.hpp>

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DISABLED


// ----------------------------------------------------------------------------

static uint8_t statusBuffer_;
static uint8_t addressBuffer_;
static uint8_t dataBuffer_;
static modm::can::Message messageBuffer_;
static uint8_t i_;
static uint8_t a_;
static uint8_t b_;
static bool temp_;
static bool temp2_;
static bool hasSend_;
static modm::ShortTimeout delay_;

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
modm::Mcp2515<SPI, CS, INT>::initializeWithPrescaler(
	uint8_t prescaler /* 2 .. 128 */,
	uint8_t sjw       /* in 1TQ .. 3TQ */,
	uint8_t prop      /* in 1TQ .. 8TQ */,
	uint8_t ps1       /* in 1TQ .. 8TQ */,
	uint8_t ps2       /* in 2TQ .. 8TQ */)
{
	// Build CNF1 .. 3 from parameters
	// Configuration is stored at increasing addresses in MCP2515,
	// so prepare CNF3, CNF2 and CNF1 in that order.
	uint8_t cnf[3] = {0};
	static constexpr uint8_t CNF1_idx = 2;
	static constexpr uint8_t CNF2_idx = 1;
	static constexpr uint8_t CNF3_idx = 0;

	MODM_LOG_DEBUG.printf("SJW: %d\nProp: %d\nPS1: %d\nPS2: %d\nprescaler: %d\n",
		sjw, prop, ps1, ps2, prescaler);

	cnf[CNF1_idx] = ((sjw - 1) << 6) | ((prescaler / 2 - 1) & 0x3f);

	cnf[CNF2_idx] = (1 << 7) | ( (ps1 - 1) << 3) | ( (prop - 1) << 0);

	cnf[CNF3_idx] = (ps2 - 1);

	MODM_LOG_DEBUG.printf("CNF1 %02x, CNF2 %02x, CNF3 %02x\n", cnf[CNF1_idx], cnf[CNF2_idx], cnf[CNF3_idx]);

	using namespace mcp2515;

	while(!this->acquireMaster()){};
	// software reset for the mcp2515, after this the chip is back in the
	// configuration mode
	chipSelect.reset();
	spi.transferBlocking(RESET);
	modm::delay_ms(1);
	chipSelect.set();

	// wait a bit to give the MCP2515 some time to restart
	modm::delay_ms(30);

	chipSelect.reset();
	spi.transferBlocking(WRITE);
	spi.transferBlocking(CNF3);

	// load CNF1..3
	spi.transferBlocking(cnf, nullptr, 3);

	// enable interrupts
	spi.transferBlocking(RX1IE | RX0IE);
	chipSelect.set();
	if(this->releaseMaster()){
		chipSelect.set();
	}
	// set TXnRTS pins as inwrites
	writeRegister(TXRTSCTRL, 0);

	// disable RXnBF pins (high impedance state)
	writeRegister(BFPCTRL, 0);

	// check if we could read back some of the values
	uint8_t readback = readRegister(CNF2);

	if (not modm_assert_continue_fail_debug(readback == cnf[CNF2_idx], "mcp2515.init",
				"Cannot read the CNF2 register of the MCP2515!", readback))
		return false;

	// reset device to normal mode and disable the clkout pin and
	// wait until the new mode is active
	writeRegister(CANCTRL, 0);
	while ((readRegister(CANSTAT) &
			(OPMOD2 | OPMOD1 | OPMOD0)) != 0) {
	}

	return true;
}


template <typename SPI, typename CS, typename INT>
template <modm::frequency_t externalClockFrequency, modm::bitrate_t bitrate, modm::percent_t tolerance>
bool
modm::Mcp2515<SPI, CS, INT>::initialize()
{
	using Timings = modm::CanBitTimingMcp2515<externalClockFrequency, bitrate>;

	return initializeWithPrescaler(
		Timings::getPrescaler(),
		Timings::getSJW(),
		Timings::getProp(),
		Timings::getPS1(),
		Timings::getPS2());
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
void
modm::Mcp2515<SPI, CS, INT>::setFilter(accessor::Flash<uint8_t> filter)
{
	using namespace mcp2515;

	// change to configuration mode
	bitModify(CANCTRL, 0xe0, REQOP2);
	while ((readRegister(CANSTAT) & 0xe0) != REQOP2);

	writeRegister(RXB0CTRL, BUKT);
	writeRegister(RXB1CTRL, 0);

	while(!this->acquireMaster()){};
	uint8_t i, j;
	for (i = 0; i < 0x30; i += 0x10)
	{
		chipSelect.reset();
		spi.transferBlocking(WRITE);
		spi.transferBlocking(i);

		for (j = 0; j < 12; j++)
		{
			if (i == 0x20 && j >= 0x08)
				break;

			spi.transferBlocking(*filter++);
		}
		chipSelect.set();
	}
	if(this->releaseMaster()){
		chipSelect.set();
	}
	bitModify(CANCTRL, 0xe0, 0);
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
void
modm::Mcp2515<SPI, CS, INT>::setMode(Can::Mode mode)
{
	using namespace mcp2515;

	uint8_t reg = 0;
	if (mode == Can::Mode::ListenOnly) {
		reg = REQOP1 | REQOP0;
	}
	else if (mode == Can::Mode::LoopBack) {
		reg = REQOP1;
	}

	// set the new mode
	bitModify(CANCTRL, REQOP2 | REQOP1 | REQOP0, reg);

	while ((readRegister(CANSTAT) &	(OPMOD2 | OPMOD1 | OPMOD0)) != reg) {
		// wait for the new mode to become active
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
modm::Mcp2515<SPI, CS, INT>::isMessageAvailable()
{
	return rxQueue.isNotEmpty();
}

template <typename SPI, typename CS, typename INT>
bool
modm::Mcp2515<SPI, CS, INT>::getMessage(can::Message& message, uint8_t* /*filter_id*/)
{
	if (rxQueue.isEmpty())
	{
		// no message in the receive buffer
		return false;
	}
	else {
        auto& rxMessage = rxQueue.get();
		memcpy(&message, &rxMessage, sizeof(message));
		rxQueue.pop();
		return true;
	}
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
bool
modm::Mcp2515<SPI, CS, INT>::isReadyToSend()
{
	return txQueue.isNotFull();
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
bool
modm::Mcp2515<SPI, CS, INT>::sendMessage(const can::Message& message)
{
	if (not modm_assert_continue_ignore(txQueue.push(message), "mcp2515.can.tx",
			"CAN transmit software buffer overflowed!", 1)) {
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
modm::ResumableResult<bool>
modm::Mcp2515<SPI, CS, INT>::mcp2515ReadMessage(can::Message& message, uint8_t status)
{
	using namespace mcp2515;

	RF_BEGIN();

	temp_ = true;
	// dont read status again if we have been provided one
	if(status == 0xff){
		status = RF_CALL(readStatus(READ_STATUS));
	}

	if (status & FLAG_RXB0_FULL) {
		addressBuffer_ = READ_RX;			// message in buffer 0
	}
	else if (status & FLAG_RXB1_FULL) {
		addressBuffer_ = READ_RX | 0x04;	// message in buffer 1 (RXB1SIDH)
	}
	else {
		temp_ = false;						// Error: no message available
	}

	if(temp_)
	{
		RF_WAIT_UNTIL(this->acquireMaster());
		chipSelect.reset();
		RF_CALL(spi.transfer(addressBuffer_));

		message.flags.extended = RF_CALL(readIdentifier(message.identifier));
		if (status & FLAG_RTR) {
			message.flags.rtr = true;
		}
		else {
			message.flags.rtr = false;
		}
		message.length = RF_CALL(spi.transfer(0xff)) & 0x0f;

		for (i_ = 0; i_ < message.length; ++i_) {
			message.data[i_] = RF_CALL(spi.transfer(0xff));
		}
		if(this->releaseMaster()){
			chipSelect.set();
		}
	}
	// RX0IF or RX1IF respectivly were already cleared automatically by rising CS.
	// See section 12.4 in datasheet.
	RF_END_RETURN(temp_);
}

template <typename SPI, typename CS, typename INT>
modm::ResumableResult<bool>
modm::Mcp2515<SPI, CS, INT>::update(){
	/// todo
	/// this should be a timer interrupt 
	using namespace mcp2515;

	RF_BEGIN();

	// read status flag of the device
	statusBuffer_ = RF_CALL(readStatus(READ_STATUS));

	// check if the device has received a message(pin = LOW)
	// if yes: read it and put it into the rxQueue
	if(!interruptPin.read()){
		if(RF_CALL(mcp2515ReadMessage(messageBuffer_, statusBuffer_)))
		{
			if(not modm_assert_continue_ignore(rxQueue.push(messageBuffer_), "mcp2515.can.tx",
			"CAN transmit software buffer overflowed!", 1)){
				/// ignore
			}
		}
	}

	/// check if device accepts messages and start emptying the transmit queue if not empty
	if (txQueue.isNotEmpty())
	{
		if(mcp2515IsReadyToSend(statusBuffer_)){
			hasSend_ = RF_CALL(mcp2515SendMessage(txQueue.get(), statusBuffer_));
			txQueue.pop();
		}
	}
	RF_END();
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
modm::ResumableResult<bool>
modm::Mcp2515<SPI, CS, INT>::mcp2515IsReadyToSend()
{
	using namespace mcp2515;

	RF_BEGIN();

	temp_ = true;
	statusBuffer_ = RF_CALL(readStatus(READ_STATUS));
	if (( statusBuffer_& (TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ)) ==
			(TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ))
	{
		// all buffers currently in use
		temp_ = false;
	}

	RF_END_RETURN(temp_);
}
template <typename SPI, typename CS, typename INT>
bool
modm::Mcp2515<SPI, CS, INT>::mcp2515IsReadyToSend(uint8_t status)
{
	using namespace mcp2515;
	bool ready = true;
	if ((status & (TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ)) ==
			(TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ))
	{
		// all buffers currently in use
		ready = false;
	}

	return ready;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
modm::ResumableResult<bool>
modm::Mcp2515<SPI, CS, INT>::mcp2515SendMessage(const can::Message& message, uint8_t status)
{
	using namespace modm::mcp2515;
	RF_BEGIN();

	// dont read status again if we have been provided one
	if(status == 0xff){
		status = RF_CALL(readStatus(READ_STATUS));
	}

	if ((status & TXB0CNTRL_TXREQ) == 0)
	{
		addressBuffer_ = 0x00;  // TXB0SIDH
	} else if ((status & TXB1CNTRL_TXREQ) == 0)
	{
		addressBuffer_ = 0x02;  // TXB1SIDH
	} else if ((status & TXB2CNTRL_TXREQ) == 0)
	{
		addressBuffer_ = 0x04;  // TXB2SIDH
	} else
	{
		// all buffer are in use => could not send the message
	}

	if (addressBuffer_ == 0x00 || addressBuffer_ == 0x02 || addressBuffer_ == 0x04)
	{
		RF_WAIT_UNTIL(this->acquireMaster());
		chipSelect.reset();
		RF_CALL(spi.transfer(WRITE_TX | addressBuffer_));
		RF_CALL(writeIdentifier(message.identifier, message.flags.extended));

		// if the message is a rtr-frame, is has a length but no attached data
		if (message.flags.rtr)
		{
			RF_CALL(spi.transfer(MCP2515_RTR | message.length));
		} else
		{
			RF_CALL(spi.transfer(message.length));

			for (i_ = 0; i_ < message.length; ++i_) {
				RF_CALL(spi.transfer(message.data[i_]));
			}
		}
		delay_.restart(1ms);
		chipSelect.set();
		RF_WAIT_UNTIL(delay_.isExpired());

		// send message via RTS command
		chipSelect.reset();
		addressBuffer_ = (addressBuffer_ == 0) ? 1 : addressBuffer_;  // 0 2 4 => 1 2 4
		RF_CALL(spi.transfer(RTS | addressBuffer_));
		if(this->releaseMaster()){
			chipSelect.set();
		}
	}

	RF_END_RETURN(static_cast<bool>(addressBuffer_));
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
modm::Mcp2515<SPI, CS, INT>::writeRegister(uint8_t address, uint8_t data)
{
	while(!this->acquireMaster()){};
	chipSelect.reset();
	spi.transferBlocking(WRITE);
	spi.transferBlocking(address);
	spi.transferBlocking(data);
	if(this->releaseMaster()){
		chipSelect.set();
	}
}

template <typename SPI, typename CS, typename INT>
uint8_t
modm::Mcp2515<SPI, CS, INT>::readRegister(uint8_t address)
{
	while(!this->acquireMaster()){};
	chipSelect.reset();
	spi.transferBlocking(READ);
	spi.transferBlocking(address);
	uint8_t data = spi.transferBlocking(0xff);
	if(this->releaseMaster()){
		chipSelect.set();
	}
	return data;
}

template <typename SPI, typename CS, typename INT>
void
modm::Mcp2515<SPI, CS, INT>::bitModify(uint8_t address, uint8_t mask, uint8_t data)
{
	while(!this->acquireMaster()){};
	chipSelect.reset();
	spi.transferBlocking(BIT_MODIFY);
	spi.transferBlocking(address);
	spi.transferBlocking(mask);
	spi.transferBlocking(data);
	if(this->releaseMaster()){
		chipSelect.set();
	}
}

template <typename SPI, typename CS, typename INT>
modm::ResumableResult<uint8_t>
modm::Mcp2515<SPI, CS, INT>::readStatus(uint8_t type)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	chipSelect.reset();
	dataBuffer_ = RF_CALL(spi.transfer(type));
	RF_CALL(spi.transfer(0xff));
	if(this->releaseMaster()) {
		chipSelect.set();
	}

	RF_END_RETURN(dataBuffer_);
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
modm::ResumableResult<void>
modm::Mcp2515<SPI, CS, INT>::writeIdentifier(const uint32_t& identifier,
											 bool isExtendedFrame)
{
	using namespace mcp2515;
	const uint32_t *ptr = &identifier;

	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	if (isExtendedFrame)
	{
		RF_CALL(spi.transfer(*((uint16_t *)ptr + 1) >> 5));

		// calculate the next values
		a_ = (*((uint8_t *)ptr + 2) << 3) & 0xe0;
		a_ |= MCP2515_IDE;
		a_ |= (*((uint8_t *)ptr + 2)) & 0x03;
		RF_CALL(spi.transfer(a_));
		RF_CALL(spi.transfer(*((uint8_t *)ptr + 1)));
		RF_CALL(spi.transfer(*((uint8_t *)ptr)));
	} else
	{
		RF_CALL(spi.transfer(*((uint16_t *)ptr) >> 3));
		RF_CALL(spi.transfer(*((uint8_t *)ptr) << 5));
		RF_CALL(spi.transfer(0));
		RF_CALL(spi.transfer(0));
	}
	if(this->releaseMaster()) {
		chipSelect.set();
	}

	RF_END();
}

template<typename SPI, typename CS, typename INT>
modm::ResumableResult<bool>
modm::Mcp2515<SPI, CS, INT>::readIdentifier(uint32_t &identifier)
{
	using namespace mcp2515;
	const uint32_t *ptr = &identifier;

	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	a_ = RF_CALL(spi.transfer(0xff));
	b_ = RF_CALL(spi.transfer(0xff));

	temp2_ = false;

	if (b_ & MCP2515_IDE)
	{
		*((uint16_t *)ptr + 1) = (uint16_t)a_ << 5;
		*((uint8_t *)ptr + 1) = RF_CALL(spi.transfer(0xff));

		*((uint8_t *)ptr + 2) |= (b_ >> 3) & 0x1C;
		*((uint8_t *)ptr + 2) |= b_ & 0x03;

		*((uint8_t *)ptr) = RF_CALL(spi.transfer(0xff));

		temp2_ = true;
	} else
	{
		RF_CALL(spi.transfer(0xff));

		*((uint8_t *)ptr + 3) = 0;
		*((uint8_t *)ptr + 2) = 0;

		*((uint16_t *)ptr) = (uint16_t)a_ << 3;

		RF_CALL(spi.transfer(0xff));

		*((uint8_t *)ptr) |= b_ >> 5;
	}
	if(this->releaseMaster()){
		chipSelect.set();
	}

	RF_END_RETURN(temp2_);
}