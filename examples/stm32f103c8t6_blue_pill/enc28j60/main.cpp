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


enum class RxFilter : uint8_t {
	PROMISC,
	MULTI,
	NORMAL
};

uint8_t macaddr[6] = {0xfe, 0xa7, 0x75, 0x3a, 0x20, 0xfb};

class Enc28j60
{
public:

	/// Bring the chip up. This should be the first method to be called
	static int8_t
	enc28j60_hw_init() {
		// mutex_lock

		/* first reset the chip */
		enc28j60_soft_reset();

		/* Clear ECON1 */
		spi_write_op(SpiOperation::WRITE_CTRL_REG, Register::ECON1, 0x00);

		bank = 0;
		hw_enable = false;
		tx_retry_count = 0;
		max_pk_counter = 0;
		rxfilter = RxFilter::NORMAL;

		/* enable address auto increment and voltage regulator powersave */
		nolock_regb_write(Register::ECON2, ECON2_AUTOINC | ECON2_VRPS);

		nolock_rxfifo_init(RXSTART_INIT, RXEND_INIT);
		nolock_txfifo_init(TXSTART_INIT, TXEND_INIT);

		// mutex_unlock

		/*
		 * Check the RevID.
		 * If it's 0x00 or 0xFF probably the enc28j60 is not mounted or
		 * damaged.
		 */
		uint8_t revid = locked_regb_read(Register::EREVID);
		MODM_LOG_DEBUG.printf("chip RevID: 0x%02x\n", revid);
		if (revid == 0x00 or revid == 0xff) {
			MODM_LOG_ERROR.printf("Invalid RevId %d\n", revid);
			return 0;
		}

		/* default filter mode: (unicast OR broadcast) AND crc valid */
		locked_regb_write(Register::ERXFCON,
				    ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

		/* enable MAC receive */
		locked_regb_write(Register::MACON1,
				    MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);

		/* enable automatic padding and CRC operations */
		if (full_duplex) {
			locked_regb_write(Register::MACON3,
					    MACON3_PADCFG0 | MACON3_TXCRCEN |
					    MACON3_FRMLNEN | MACON3_FULDPX);
			/* set inter-frame gap (non-back-to-back) */
			locked_regb_write(Register::MAIPGL, 0x12);
			/* set inter-frame gap (back-to-back) */
			locked_regb_write(Register::MABBIPG, 0x15);
		} else {
			locked_regb_write(Register::MACON3,
					    MACON3_PADCFG0 | MACON3_TXCRCEN |
					    MACON3_FRMLNEN);
			locked_regb_write(Register::MACON4, 1 << 6);	/* DEFER bit */
			/* set inter-frame gap (non-back-to-back) */
			locked_regw_write(Register::MAIPGL, 0x0C12);
			/* set inter-frame gap (back-to-back) */
			locked_regb_write(Register::MABBIPG, 0x12);
		}

		/*
		 * MACLCON1 (default)
		 * MACLCON2 (default)
		 * Set the maximum packet size which the controller will accept.
		 */
		locked_regw_write(Register::MAMXFLL, MAX_FRAMELEN);

		/* Configure LEDs */
		if (not enc28j60_phy_write(PhyRegister::PHLCON, ENC28J60_LAMPS_MODE)) {
			return 0;
		}

		if (full_duplex) {
			if (not enc28j60_phy_write(PhyRegister::PHCON1, PHCON1_PDPXMD)) {
				return 0;
			}
			if (not enc28j60_phy_write(PhyRegister::PHCON2, 0x00)) {
				return 0;
			}
		} else {
			if (not enc28j60_phy_write(PhyRegister::PHCON1, 0x00)) {
				return 0;
			}
			if (not enc28j60_phy_write(PhyRegister::PHCON2, PHCON2_HDLDIS)) {
				return 0;
			}
		}
		enc28j60_dump_regs("Hw initialized.");
		return 1;
	}


	/*
	 * Open/initialize the board. This is called (in the current kernel)
	 * sometime after booting when the 'ifconfig' program is run.
	 *
	 * This routine should set everything up anew at each open, even
	 * registers that "should" only need to be set once at boot, so that
	 * there is non-reboot way to recover if something goes wrong.
	 */
	static int
	enc28j60_net_open()
	{
		/* Reset the hardware here (and take it out of low power mode) */
		enc28j60_lowpower(false);
		enc28j60_hw_disable();
		if (not enc28j60_hw_init()) {
			MODM_LOG_ERROR.printf("hw_reset() failed\n");
			return -1; // EINVAL;
		}
		/* Update the MAC address (in case user has changed it) */
		enc28j60_set_hw_macaddr(macaddr);
		/* Enable interrupts */
		enc28j60_hw_enable();
		/* check link status */
		enc28j60_check_link_status();
		/* We are now ready to accept transmit requests from
		 * the queueing layer of the networking.
		 */
		// netif_start_queue(dev);

		return 0;
	}

	/* The inverse routine to net_open(). */
	static int enc28j60_net_close()
	{
		enc28j60_hw_disable();
		enc28j60_lowpower(true);
		// netif_stop_queue(dev);

		return 0;
	}


	/// Soft reset of chip
	static void
	enc28j60_soft_reset() {
		spi_write_op(SpiOperation::SOFT_RESET, Register::ERDPTL /* 0 */, 0xff);
		/* Errata workaround #1, CLKRDY check is unreliable,
		 * delay at least 1 ms instead */
		modm::delay(2ms);
	}

	static void
	enc28j60_hw_enable()
	{
		/* enable interrupts */
		MODM_LOG_DEBUG.printf("enabling interrupts.\n");

		enc28j60_phy_write(PhyRegister::PHIE, PHIE_PGEIE | PHIE_PLNKIE);

		// mutex_lock(&priv->lock);
		nolock_reg_bfclr(Register::EIR, EIR_DMAIF | EIR_LINKIF |
				 EIR_TXIF | EIR_TXERIF | EIR_RXERIF | EIR_PKTIF);
		nolock_regb_write(Register::EIE, EIE_INTIE | EIE_PKTIE | EIE_LINKIE |
				  EIE_TXIE | EIE_TXERIE | EIE_RXERIE);

		/* enable receive logic */
		nolock_reg_bfset(Register::ECON1, ECON1_RXEN);
		hw_enable = true;
		// mutex_unlock(&priv->lock);
	}

	static void
	enc28j60_hw_disable()
	{
		// mutex_lock(&priv->lock);
		/* disable interrupts and packet reception */
		nolock_regb_write(Register::EIE, 0x00);
		nolock_reg_bfclr(Register::ECON1, ECON1_RXEN);
		hw_enable = false;
		// mutex_unlock(&priv->lock);
	}
	/*
	 * Low power mode shrinks power consumption about 100x, so we'd like
	 * the chip to be in that mode whenever it's inactive. (However, we
	 * can't stay in low power mode during suspend with WOL active.)
	 */
	static void
	enc28j60_lowpower(const bool is_low)
	{
		MODM_LOG_DEBUG.printf("Setting power %s\n", is_low ? "low" : "high");

		// mutex_lock
		if (is_low) {
			nolock_reg_bfclr(Register::ECON1, ECON1_RXEN);
			poll_ready(Register::ESTAT, ESTAT_RXBUSY, 0);
			poll_ready(Register::ECON1, ECON1_TXRTS, 0);
			/* ECON2_VRPS was set during initialization */
			nolock_reg_bfset(Register::ECON2, ECON2_PWRSV);
		} else {
			nolock_reg_bfclr(Register::ECON2, ECON2_PWRSV);
			poll_ready(Register::ESTAT, ESTAT_CLKRDY, ESTAT_CLKRDY);
			/* caller sets ECON1_RXEN */
		}
		// mutex_unlock
	}


	static int8_t
	enc28j60_set_hw_macaddr(const uint8_t macaddr[6])
	{
		int8_t ret = 0;
		// mutex_lock
		if (not hw_enable) {
			MODM_LOG_DEBUG.printf("Setting MAC address to %02x:%02x:%02x:%02x:%02x:%02x\n",
				macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
			/* NOTE: MAC address in ENC28J60 is byte-backward */
			nolock_regb_write(Register::MAADR5, macaddr[0]);
			nolock_regb_write(Register::MAADR4, macaddr[1]);
			nolock_regb_write(Register::MAADR3, macaddr[2]);
			nolock_regb_write(Register::MAADR2, macaddr[3]);
			nolock_regb_write(Register::MAADR1, macaddr[4]);
			nolock_regb_write(Register::MAADR0, macaddr[5]);
		} else {
			MODM_LOG_ERROR.printf("Hardware must be disabled to set Mac address\n");
			ret = -1;
		}
		// mutex_unlock
		return ret;
	}


	/// Access the PHY to determine link status
	static void
	enc28j60_check_link_status()
	{
		uint16_t phstat2 = enc28j60_phy_read(PhyRegister::PHSTAT2);
		uint16_t phstat1 = enc28j60_phy_read(PhyRegister::PHSTAT1);
		MODM_LOG_DEBUG.printf("PHSTAT1: %04x, PHSTAT2: %04x\n", phstat1, phstat2);
		bool duplex = phstat2 & PHSTAT2_DPXSTAT;

		if (phstat2 & PHSTAT2_LSTAT) {
			MODM_LOG_DEBUG.printf("link up - %s\n", duplex ? "Full duplex" : "Half duplex");
		} else {
			MODM_LOG_DEBUG.printf("link down\n");
		}
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


	/// Dump an Ethernet Frame
	static void
	enc28j60_dump_packet(std::size_t len, const uint8_t* data)
	{
		for (std::size_t ii = 0; ii < len; ++ii) {
			MODM_LOG_DEBUG.printf("%02x ", data[ii]);

			if (ii % 16 == 15) {
				MODM_LOG_DEBUG.printf("\n");
			}
		}
		MODM_LOG_DEBUG.printf("\n");
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
	/// IRQ handlers

	/*
	 * RX handler
	 * Ignore PKTIF because is unreliable! (Look at the errata datasheet)
	 * Check EPKTCNT is the suggested workaround.
	 * We don't need to clear interrupt flag, automatically done when
	 * enc28j60_hw_rx() decrements the packet counter.
	 * Returns how many packet processed.
	 */
	static uint8_t
	enc28j60_rx_interrupt()
	{
		uint8_t pk_counter = locked_regb_read(Register::EPKTCNT);
		if (pk_counter) {
			MODM_LOG_DEBUG.printf("intRX, pk_cnt: %d\n", pk_counter);
		}
		if (pk_counter > max_pk_counter) {
			/* update statistics */
			max_pk_counter = pk_counter;
			if (max_pk_counter > 1) {
				MODM_LOG_DEBUG.printf("RX max_pk_cnt: %d\n", max_pk_counter);
			}
		}
		uint8_t ret = pk_counter;
		while (pk_counter-- > 0) {
			enc28j60_hw_rx();
		}

		return ret;
	}

	/*
	 * Hardware receive function.
	 * Read the buffer memory, update the FIFO pointer to free the buffer,
	 * check the status vector and decrement the packet counter.
	 */
	static void
	enc28j60_hw_rx()
	{
		MODM_LOG_DEBUG.printf("RX pk_addr:0x%04x\n", next_pk_ptr);

		if (next_pk_ptr > RXEND_INIT) {
			MODM_LOG_DEBUG.printf("Invalid packet address!! 0x%04x\n", next_pk_ptr);
			/* packet address corrupted: reset RX logic */
			// mutex_lock(&priv->lock);
			nolock_reg_bfclr(Register::ECON1, ECON1_RXEN);
			nolock_reg_bfset(Register::ECON1, ECON1_RXRST);
			nolock_reg_bfclr(Register::ECON1, ECON1_RXRST);
			nolock_rxfifo_init(RXSTART_INIT, RXEND_INIT);
			nolock_reg_bfclr(Register::EIR, EIR_RXERIF);
			nolock_reg_bfset(Register::ECON1, ECON1_RXEN);
			// mutex_unlock(&priv->lock);
			// ndev->stats.rx_errors++;
			return;
		}
		/* Read next packet pointer and rx status vector */
		uint8_t rsv[RSV_SIZE];
		enc28j60_mem_read(next_pk_ptr, sizeof(rsv), rsv);

		uint16_t next_packet = rsv[1];
		next_packet <<= 8;
		next_packet |= rsv[0];

		int len = rsv[3];
		len <<= 8;
		len |= rsv[2];

		uint16_t rxstat = rsv[5];
		rxstat <<= 8;
		rxstat |= rsv[4];

		enc28j60_dump_rsv(next_packet, len, rxstat);

		if (not RSV_GETBIT(rxstat, RSV_RXOK) or len > MAX_FRAMELEN) {
			// if (netif_msg_rx_err(priv)) {
			// 	netdev_err(ndev, "Rx Error (%04x)\n", rxstat);
			// }
			// ndev->stats.rx_errors++;
			if (RSV_GETBIT(rxstat, RSV_CRCERROR)) {
				// ndev->stats.rx_crc_errors++;
			}
			if (RSV_GETBIT(rxstat, RSV_LENCHECKERR)) {
				// ndev->stats.rx_frame_errors++;
			}
			if (len > MAX_FRAMELEN) {
				// ndev->stats.rx_over_errors++;
			}
		} else {
			// skb = netdev_alloc_skb(ndev, len + NET_IP_ALIGN);
			// if (!skb) {
			// 	if (netif_msg_rx_err(priv))
			// 		netdev_err(ndev, "out of memory for Rx'd frame\n");
			// 	ndev->stats.rx_dropped++;
			// } else {
			// 	skb_reserve(skb, NET_IP_ALIGN);
				/* copy the packet from the receive buffer */
				uint8_t buf[MAX_FRAMELEN];
				enc28j60_mem_read(
					rx_packet_start(next_pk_ptr),
					len, buf); // skb_put(skb, len));
				enc28j60_dump_packet(len, buf);
				// if (netif_msg_pktdata(priv))
				// 	dump_packet(__func__, skb->len, skb->data);
				// skb->protocol = eth_type_trans(skb, ndev);
				// /* update statistics */
				// ndev->stats.rx_packets++;
				// ndev->stats.rx_bytes += len;
				// netif_rx_ni(skb);
			// }
		}
		/*
		 * Move the RX read pointer to the start of the next
		 * received packet.
		 * This frees the memory we just read out.
		 */
		uint16_t erxrdpt = erxrdpt_workaround(next_packet, RXSTART_INIT, RXEND_INIT);
		MODM_LOG_DEBUG.printf("%s() ERXRDPT:0x%04x\n",
				   __func__, erxrdpt);

		// mutex_lock(&priv->lock);
		nolock_regw_write(Register::ERXRDPTL, erxrdpt);
	#ifdef CONFIG_ENC28J60_WRITEVERIFY
		if (netif_msg_drv(priv)) {
			u16 reg;
			reg = nolock_regw_read(priv, ERXRDPTL);
			if (reg != erxrdpt)
				MODM_LOG_DEBUG.printf(
					   "%s() ERXRDPT verify error (0x%04x - 0x%04x)\n",
					   __func__, reg, erxrdpt);
		}
	#endif
		next_pk_ptr = next_packet;
		/* we are done with this packet, decrement the packet counter */
		nolock_reg_bfset(Register::ECON2, ECON2_PKTDEC);
		// mutex_unlock(&priv->lock);
	}


private:
	/// Wait until the PHY operation is complete.
	static int8_t wait_phy_ready()
	{
		return poll_ready(Register::MISTAT, MISTAT_BUSY, 0) ? 0 : 1;
	}

	static int8_t
	poll_ready(const Register address, uint8_t bitmask, uint8_t value)
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

	/// Calculate wrap around when reading beyond the end of the RX buffer
	static uint16_t
	rx_packet_start(uint16_t ptr)
	{
		if (ptr + RSV_SIZE > RXEND_INIT) {
			return (ptr + RSV_SIZE) - (RXEND_INIT - RXSTART_INIT + 1);
		} else {
			return ptr + RSV_SIZE;
		}
	}

	/// ERXRDPT need to be set always at odd addresses, refer to errata datasheet
	static uint16_t
	erxrdpt_workaround(const uint16_t next_packet_ptr, const uint16_t start, const uint16_t end)
	{
		uint16_t erxrdpt;

		if ((next_packet_ptr - 1 < start) or (next_packet_ptr - 1 > end)) {
			erxrdpt = end;
		} else {
			erxrdpt = next_packet_ptr - 1;
		}

		return erxrdpt;
	}


	/// Dump the Receive Status Vector (RSV)
	static void
	enc28j60_dump_rsv(const uint16_t pk_ptr, uint16_t len, uint16_t sts)
	{
		MODM_LOG_DEBUG.printf("NextPk: 0x%04x - RSV:\n", pk_ptr);
		MODM_LOG_DEBUG.printf("ByteCount: %d, DribbleNibble: %d\n",
			   len, RSV_GETBIT(sts, RSV_DRIBBLENIBBLE));
		MODM_LOG_DEBUG.printf(
			   "RxOK: %d, CRCErr:%d, LenChkErr: %d, LenOutOfRange: %d\n",
			   RSV_GETBIT(sts, RSV_RXOK),
			   RSV_GETBIT(sts, RSV_CRCERROR),
			   RSV_GETBIT(sts, RSV_LENCHECKERR),
			   RSV_GETBIT(sts, RSV_LENOUTOFRANGE));
		MODM_LOG_DEBUG.printf(
			   "Multicast: %d, Broadcast: %d, LongDropEvent: %d, CarrierEvent: %d\n",
			   RSV_GETBIT(sts, RSV_RXMULTICAST),
			   RSV_GETBIT(sts, RSV_RXBROADCAST),
			   RSV_GETBIT(sts, RSV_RXLONGEVDROPEV),
			   RSV_GETBIT(sts, RSV_CARRIEREV));
		MODM_LOG_DEBUG.printf(
			   "ControlFrame: %d, PauseFrame: %d, UnknownOp: %d, VLanTagFrame: %d\n",
			   RSV_GETBIT(sts, RSV_RXCONTROLFRAME),
			   RSV_GETBIT(sts, RSV_RXPAUSEFRAME),
			   RSV_GETBIT(sts, RSV_RXUNKNOWNOPCODE),
			   RSV_GETBIT(sts, RSV_RXTYPEVLAN));
	}




	/// Receive FIFO
	static void
	nolock_rxfifo_init(const uint16_t start, const uint16_t end)
	{
		// validate parameters
		if (start > 0x1fff or end > 0x1fff or start > end) {
			MODM_LOG_ERROR.printf("RXFIFO bad parameters (start=%04x, end=%04x)\n", start, end);
			return;
		}

		/* set receive buffer start + end */
		next_pk_ptr = start;
		nolock_regw_write(Register::ERXSTL, start);
		uint16_t erxrdpt = erxrdpt_workaround(next_pk_ptr, start, end);
		nolock_regw_write(Register::ERXRDPTL, erxrdpt);
		nolock_regw_write(Register::ERXNDL, end);
	}


	/// Transmit FIFO
	static void
	nolock_txfifo_init(const uint16_t start, const uint16_t end)
	{
		// validate parameters
		if (start > 0x1fff or end > 0x1fff or start > end) {
			MODM_LOG_ERROR.printf("TXFIFO bad parameters (start=%04x, end=%04x)\n", start, end);
			return;
		}
		/* set transmit buffer start + end */
		nolock_regw_write(Register::ETXSTL, start);
		nolock_regw_write(Register::ETXNDL, end);
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
	static void
	nolock_reg_bfset(const Register address, const uint8_t bitmask) {
		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::BIT_FIELD_SET, address, bitmask);
	}

	static void
	locked_reg_bfset(const Register address, const uint8_t bitmask) {
		// mutex_lock
		nolock_reg_bfset(address, bitmask);
		// mutex_unlock
	}

	/// Register bit field Clear
	static void
	nolock_reg_bfclr(const Register address, const uint8_t bitmask) {
		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::BIT_FIELD_CLR, address, bitmask);
	}

	static void
	locked_reg_bfclr(const Register address, const uint8_t bitmask) {
		// mutex_lock
		nolock_reg_bfclr(address, bitmask);
		// mutex_unlock
	}


	/// Register byte read
	static uint8_t
	nolock_regb_read(const Register address) {
		enc28j60_set_bank(address);
		return spi_read_op(SpiOperation::READ_CTRL_REG, address);
	}

	static uint8_t
	locked_regb_read(const Register address) {
		// mutex_lock
		auto ret = nolock_regb_read(address);
		// mutex_unlock

		return ret;
	}


	/// Register word read
	static uint16_t
	nolock_regw_read(const Register address) {
		// assert address is even
		// assert address is word address (register with L and H)

		enc28j60_set_bank(address);
		uint8_t rl = spi_read_op(SpiOperation::READ_CTRL_REG, address);
		uint8_t rh = spi_read_op(SpiOperation::READ_CTRL_REG, address + 1);

		return (rh << 8) | rl;
	}

	static uint16_t
	locked_regw_read(const Register address) {
		// mutex_lock
		auto ret = nolock_regw_read(address);
		// mutex_unlock

		return ret;
	}


	/// Register byte write
	static void
	nolock_regb_write(const Register address, const uint8_t data) {
		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::WRITE_CTRL_REG, address, data);
	}

	static void
	nolock_regb_write(const Register address, const PhyRegister phy_register) {
		nolock_regb_write(address, static_cast<uint8_t>(phy_register));
	}

	static void
	locked_regb_write(const Register address, const uint8_t data) {
		// mutex_lock
		nolock_regb_write(address, data);
		// mutex_unlock
	}


	/// Register word write
	static void
	nolock_regw_write(const Register address, const uint16_t data) {
		// assert address is even
		// assert address is word address (register with L and H)

		enc28j60_set_bank(address);
		spi_write_op(SpiOperation::WRITE_CTRL_REG, address, data);
		spi_write_op(SpiOperation::WRITE_CTRL_REG, address + 1, data >> 8);
	}

	static void
	locked_regw_write(const Register address, const uint16_t data) {
		// mutex_lock
		nolock_regw_write(address, data);
		// mutex_unlock
	}


	/// Buffer memory read
	static void
	enc28j60_mem_read(const uint16_t address, std::size_t len, uint8_t *data) {
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
	static void
	enc28j60_packet_write(const std::size_t len, const uint8_t *data)
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
		uint8_t tx_buf = static_cast<uint8_t>(SpiOperation::READ_BUF_MEM);

		// assert len <= SPI_TRANSFER_BUF_LEN
		enc28j60::Cs::reset();
		enc28j60::SpiMaster::transferBlocking(tx_buf);
		enc28j60::SpiMaster::transferBlocking(nullptr, data, len);
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
	static uint8_t bank; /* current register bank selected */
	static uint16_t next_pk_ptr; /* next packet pointer within FIFO */
	static uint16_t max_pk_counter; /* statistics: max packet counter */
	static uint16_t tx_retry_count;

	static bool hw_enable;
	static bool full_duplex;
	static RxFilter rxfilter;


};

uint8_t Enc28j60::bank = 0;
uint16_t Enc28j60::next_pk_ptr = 0;
uint16_t Enc28j60::max_pk_counter = 0;

uint16_t Enc28j60::tx_retry_count = 0;
bool Enc28j60::hw_enable = false;
bool Enc28j60::full_duplex = false;
RxFilter Enc28j60::rxfilter = RxFilter::NORMAL;


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


	// Access PHY for testing communication

	// uint16_t phlcon = Enc28j60::enc28j60_phy_read(PhyRegister::PHLCON);
	// MODM_LOG_DEBUG.printf("PHLCON=%04x\n", phlcon);

	// blink LED B (orange) fast
	// blink LED A (green)  slow
	//                                         0011 LEDA LEDB 0010
	// Enc28j60::enc28j60_phy_write(PhyRegister::PHLCON, 0b0011'1011'1010'0010);

	// phlcon = Enc28j60::enc28j60_phy_read(PhyRegister::PHLCON);
	// MODM_LOG_DEBUG.printf("PHLCON=%04x\n", phlcon);

	// MODM_LOG_DEBUG.printf("**************************************************\n");




	Enc28j60::enc28j60_hw_init();
	Enc28j60::enc28j60_net_open();


	for (uint8_t ii = 0; ii < 8; ++ii) {
		Enc28j60::enc28j60_check_link_status();
		modm::delay(500ms);
	}


	if (false) {
		// Check Buffer read and write

		uint8_t rsv[RSV_SIZE] = {0x19, 0xc4, 0xb6, 0x0b, 0xb4, 0xd9 };

		// Write
		Enc28j60::nolock_regw_write(Register::ERDPTL, RXSTART_INIT);
		Enc28j60::spi_write_buf(RSV_SIZE, rsv);

		// Read back
		Enc28j60::enc28j60_mem_read(0x0000, sizeof(rsv), rsv);

		for (uint8_t ii = 0; ii < RSV_SIZE; ++ii) {
			MODM_LOG_DEBUG.printf("%02x ", rsv[ii]);
		}

	}



	while (true)
	{
		// LedGreen::set();
		// modm::delay(10ms);

		// LedGreen::reset();
		// modm::delay(90ms);

		Enc28j60::enc28j60_rx_interrupt();
	}

	return 0;
}
