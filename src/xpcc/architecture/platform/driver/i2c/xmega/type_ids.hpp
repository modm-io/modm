/*
 * Copyright (c) 2013-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_XMEGA_I2C_TYPE_IDS_HPP
#define MODM_XMEGA_I2C_TYPE_IDS_HPP

namespace modm
{

namespace xmega
{

namespace TypeId
{
	typedef struct{} I2cMasterCSda;
	typedef struct{} I2cMasterCScl;

	typedef struct{} I2cMasterDSda;
	typedef struct{} I2cMasterDScl;

	typedef struct{} I2cMasterESda;
	typedef struct{} I2cMasterEScl;

	typedef struct{} I2cMasterFSda;
	typedef struct{} I2cMasterFScl;
}

} // namespace xmega

} // namespace modm

#endif // MODM_XMEGA_I2C_TYPE_IDS_HPP
