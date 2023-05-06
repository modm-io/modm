/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "macros.hpp"
#include <stdint.h>

#define MODM_RESUMABLE_IS_FIBER

namespace modm
{

/// @ingroup	modm_processing_resumable
/// @{

/// Resumable functions implemented via fibers return like a normal function
template < typename T >
using ResumableResult = T;

/// Resumable functions implemented via fibers are normal functions
template< uint8_t Functions = 0 >
struct Resumable
{
    // unimplementable with fibers, but may be stubbed by user application
    void stopAllResumables();
    bool stopResumable(uint8_t id);
    bool isResumableRunning(uint8_t id) const;
    bool areAnyResumablesRunning() const;
    bool areAnyResumablesRunning(std::initializer_list<uint8_t> ids) const;
    bool areAllResumablesRunning(std::initializer_list<uint8_t> ids) const;
    bool joinResumables(std::initializer_list<uint8_t> ids) const;
};

/// Resumable functions implemented via fibers are normal functions
template< uint8_t Levels = 0 >
struct NestedResumable
{
    // unimplementable with fibers, but may be stubbed by user application
    void stopResumable();
    bool isResumableRunning() const;
    int8_t getResumableDepth() const;
};

/// @}

} // namespace modm
