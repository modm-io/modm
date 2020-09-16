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

#include "enc28j60_bank.hpp"


/// Length of standard SPI operation,
static constexpr uint8_t SPI_OPLEN = 2;



/* ENC28J60 ECON1 Register Bit Definitions */
static constexpr uint8_t ECON1_TXRST	= 0x80;
static constexpr uint8_t ECON1_RXRST	= 0x40;
static constexpr uint8_t ECON1_DMAST	= 0x20;
static constexpr uint8_t ECON1_CSUMEN	= 0x10;
static constexpr uint8_t ECON1_TXRTS	= 0x08;
static constexpr uint8_t ECON1_RXEN		= 0x04;
static constexpr uint8_t ECON1_BSEL1	= 0x02;
static constexpr uint8_t ECON1_BSEL0	= 0x01;



enum class PhyRegister : uint8_t
{
	PHCON1		= 0x00,
	PHSTAT1		= 0x01,
	PHHID1		= 0x02,
	PHHID2		= 0x03,
	PHCON2		= 0x10,
	PHSTAT2		= 0x11,
	PHIE		= 0x12,
	PHIR		= 0x13,
	PHLCON		= 0x14
};

/* buffer boundaries applied to internal 8K ram
 * entire available packet buffer space is allocated.
 * Give TX buffer space for one full ethernet frame (~1500 bytes)
 * receive buffer gets the rest */
static constexpr uint16_t TXSTART_INIT	= 0x1A00;
static constexpr uint16_t TXEND_INIT	= 0x1FFF;

/* Put RX buffer at 0 as suggested by the Errata datasheet */
static constexpr uint16_t RXSTART_INIT	= 0x0000;
static constexpr uint16_t RXEND_INIT	= 0x19FF;

/// Maximum Ethernet frame length
static constexpr uint16_t MAX_FRAMELEN = 1518;

/// Buffer size required for the largest SPI transfer (i.e., reading a frame).
static constexpr uint16_t SPI_TRANSFER_BUF_LEN = (4 + MAX_FRAMELEN);


uint8_t spi_transfer_buf[SPI_TRANSFER_BUF_LEN];

class Enc28j60
{
public:

	/// Soft reset of chip
	static void
	enc28j60_soft_reset() {
		spi_write_op(SpiOperation::SOFT_RESET, Register::ERDPTL /* 0 */, 0xff);
		/* Errata workaround #1, CLKRDY check is unreliable,
		 * delay at least 1 ms instead */
		modm::delay(2ms);
}

	/// Debug routine to dump useful register contents
	static void
	enc28j60_dump_regs(const char *msg)
	{
		MODM_LOG_DEBUG.printf(
			" %s\n"
			"HwRevID: 0x%02x\n"
			"Cntrl: ECON1 ECON2 ESTAT EIR   EIE\n"
			"       0x%02x  0x%02x  0x%02x  0x%02x  0x%02x\n"
			"MAC  : MACON1 MACON3 MACON4\n"
			"       0x%02x   0x%02x   0x%02x\n"
			"Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON EPKTCNT MAMXFL\n"
			"       0x%04x 0x%04x 0x%04x  0x%04x  "
			"0x%02x    0x%02x    0x%04x\n"
			"Tx   : ETXST  ETXND  MACLCON1 MACLCON2 MAPHSUP\n"
			"       0x%04x 0x%04x 0x%02x     0x%02x     0x%02x\n",

			msg, nolock_regb_read(Register::EREVID),
			nolock_regb_read(Register::ECON1), nolock_regb_read(Register::ECON2),
			nolock_regb_read(Register::ESTAT), nolock_regb_read(Register::EIR),
			nolock_regb_read(Register::EIE), nolock_regb_read(Register::MACON1),
			nolock_regb_read(Register::MACON3), nolock_regb_read(Register::MACON4),
			nolock_regw_read(Register::ERXSTL), nolock_regw_read(Register::ERXNDL),
			nolock_regw_read(Register::ERXWRPTL),
			nolock_regw_read(Register::ERXRDPTL),
			nolock_regb_read(Register::ERXFCON),
			nolock_regb_read(Register::EPKTCNT),
			nolock_regw_read(Register::MAMXFLL), nolock_regw_read(Register::ETXSTL),
			nolock_regw_read(Register::ETXNDL),
			nolock_regb_read(Register::MACLCON1),
			nolock_regb_read(Register::MACLCON2),
			nolock_regb_read(Register::MAPHSUP));
	}


	/// PHY register read
	static uint16_t
	enc28j60_phy_read(const PhyRegister address)
	{
		// mutex_lock

		/* set the PHY register address */
		nolock_regb_write(Register::MIREGADR, address);
		/* start the register read operation */
		nolock_regb_write(Register::MICMD, MICMD_MIIRD);
		/* wait until the PHY read completes */
		wait_phy_ready();
		/* quit reading */
		nolock_regb_write(Register::MICMD, 0x00);
		/* return the data */
		uint16_t ret = nolock_regw_read(Register::MIRDL);
		// mutex_unlock

		return ret;
	}


	/// PHY register write
	static int8_t
	enc28j60_phy_write(const PhyRegister address, const uint16_t data)
	{
		// mutex_lock

		/* set the PHY register address */
		nolock_regb_write(Register::MIREGADR, address);
		/* write the PHY data */
		nolock_regw_write(Register::MIWRL, data);
		/* wait until the PHY write completes and return */
		int8_t ret = wait_phy_ready();

		// mutex_unlock
		return ret;
	}


private:
	/// Wait until the PHY operation is complete.
	static int8_t wait_phy_ready()
	{
		return poll_ready(Register::MISTAT, MISTAT_BUSY, 0) ? 0 : 1;
	}

	static int8_t poll_ready(const Register address, uint8_t bitmask, uint8_t value)
	{
		/* 20 msec timeout read */
		uint8_t tt = 20;
		while ((nolock_regb_read(address) & bitmask) != value) {
			// ToDo modm::Timeout
			if (--tt == 0) {
				MODM_LOG_ERROR.printf("Timeout poll_read 0x%02x, 0x%02x, 0x%02x\n",
					static_cast<uint8_t>(address), bitmask, value);
				return -1;
			}
		}

		return 0;
	}

	
	/*
	 * Register access routines through the SPI bus.
	 * Every register access comes in two flavours:
	 * - nolock_xxx: caller needs to invoke mutex_lock, usually to access
	 *   atomically more than one register
	 * - locked_xxx: caller doesn't need to invoke mutex_lock, single access
	 *
	 * Some registers can be accessed through the bit field clear and
	 * bit field set to avoid a read modify write cycle.
	 */

	/// Register bit field Set
	static void nolock_reg_bfset(const Register address, const uint8_t bitmask) {
		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::BIT_FIELD_SET, address, bitmask);
	}

	static void locked_reg_bfset(const Register address, const uint8_t bitmask) {
		// mutex_lock
		nolock_reg_bfset(address, bitmask);
		// mutex_unlock
	}

	/// Register bit field Clear
	static void nolock_reg_bfclr(const Register address, const uint8_t bitmask) {
		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::BIT_FIELD_CLR, address, bitmask);
	}

	static void locked_reg_bfclr(const Register address, const uint8_t bitmask) {
		// mutex_lock
		nolock_reg_bfclr(address, bitmask);
		// mutex_unlock
	}


	/// Register byte read
	static uint8_t nolock_regb_read(const Register address) {
		enc28j60_set_bank(address);
		return spi_read_op(SpiOperation::READ_CTRL_REG, address);
	}

	static uint8_t locked_regb_read(const Register address) {
		// mutex_lock
		auto ret = nolock_regb_read(address);
		// mutex_unlock

		return ret;
	}


	/// Register word read
	static uint16_t nolock_regw_read(const Register address) {
		// assert address is even
		// assert address is word address (register with L and H)

		enc28j60_set_bank(address);
		uint8_t rl = spi_read_op(SpiOperation::READ_CTRL_REG, address);
		uint8_t rh = spi_read_op(SpiOperation::READ_CTRL_REG, address + 1);

		return (rh << 8) | rl;
	}

	static uint16_t locked_regw_read(const Register address) {
		// mutex_lock
		auto ret = nolock_regw_read(address);
		// mutex_unlock

		return ret;
	}


	/// Register byte write
	static void nolock_regb_write(const Register address, const uint8_t data) {
		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::WRITE_CTRL_REG, address, data);
	}

	static void nolock_regb_write(const Register address, const PhyRegister phy_register) {
		nolock_regb_write(address, static_cast<uint8_t>(phy_register));
	}

	static void locked_regb_write(const Register address, const uint8_t data) {
		// mutex_lock
		nolock_regb_write(address, data);
		// mutex_unlock
	}


	/// Register word write
	static void nolock_regw_write(const Register address, const uint16_t data) {
		// assert address is even
		// assert address is word address (register with L and H)

		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::WRITE_CTRL_REG, address, data);
		spi_write_op(SpiOperation::WRITE_CTRL_REG, address + 1, data >> 8);
	}

	static void locked_regw_write(const Register address, const uint16_t data) {
		// mutex_lock
		nolock_regw_write(address, data);
		// mutex_unlock
	}


	/// Buffer memory read
	static void enc28j60_mem_read(const uint16_t address, std::size_t len, uint8_t *data) {
		// mutex_lock
		nolock_regw_write(Register::ERDPTL, address);

		if (true) {
			uint16_t reg = nolock_regw_read(Register::ERDPTL);
			if (reg != address) {
				MODM_LOG_ERROR.printf("Error writing ERDPT (0x%04x - 0x%04x)\n",
					reg, address);
			}
		}

		spi_read_buf(len, data);
		// mutex_unlock
	}


	/// Buffer memory write
	static void enc28j60_packet_write(const std::size_t len, const uint8_t *data)
	{
		// mutex_lock

		/* Set the write pointer to start of transmit buffer area */
		nolock_regw_write(Register::EWRPTL, TXSTART_INIT);

		if (true) {
			uint16_t reg = nolock_regw_read(Register::EWRPTL);
			if (reg != TXSTART_INIT) {
				MODM_LOG_ERROR.printf("ERWPT:0x%04x != 0x%04x\n", reg, TXSTART_INIT);
			}
		}

		/* Set the TXND pointer to correspond to the packet size given */
		nolock_regw_write(Register::ETXNDL, TXSTART_INIT + len);
		/* write per-packet control byte */
		spi_write_op(SpiOperation::WRITE_BUF_MEM, Register::ERDPTL/* 0 */, 0x00);
		MODM_LOG_DEBUG.printf("after control byte ERWPT:0x%04x\n", nolock_regw_read(Register::EWRPTL));

		/* copy the packet into the transmit buffer */
		spi_write_buf(len, data);
		MODM_LOG_DEBUG.printf("after write packet ERWPT:0x%04x, len=%d\n", nolock_regw_read(Register::EWRPTL), len);

		// mutex_unlock
	}


	static void
	enc28j60_set_bank(const Register address)
	{
		/* These registers (EIE, EIR, ESTAT, ECON2, ECON1)
		 * are present in all banks, no need to switch bank.
		 */
		if (address == RegisterBank::BANK_ALL) {
			return;
		}

		uint8_t new_bank = get_bank(address);

		/* Clear or set each bank selection bit as needed */
		if ((new_bank & ECON1_BSEL0) != (bank & ECON1_BSEL0)) {
			if (new_bank & ECON1_BSEL0) {
				spi_write_op(SpiOperation::BIT_FIELD_SET, Register::ECON1, ECON1_BSEL0);
			} else {
				spi_write_op(SpiOperation::BIT_FIELD_CLR, Register::ECON1, ECON1_BSEL0);
			}
		}
		if ((new_bank & ECON1_BSEL1) != (bank & ECON1_BSEL1)) {
			if (new_bank & ECON1_BSEL1) {
				spi_write_op(SpiOperation::BIT_FIELD_SET, Register::ECON1, ECON1_BSEL1);
			} else {
				spi_write_op(SpiOperation::BIT_FIELD_CLR, Register::ECON1, ECON1_BSEL1);
			}
		}

		// Bank change successful, so store
		bank = new_bank;
	}


	enum class SpiOperation : uint8_t
	{
		READ_CTRL_REG	= 0b000 << 5, // RCR
		READ_BUF_MEM	= 0b001 << 5 | 0x1a, // RBM
		WRITE_CTRL_REG	= 0b010 << 5, // WCR
		WRITE_BUF_MEM	= 0b011 << 5 | 0x1a, // WBM
		BIT_FIELD_SET	= 0b100 << 5, // BFS
		BIT_FIELD_CLR	= 0b101 << 5, // BFC
		SOFT_RESET		= 0b111 << 5  // SRC
	};


	/// SPI read buffer
	static void
	spi_read_buf(const std::size_t len, uint8_t *data)
	{
		uint8_t *tx_buf = spi_transfer_buf;

		tx_buf[0] = static_cast<uint8_t>(SpiOperation::READ_BUF_MEM);

		// assert len <= SPI_TRANSFER_BUF_LEN
		enc28j60::Cs::reset();
		enc28j60::SpiMaster::transferBlocking(tx_buf, data, len);
		enc28j60::Cs::set();
	}


	/// SPI write buffer
	static void
	spi_write_buf(const std::size_t len, const uint8_t *data)
	{
		if (len > SPI_TRANSFER_BUF_LEN - 1) {
			MODM_LOG_ERROR.printf("len(%d) > SPI_TRANSFER_BUF_LEN - 1\n", len);
			return;
		} else if (len <= 0) {
			// nothing to do
			return;
		} else {
			spi_transfer_buf[0] = static_cast<uint8_t>(SpiOperation::WRITE_BUF_MEM);
			memcpy(&spi_transfer_buf[1], data, len);

			enc28j60::Cs::reset();
			enc28j60::SpiMaster::transferBlocking(spi_transfer_buf, nullptr, len + 1);
			enc28j60::Cs::set();
		}
	}


	// basic SPI read operation
	static uint8_t
	spi_read_op(SpiOperation op, Register address)
	{
		// buffers account for dummy byte for MII and MAC transfers
		uint8_t tx_buf[SPI_OPLEN + 1];
		uint8_t rx_buf[SPI_OPLEN + 1];

		uint8_t slen = SPI_OPLEN;

		/* add dummy read if needed */
		if (is_mii_mac(address)) {
			slen++;
		}

		tx_buf[0] = static_cast<uint8_t>(op) | get_address(address);

		enc28j60::Cs::reset();
		enc28j60::SpiMaster::transferBlocking(tx_buf, rx_buf, slen);
		enc28j60::Cs::set();

		return rx_buf[slen - 1];
	}

	static void
	spi_write_op(SpiOperation op, Register address, uint8_t value)
	{
		uint8_t tx_buf[2];

		tx_buf[0] = static_cast<uint8_t>(op) | get_address(address);
		tx_buf[1] = value;

		enc28j60::Cs::reset();
		enc28j60::SpiMaster::transferBlocking(tx_buf, nullptr, 2);
		enc28j60::Cs::set();
	}


private:
	static uint8_t bank;


};

uint8_t Enc28j60::bank = 0;

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

	// Testing Register and RegisterBank class
	if (true)
	{
		Register econ1 = Register::ECON1;

		MODM_LOG_DEBUG.printf("cast = %02x\n", static_cast<uint8_t>(econ1));
		MODM_LOG_DEBUG.printf("BANK_ALL = %d\n", (econ1 == RegisterBank::BANK_ALL));

		MODM_LOG_DEBUG.printf("+1: %d %d\n",
			static_cast<uint8_t>(Register::ECON1), static_cast<uint8_t>(Register::ECON1 + 1));

		for (uint8_t ii = 0; ii < 80; ++ii) {
			MODM_LOG_DEBUG.printf("*");
		}
		MODM_LOG_DEBUG.printf("\n");
	}

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


	// Dump registers
	Enc28j60::enc28j60_dump_regs("Hw init");

	// Access PHY for testing communication

	uint16_t phlcon = Enc28j60::enc28j60_phy_read(PhyRegister::PHLCON);
	MODM_LOG_DEBUG.printf("PHLCON=%04x\n", phlcon);

	// blink LED B (orange) fast
	// blink LED A (green)  slow
	//                                         0011 LEDA LEDB 0010
	Enc28j60::enc28j60_phy_write(PhyRegister::PHLCON, 0b0011'1011'1010'0010);

	phlcon = Enc28j60::enc28j60_phy_read(PhyRegister::PHLCON);
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
