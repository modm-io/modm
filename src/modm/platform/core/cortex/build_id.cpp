/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/build_id.hpp>

typedef struct {
    uint32_t namesz;
    uint32_t descsz;
    uint32_t type;
    uint8_t data[30];
} ElfNoteSection_t;
extern "C" const ElfNoteSection_t __build_id;

namespace modm
{

std::span<const uint8_t, 20>
build_id()
{
    return std::span<const uint8_t, 20>(&__build_id.data[__build_id.namesz], 20);
}

}
