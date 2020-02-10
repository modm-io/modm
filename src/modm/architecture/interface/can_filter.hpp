/*
 * Copyright (c) 2020, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_CAN_FILTER_HPP
#define MODM_CAN_FILTER_HPP

#include <modm/io/iostream.hpp>

namespace modm::can
{

/// Representation of a CAN filter
/// @ingroup modm_architecture_can
struct Filter
{
	Filter(uint32_t identifier_, uint32_t mask_, bool extended_=false, Mode mode_=Mode::ClassicMask) :
		identifier{ identifier_ },
		mask{ mask_ },
		mode{ _mode },
		extended{ extended_ }
	{
	}

	enum class Mode : uint8_t {
		ClassicMask,
		DualIdentifier,
		//Range, // only supported on STM32 FDCAN IP
	}

public:
	uint32_t identifier;
	uint32_t mask;
	Mode mode;
	bool extended;
};

modm::IOStream&
operator << (modm::IOStream& s, const modm::can::Filter f);

}

#endif
