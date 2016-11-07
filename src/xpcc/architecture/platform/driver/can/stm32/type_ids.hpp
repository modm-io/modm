/*
 * Copyright (c) 2013-2015, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32_CAN_TYPE_IDS_HPP
#define XPCC_STM32_CAN_TYPE_IDS_HPP

namespace xpcc
{

namespace stm32
{

namespace TypeId
{
	typedef struct{} Can1Rx;
	typedef struct{} Can1Tx;

	typedef struct{} Can2Rx;
	typedef struct{} Can2Tx;

	// for STM32F3
	typedef struct{} CanRx;
	typedef struct{} CanTx;

	// for STM32F7
	typedef struct{} Can3Rx;
	typedef struct{} Can3Tx;
}

} // namespace stm32

} // namespace xpcc

#endif // XPCC_STM32_CAN_TYPE_IDS_HPP
