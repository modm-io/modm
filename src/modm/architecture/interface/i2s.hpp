/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2021, Marton Ledneczki
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2S_HPP
#define MODM_INTERFACE_I2S_HPP

#include <modm/architecture/interface/peripheral.hpp>

namespace modm
{

/// @ingroup modm_architecture_i2s
struct I2s
{
	/// The signature of the configuration function.
	using ConfigurationHandler = void(*)();
};

} // namespace modm

#endif // MODM_INTERFACE_I2S_HPP
