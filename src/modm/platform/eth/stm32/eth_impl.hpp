/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ETH_HPP
#	error	"Don't include this file directly, use 'eth.hpp' instead!"
#endif

namespace modm::platform {


template <class PHY>
void
Eth<PHY>::start()
{
	// transmission enable
	uint32_t tmp = ETH->MACCR | ETH_MACCR_TE;
	writeMACCR(tmp);

	// reception enable
	tmp = ETH->MACCR | ETH_MACCR_RE;
	writeMACCR(tmp);

	// flush transmission fifo
	tmp = ETH->DMAOMR | ETH_DMAOMR_FTF;
	writeDMAOMR(tmp);

	// DMA transmission enable
	tmp = ETH->DMAOMR | ETH_DMAOMR_ST;
	writeDMAOMR(tmp);

	// DMA reception enable
	tmp = ETH->DMAOMR | ETH_DMAOMR_SR;
	writeDMAOMR(tmp);
}

template <class PHY>
void
Eth<PHY>::stop()
{
	// DMA transmission disable
	uint32_t tmp = ETH->DMAOMR & ~ETH_DMAOMR_ST;
	writeDMAOMR(tmp);

	// DMA reception disable
	tmp = ETH->DMAOMR & ~ETH_DMAOMR_SR;
	writeDMAOMR(tmp);

	// reception disable
	tmp = ETH->MACCR & ~ETH_MACCR_RE;
	writeMACCR(tmp);

	// flush transmission fifo
	tmp = ETH->DMAOMR | ETH_DMAOMR_FTF;
	writeDMAOMR(tmp);

	// transmission disable
	tmp = ETH->MACCR & ~ETH_MACCR_TE;
	writeMACCR(tmp);
}

template <class PHY>
void
Eth<PHY>::configureMacAddresses()
{
	static constexpr uint32_t ETH_MAC_ADDR_HBASE { ETH_MAC_BASE + 0x40 };  /* Ethernet MAC address high offset */
	static constexpr uint32_t ETH_MAC_ADDR_LBASE { ETH_MAC_BASE + 0x44 };  /* Ethernet MAC address low offset */

	static constexpr MacAddress zeroMac { 0 };

	for (std::size_t i = 0; i < macAddresses.size(); ++i) {
		auto const &macAddress = macAddresses[i];
		if (std::memcmp(zeroMac.data(), macAddress.data(), macAddress.size()) == 0)
			continue;

		uint32_t tmp_register = (macAddress[5] << 8) | macAddress[4];
		*reinterpret_cast<__IO uint32_t *>(ETH_MAC_ADDR_HBASE + i * 0x08) = tmp_register;
		tmp_register = (macAddress[3] << 24) | (macAddress[2] << 16) | (macAddress[1] << 8) | macAddress[0];
		*reinterpret_cast<__IO uint32_t *>(ETH_MAC_ADDR_LBASE + i * 0x08) = tmp_register;
	}
}

template <class PHY>
bool
Eth<PHY>::readPhyRegister(uint16_t reg, uint32_t &value)
{
	// get only CR bits from MACMIIAR
	uint32_t tmp = ETH->MACMIIAR & ETH_MACMIIAR_CR_Msk;
	tmp |= (PHY::Address << 11) & ETH_MACMIIAR_PA;
	tmp |= (reg << 6) & ETH_MACMIIAR_MR;
	tmp &= ~ETH_MACMIIAR_MW;
	tmp |= ETH_MACMIIAR_MB;

	ETH->MACMIIAR = tmp;

	int timeout = PHY::ReadTimeout;
	while (timeout-- > 0) {
		if ((ETH->MACMIIAR & ETH_MACMIIAR_MB) == 0) {
			// busy flag cleared, read data
			value = ETH->MACMIIDR;
			return true;
		}
	}

	return false;
}

template <class PHY>
bool
Eth<PHY>::writePhyRegister(uint16_t reg, uint32_t value)
{
	// get only CR bits from MACMIIAR
	uint32_t tmp = ETH->MACMIIAR & ETH_MACMIIAR_CR_Msk;
	tmp |= (PHY::Address << 11) & ETH_MACMIIAR_PA;
	tmp |= (reg << 6) & ETH_MACMIIAR_MR;
	tmp |= ETH_MACMIIAR_MW;
	tmp |= ETH_MACMIIAR_MB;

	ETH->MACMIIDR = value;
	ETH->MACMIIAR = tmp;

	int timeout = PHY::WriteTimeout;
	while (timeout-- > 0) {
		if ((ETH->MACMIIAR & ETH_MACMIIAR_MB) == 0)
			return true;
	}

	return false;
}

}
