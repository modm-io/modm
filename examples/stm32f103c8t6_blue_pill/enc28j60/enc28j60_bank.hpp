#ifndef MODM_ENC28J60_BANK
#define MODM_ENC28J60_BANK

static constexpr uint8_t BANK_0 = 0 << 5;
static constexpr uint8_t BANK_1 = 1 << 5;
static constexpr uint8_t BANK_2 = 2 << 5;
static constexpr uint8_t BANK_3 = 3 << 5;

static constexpr uint8_t ADDR_MASK = 0x1F;
static constexpr uint8_t BANK_MASK = 0x60;
static constexpr uint8_t SPRD_MASK = 0x80;

enum class RegisterBank : uint8_t
{
	BANK_0 = BANK_0,
	BANK_1 = BANK_1,
	BANK_2 = BANK_2,
	BANK_3 = BANK_3,
	BANK_ALL = 0xff
};

/* ENC28J60 register definitions.
   Blindly copied from enc28j60_hw.h from the Linux kernel
 */

enum class Register : uint8_t
{
	/* All-bank registers */
	EIE			= 0x1B,
	EIR			= 0x1C,
	ESTAT		= 0x1D,
	ECON2		= 0x1E,
	ECON1		= 0x1F,

	/* Bank 0 registers */
	ERDPTL		= 0x00 | BANK_0,
	ERDPTH		= 0x01 | BANK_0,
	EWRPTL		= 0x02 | BANK_0,
	EWRPTH		= 0x03 | BANK_0,
	ETXSTL		= 0x04 | BANK_0,
	ETXSTH		= 0x05 | BANK_0,
	ETXNDL		= 0x06 | BANK_0,
	ETXNDH		= 0x07 | BANK_0,
	ERXSTL		= 0x08 | BANK_0,
	ERXSTH		= 0x09 | BANK_0,
	ERXNDL		= 0x0A | BANK_0,
	ERXNDH		= 0x0B | BANK_0,
	ERXRDPTL	= 0x0C | BANK_0,
	ERXRDPTH	= 0x0D | BANK_0,
	ERXWRPTL	= 0x0E | BANK_0,
	ERXWRPTH	= 0x0F | BANK_0,
	EDMASTL		= 0x10 | BANK_0,
	EDMASTH		= 0x11 | BANK_0,
	EDMANDL		= 0x12 | BANK_0,
	EDMANDH		= 0x13 | BANK_0,
	EDMADSTL	= 0x14 | BANK_0,
	EDMADSTH	= 0x15 | BANK_0,
	EDMACSL		= 0x16 | BANK_0,
	EDMACSH		= 0x17 | BANK_0,

	/* Bank 1 registers */
	EHT0		= 0x00 | BANK_1,
	EHT1		= 0x01 | BANK_1,
	EHT2		= 0x02 | BANK_1,
	EHT3		= 0x03 | BANK_1,
	EHT4		= 0x04 | BANK_1,
	EHT5		= 0x05 | BANK_1,
	EHT6		= 0x06 | BANK_1,
	EHT7		= 0x07 | BANK_1,
	EPMM0		= 0x08 | BANK_1,
	EPMM1		= 0x09 | BANK_1,
	EPMM2		= 0x0A | BANK_1,
	EPMM3		= 0x0B | BANK_1,
	EPMM4		= 0x0C | BANK_1,
	EPMM5		= 0x0D | BANK_1,
	EPMM6		= 0x0E | BANK_1,
	EPMM7		= 0x0F | BANK_1,
	EPMCSL		= 0x10 | BANK_1,
	EPMCSH		= 0x11 | BANK_1,
	EPMOL		= 0x14 | BANK_1,
	EPMOH		= 0x15 | BANK_1,
	EWOLIE		= 0x16 | BANK_1,
	EWOLIR		= 0x17 | BANK_1,
	ERXFCON		= 0x18 | BANK_1,
	EPKTCNT		= 0x19 | BANK_1,

	/* Bank 2 registers */
	MACON1		= 0x00 | BANK_2 | SPRD_MASK,
	// MACON2	= 0x01 | BANK_2 | SPRD_MASK,
	MACON3		= 0x02 | BANK_2 | SPRD_MASK,
	MACON4		= 0x03 | BANK_2 | SPRD_MASK,
	MABBIPG		= 0x04 | BANK_2 | SPRD_MASK,
	MAIPGL		= 0x06 | BANK_2 | SPRD_MASK,
	MAIPGH		= 0x07 | BANK_2 | SPRD_MASK,
	MACLCON1	= 0x08 | BANK_2 | SPRD_MASK,
	MACLCON2	= 0x09 | BANK_2 | SPRD_MASK,
	MAMXFLL		= 0x0A | BANK_2 | SPRD_MASK,
	MAMXFLH		= 0x0B | BANK_2 | SPRD_MASK,
	MAPHSUP		= 0x0D | BANK_2 | SPRD_MASK,
	MICON		= 0x11 | BANK_2 | SPRD_MASK,
	MICMD		= 0x12 | BANK_2 | SPRD_MASK,
	MIREGADR	= 0x14 | BANK_2 | SPRD_MASK,
	MIWRL		= 0x16 | BANK_2 | SPRD_MASK,
	MIWRH		= 0x17 | BANK_2 | SPRD_MASK,
	MIRDL		= 0x18 | BANK_2 | SPRD_MASK,
	MIRDH		= 0x19 | BANK_2 | SPRD_MASK,

	/* Bank 3 registers */
	MAADR1		= 0x00 | BANK_3 | SPRD_MASK,
	MAADR0		= 0x01 | BANK_3 | SPRD_MASK,
	MAADR3		= 0x02 | BANK_3 | SPRD_MASK,
	MAADR2		= 0x03 | BANK_3 | SPRD_MASK,
	MAADR5		= 0x04 | BANK_3 | SPRD_MASK,
	MAADR4		= 0x05 | BANK_3 | SPRD_MASK,
	EBSTSD		= 0x06 | BANK_3,
	EBSTCON		= 0x07 | BANK_3,
	EBSTCSL		= 0x08 | BANK_3,
	EBSTCSH		= 0x09 | BANK_3,
	MISTAT		= 0x0A | BANK_3 | SPRD_MASK,
	EREVID		= 0x12 | BANK_3,
	ECOCON		= 0x15 | BANK_3,
	EFLOCON		= 0x17 | BANK_3,
	EPAUSL		= 0x18 | BANK_3,
	EPAUSH		= 0x19 | BANK_3,
};

/* ENC28J60 EIE Register Bit Definitions */
static constexpr uint8_t EIE_INTIE	= 0x80;
static constexpr uint8_t EIE_PKTIE	= 0x40;
static constexpr uint8_t EIE_DMAIE	= 0x20;
static constexpr uint8_t EIE_LINKIE	= 0x10;
static constexpr uint8_t EIE_TXIE	= 0x08;
/* static constexpr uint8_t  EIE_WOLIE	0x04 (reserved) */
static constexpr uint8_t EIE_TXERIE	= 0x02;
static constexpr uint8_t EIE_RXERIE	= 0x01;

/* ENC28J60 EIR Register Bit Definitions */
static constexpr uint8_t EIR_PKTIF	= 0x40;
static constexpr uint8_t EIR_DMAIF	= 0x20;
static constexpr uint8_t EIR_LINKIF	= 0x10;
static constexpr uint8_t EIR_TXIF	= 0x08;
/* static constexpr uint8_t EIR_WOLIF	0x04 (reserved) */
static constexpr uint8_t EIR_TXERIF	= 0x02;
static constexpr uint8_t EIR_RXERIF	= 0x01;

/* ENC28J60 ESTAT Register Bit Definitions */
static constexpr uint8_t ESTAT_INT		= 0x80;
static constexpr uint8_t ESTAT_LATECOL	= 0x10;
static constexpr uint8_t ESTAT_RXBUSY	= 0x04;
static constexpr uint8_t ESTAT_TXABRT	= 0x02;
static constexpr uint8_t ESTAT_CLKRDY	= 0x01;

/* ENC28J60 ECON2 Register Bit Definitions */
static constexpr uint8_t ECON2_AUTOINC	= 0x80;
static constexpr uint8_t ECON2_PKTDEC	= 0x40;
static constexpr uint8_t ECON2_PWRSV	= 0x20;
static constexpr uint8_t ECON2_VRPS		= 0x08;

/* ENC28J60 ECON1 Register Bit Definitions */
static constexpr uint8_t ECON1_TXRST	= 0x80;
static constexpr uint8_t ECON1_RXRST	= 0x40;
static constexpr uint8_t ECON1_DMAST	= 0x20;
static constexpr uint8_t ECON1_CSUMEN	= 0x10;
static constexpr uint8_t ECON1_TXRTS	= 0x08;
static constexpr uint8_t ECON1_RXEN		= 0x04;
static constexpr uint8_t ECON1_BSEL1	= 0x02;
static constexpr uint8_t ECON1_BSEL0	= 0x01;

/* ENC28J60 MACON1 Register Bit Definitions */
static constexpr uint8_t MACON1_LOOPBK	= 0x10;
static constexpr uint8_t MACON1_TXPAUS	= 0x08;
static constexpr uint8_t MACON1_RXPAUS	= 0x04;
static constexpr uint8_t MACON1_PASSALL	= 0x02;
static constexpr uint8_t MACON1_MARXEN	= 0x01;

/* ENC28J60 MACON2 Register Bit Definitions */
static constexpr uint8_t MACON2_MARST	= 0x80;
static constexpr uint8_t MACON2_RNDRST	= 0x40;
static constexpr uint8_t MACON2_MARXRST	= 0x08;
static constexpr uint8_t MACON2_RFUNRST	= 0x04;
static constexpr uint8_t MACON2_MATXRST	= 0x02;
static constexpr uint8_t MACON2_TFUNRST	= 0x01;

/* ENC28J60 MACON3 Register Bit Definitions */
static constexpr uint8_t MACON3_PADCFG2	= 0x80;
static constexpr uint8_t MACON3_PADCFG1	= 0x40;
static constexpr uint8_t MACON3_PADCFG0	= 0x20;
static constexpr uint8_t MACON3_TXCRCEN	= 0x10;
static constexpr uint8_t MACON3_PHDRLEN	= 0x08;
static constexpr uint8_t MACON3_HFRMLEN	= 0x04;
static constexpr uint8_t MACON3_FRMLNEN	= 0x02;
static constexpr uint8_t MACON3_FULDPX	= 0x01;


/* ENC28J60 MICMD Register Bit Definitions */
static constexpr uint8_t MICMD_MIISCAN	= 0x02;
static constexpr uint8_t MICMD_MIIRD	= 0x01;

/* ENC28J60 MISTAT Register Bit Definitions */
static constexpr uint8_t MISTAT_NVALID	= 0x04;
static constexpr uint8_t MISTAT_SCAN	= 0x02;
static constexpr uint8_t MISTAT_BUSY	= 0x01;

/* ENC28J60 ERXFCON Register Bit Definitions */
static constexpr uint8_t ERXFCON_UCEN	= 0x80;
static constexpr uint8_t ERXFCON_ANDOR	= 0x40;
static constexpr uint8_t ERXFCON_CRCEN	= 0x20;
static constexpr uint8_t ERXFCON_PMEN	= 0x10;
static constexpr uint8_t ERXFCON_MPEN	= 0x08;
static constexpr uint8_t ERXFCON_HTEN	= 0x04;
static constexpr uint8_t ERXFCON_MCEN	= 0x02;
static constexpr uint8_t ERXFCON_BCEN	= 0x01;

/* ENC28J60 PHY PHCON1 Register Bit Definitions */
static constexpr uint16_t PHCON1_PRST		= 0x8000;
static constexpr uint16_t PHCON1_PLOOPBK	= 0x4000;
static constexpr uint16_t PHCON1_PPWRSV		= 0x0800;
static constexpr uint16_t PHCON1_PDPXMD		= 0x0100;

/* ENC28J60 PHY PHSTAT1 Register Bit Definitions */
static constexpr uint16_t PHSTAT1_PFDPX		= 0x1000;
static constexpr uint16_t PHSTAT1_PHDPX		= 0x0800;
static constexpr uint16_t PHSTAT1_LLSTAT	= 0x0004;
static constexpr uint16_t PHSTAT1_JBSTAT	= 0x0002;

/* ENC28J60 PHY PHSTAT2 Register Bit Definitions */
static constexpr uint16_t PHSTAT2_TXSTAT	= (1 << 13);
static constexpr uint16_t PHSTAT2_RXSTAT	= (1 << 12);
static constexpr uint16_t PHSTAT2_COLSTAT	= (1 << 11);
static constexpr uint16_t PHSTAT2_LSTAT		= (1 << 10);
static constexpr uint16_t PHSTAT2_DPXSTAT	= (1 << 9);
static constexpr uint16_t PHSTAT2_PLRITY	= (1 << 5);

/* ENC28J60 PHY PHCON2 Register Bit Definitions */
static constexpr uint16_t PHCON2_FRCLINK	= 0x4000;
static constexpr uint16_t PHCON2_TXDIS	= 0x2000;
static constexpr uint16_t PHCON2_JABBER	= 0x0400;
static constexpr uint16_t PHCON2_HDLDIS	= 0x0100;

/* ENC28J60 PHY PHIE Register Bit Definitions */
static constexpr uint8_t PHIE_PLNKIE	= (1 << 4);
static constexpr uint8_t PHIE_PGEIE		= (1 << 1);

/* ENC28J60 PHY PHIR Register Bit Definitions */
static constexpr uint8_t PHIR_PLNKIF	= (1 << 4);
static constexpr uint8_t PHIR_PGEIF		= (1 << 1);

/* Preferred half duplex: LEDA: Link status LEDB: Rx/Tx activity */
static constexpr uint16_t ENC28J60_LAMPS_MODE	= 0x3476;





static uint8_t get_bank(const Register address) {
	return (static_cast<uint8_t>(address) & BANK_MASK) >> 5;
}

static uint8_t get_address(const Register address) {
	return (static_cast<uint8_t>(address) & ADDR_MASK);
}

static bool is_mii_mac(const Register address) {
	return (static_cast<uint8_t>(address) & SPRD_MASK);
}

Register operator+ (const Register& lhs, const int& rhs) {
	return static_cast<Register>( static_cast<uint8_t>(lhs) + rhs );
}

bool operator== (const Register& lhs, const RegisterBank& rhs)
{
	switch (rhs) {
		case RegisterBank::BANK_ALL:
			switch (lhs) {
				case Register::EIE:
				case Register::EIR:
				case Register::ESTAT:
				case Register::ECON2:
				case Register::ECON1:
					return true;

				default:
					return false;
			}

		case RegisterBank::BANK_0:
		case RegisterBank::BANK_1:
		case RegisterBank::BANK_2:
		case RegisterBank::BANK_3:
			return (static_cast<uint8_t>(lhs) | BANK_MASK) == static_cast<uint8_t>(rhs);
	}

	// In case of wrong enums
	return false;
}



#endif // MODM_ENC28J60_BANK
