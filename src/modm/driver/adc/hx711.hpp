/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HX711_HPP
#define MODM_HX711_HPP

#include <modm/architecture/interface/delay.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/// @ingroup modm_driver_hx711
struct hx711
{
	enum class
	InputChannelAndGain : uint8_t
	{
		ChA_128 = 1,
		ChB_32 = 2,
		ChA_64 = 3
	};

	struct Config
	{
		using Sck = void; // required
		using Data = void; // required
		static const InputChannelAndGain mode = InputChannelAndGain::ChA_128;
	};
};

template <typename Cfg>
class Hx711 : public hx711, protected modm::NestedResumable<2>
{
	using Sck = typename Cfg::Sck;
	using Data = typename Cfg::Data;

public:
	modm::ResumableResult<int32_t>
	singleConversion();

private:
	int32_t data;
};

} // modm namespace

#include "hx711_impl.hpp"

#endif // MODM_HX711_HPP
