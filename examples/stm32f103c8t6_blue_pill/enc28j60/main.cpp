/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart1, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

namespace enc28j60
{
	using Cs = GpioOutputB12; 		// orange wire
	using Sck = GpioOutputB13;		// yellow wire
	using Miso = GpioInputB14;		// blue wire
	using Mosi = GpioOutputB15; 	// green wire

	using Wol = GpioInputA8;		// violet wire
	using Int = GpioInputA9;		// grey wire
	using Clk = GpioInputA10;		// white wire

	using Reset = GpioOutputB8; 	// red wire

	using SpiMaster = SpiMaster2;
}

static constexpr uint8_t BANK_0 = 0x00;
static constexpr uint8_t BANK_1 = 0x20;
static constexpr uint8_t BANK_2 = 0x40;
static constexpr uint8_t BANK_3 = 0x60;

static constexpr uint8_t MAC_MII = 0x80;


enum class Register : uint8_t
{
	ERDPTL = 0x00 | BANK_0,
	ERDPTH = 0x01 | BANK_0,

	MICMD = 0x12 | BANK_2,

	MIREGADR = 0x14 | BANK_2,

	MIRDL = 0x18 | BANK_2,
	MIRDH = 0x19 | BANK_2,


	MISTAT = 0x0a | BANK_3 | MAC_MII,

	ECON1 = 0x1F
};


enum class PhyRegister : uint8_t
{
	PHLCON = 0x14
};

class Enc28j60
{
public:
	static uint16_t
	read_phy(PhyRegister address)
	{
		// 1. Write MIREGADR
		write_control_register(Register::MIREGADR, static_cast<uint8_t>(address));

		// 2. Set MICMD.MIIRD bit
		bfs_eth(0x12, 0x01);

		// 3. Wait 10.24us. Poll MISTAT.BUSY
		modm::delay(11us);
		uint8_t busy;
		do {
			busy = read_control_register(Register::MISTAT) & 0x01;
		} while (busy);

		// 4. Clear MICMD.MIIRD (in bank 2)
		wcr_eth(0x1f, 2); // ECON1
		bfc_eth(0x12, 0x01); // MICMD

		// 5. Read MIRDL and MIRDH
		uint16_t phy_data = (rcr_mac_mii(0x19) << 8) | rcr_mac_mii(0x18);

		MODM_LOG_DEBUG.printf("PhyRegister 0x%02x=0x%04x\n", static_cast<uint8_t>(address), phy_data);
		return phy_data;
	}

	static void
	write_phy(PhyRegister address, uint16_t data)
	{
		// 1. Write MIREGADR
		write_control_register(Register::MIREGADR, static_cast<uint8_t>(address));

		// 2. Write MIWRL
		uint8_t data_low = data & 0x00ff;
		wcr_eth(0x16, data_low); // MIWRL

		// 3. Write MIWRH
		uint8_t data_high = data >> 8;
		wcr_eth(0x17, data_high); // MIWRH

		// 4. Wait 10.24 usec
		modm::delay(11us);

		// 5. Wait for MISTAT.BUSY == 0
		uint8_t busy;
		do {
			busy = read_control_register(Register::MISTAT) & 0x01;
		} while (busy);

		MODM_LOG_DEBUG.printf("PhyRegister 0x%02x:=0x%04x\n", static_cast<uint8_t>(address), data);
	}



	// ------------------------------------------------------------------------
	static uint8_t
	read_control_register(Register reg)
	{
		// Select Bank
		uint8_t bank = (static_cast<uint8_t>(reg) & 0x60) >> 5;
		uint8_t addr = static_cast<uint8_t>(reg) & 0x1f;
		bool mac_mii = static_cast<uint8_t>(reg) & 0x80;
		MODM_LOG_DEBUG.printf(" - read_control_register %02x in bank %1x\n", addr, bank);

		// Select bank
		wcr_eth(0x1f, bank); // ECON1

		uint8_t data;
		if (mac_mii) {
			data = rcr_mac_mii(addr);
		} else {
			data = rcr_eth(addr);
		}

		return data;
	}

	static void
	write_control_register(Register reg, uint8_t data)
	{
		// Select Bank
		uint8_t bank = (static_cast<uint8_t>(reg) & 0x60) >> 5;
		uint8_t addr = static_cast<uint8_t>(reg) & 0x1f;
		MODM_LOG_DEBUG.printf(" - write_control_register %02x in bank %1x\n", addr, bank);

		wcr_eth(0x1f, bank); // ECON1

		// Write data
		wcr_eth(addr, data);
	}


	// ------------------------------------------------------------------------
	static void
	bfs_eth(uint8_t address, uint8_t bitmask)
	{
		uint8_t opcode = 0b100;
		enc28j60::Cs::reset();

		MODM_LOG_DEBUG.printf("   - bfs_eth: address=%02x, bitmask=%02x\n", address, bitmask);

		uint8_t write = (opcode << 5) | (address & 0x1f);
		enc28j60::SpiMaster::transferBlocking(write);
		enc28j60::SpiMaster::transferBlocking(bitmask);

		enc28j60::Cs::set();
	}

	static void
	bfc_eth(uint8_t address, uint8_t bitmask)
	{
		uint8_t opcode = 0b101;
		enc28j60::Cs::reset();

		MODM_LOG_DEBUG.printf("   - bfc_eth: address=%02x, bitmask=%02x\n", address, bitmask);

		uint8_t write = (opcode << 5) | (address & 0x1f);
		enc28j60::SpiMaster::transferBlocking(write);
		enc28j60::SpiMaster::transferBlocking(bitmask);

		enc28j60::Cs::set();
	}

	static void
	wcr_eth(uint8_t address, uint8_t data)
	{
		uint8_t opcode = 0b010;
		enc28j60::Cs::reset();

		MODM_LOG_DEBUG.printf("   - wcr_eth: address=%02x, data=%02x\n", address, data);

		uint8_t write = (opcode << 5) | (address & 0x1f);
		enc28j60::SpiMaster::transferBlocking(write);
		enc28j60::SpiMaster::transferBlocking(data);

		enc28j60::Cs::set();
	}


	static uint8_t
	rcr_eth(uint8_t address) {
		uint8_t opcode = 0b000;
		enc28j60::Cs::reset();

		uint8_t write = (opcode << 5) | (address & 0x1f);
		enc28j60::SpiMaster::transferBlocking(write);
		uint8_t control_register = enc28j60::SpiMaster::transferBlocking(0x00); // dummy

		enc28j60::Cs::set();

		return control_register;
	}

	static uint8_t
	rcr_mac_mii(uint8_t address) {
		uint8_t opcode = 0b000;
		enc28j60::Cs::reset();

		uint8_t write = (opcode << 5) | (address & 0x1f);
		enc28j60::SpiMaster::transferBlocking(write);

		enc28j60::SpiMaster::transferBlocking(0x00); // dummy
		uint8_t control_register = enc28j60::SpiMaster::transferBlocking(0x00); // dummy

		enc28j60::Cs::set();

		return control_register;
	}

	static void
	initialize()
	{

	}
};

/*
 * Blinks the green user LED with 1 Hz.
 * It is on for 90% of the time and off for 10% of the time.
 */

int
main()
{
	Board::initialize();

	LedGreen::set();

	// initialize Uart1 for MODM_LOG_*
	Usart1::connect<GpioOutputB6::Tx>();
	Usart1::initialize<Board::SystemClock, 115200_Bd>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	enc28j60::Reset::setOutput(modm::Gpio::High);
	enc28j60::Cs::setOutput(modm::Gpio::High);

	enc28j60::Wol::setInput(Gpio::InputType::PullUp);
	enc28j60::Int::setInput(Gpio::InputType::PullUp);
	enc28j60::Clk::setInput(Gpio::InputType::PullUp);

	enc28j60::SpiMaster::connect<enc28j60::Sck::Sck, enc28j60::Mosi::Mosi, enc28j60::Miso::Miso>();
	enc28j60::SpiMaster::initialize<SystemClock, 9_MHz>();

	enc28j60::Reset::reset();
	modm::delay(500ms);

	enc28j60::Reset::set();
	modm::delay(500ms);


	// Access PHY for testing communication

	uint16_t phlcon = Enc28j60::read_phy(PhyRegister::PHLCON);
	MODM_LOG_DEBUG.printf("PHLCON=%04x\n", phlcon);

	// blink LED B (orange) fast
	// blink LED A (green)  slow
	//                                         0011 LEDA LEDB 0010
	Enc28j60::write_phy(PhyRegister::PHLCON, 0b0011'1011'1010'0010);

	phlcon = Enc28j60::read_phy(PhyRegister::PHLCON);
	MODM_LOG_DEBUG.printf("PHLCON=%04x\n", phlcon);


	while (true)
	{
		LedGreen::set();
		modm::delay(100ms);

		LedGreen::reset();
		modm::delay(90ms);
	}

	return 0;
}
