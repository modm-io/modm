// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Common Peripheral Macros
 * ========================
 *
 * This file needs to be included before any peripheral driver.
 * Somewhere in the architecture.hpp header file magic...
 **/


/**
 * Makes it possible to use enum classes as bit flags.
 * This disables some typesafety introduced by the enum class feature
 * but it should still be more typesafe than using the traditional
 * enum type.
 * Use (for public enum classes after class definition) like this:
 * ENUM_CLASS_FLAG(Adc{{ id }}::InterruptFlag)
 */
#define ENUM_CLASS_FLAG(name) \
inline name operator|(name a, name b) \
{return static_cast<name>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));} \
inline uint32_t operator&(name a, name b) \
{return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b));} \
inline uint32_t operator&(uint32_t a, name b) \
{return ((a) & static_cast<uint32_t>(b));} \
inline uint32_t operator&(name a, uint32_t b) \
{return (static_cast<uint32_t>(a) & (b));}
