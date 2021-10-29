/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "buffer.hpp"

namespace modm::graphic
{
template<color::Color C, size_t N, class Canvas>
class Layout
{
private:
	BufferInterface<C> *buffers[N];
}
}  // namespace modm::graphic