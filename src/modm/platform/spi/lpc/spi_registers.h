/*
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SPI_REGISTERS_HPP
#define MODM_SPI_REGISTERS_HPP

/*******************  Bit definition for SPI_CR0 register  ********************/
#define SPI_CR0_CPOL						((uint16_t)(1 <<  6))
#define SPI_CR0_CPHA						((uint16_t)(1 <<  7))

/*******************  Bit definition for SPI_CR1 register  ********************/
#define SPI_CR1_SSE							((uint16_t)(1 <<  1))

/*******************  Bit definition for SPI_SR0/1 register  ********************/
#define SPI_SRn_TFE		((uint16_t)(1 <<  0)) ///< Transmit FIFO Empty. This bit is 1 is the Transmit FIFO is empty, 0 if not.
#define SPI_SRn_TNF		((uint16_t)(1 <<  1)) ///< Transmit FIFO Not Full. This bit is 0 if the Tx FIFO is full, 1 if not.
#define SPI_SRn_RNE		((uint16_t)(1 <<  2)) ///< Receive FIFO Not Empty. This bit is 0 if the Receive FIFO is empty, 1 if not.
#define SPI_SRn_RFF		((uint16_t)(1 <<  3)) ///< Receive FIFO Full. This bit is 1 if the Receive FIFO is full, 0 if not.
#define SPI_SRn_BSY		((uint16_t)(1 <<  4)) ///< Busy. This bit is 0 if the SPI controller is idle, 1 if it is currentl

/*******************  Bit definition for PRESETCTRL register  *****************/
#define PRESETCTRL_SSP0_RST_N				((uint16_t)(1 <<  0)) ///< SPI0 reset de-asserted
#define PRESETCTRL_SSP1_RST_N				((uint16_t)(1 <<  2)) ///< SPI1 reset de-asserted

/*******************  Bit definition for SYSAHBCLKCTRL register  **************/
#define SYSAHBCLKCTRL_SSP0					((uint16_t)(1 << 11)) ///< Enables clock for SPI0
#define SYSAHBCLKCTRL_SSP1					((uint32_t)(1 << 18)) ///< Enables clock for SPI1

#endif // MODM_SPI_REGISTERS_HPP
