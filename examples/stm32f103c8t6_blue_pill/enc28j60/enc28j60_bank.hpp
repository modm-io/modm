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


/* ENC28J60 MICMD Register Bit Definitions */
static constexpr uint8_t MICMD_MIISCAN	= 0x02;
static constexpr uint8_t MICMD_MIIRD	= 0x01;

/* ENC28J60 MISTAT Register Bit Definitions */
static constexpr uint8_t MISTAT_NVALID	= 0x04;
static constexpr uint8_t MISTAT_SCAN	= 0x02;
static constexpr uint8_t MISTAT_BUSY	= 0x01;




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
