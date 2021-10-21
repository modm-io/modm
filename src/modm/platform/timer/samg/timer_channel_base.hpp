/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include "../device.hpp"

namespace modm::platform
{

class TimerChannelBase
{
public:
	/// Effect of various events on the TioX output pins
	enum class TioEffect : uint32_t
	{
		None = 0,
		Set = 1,
		Clear = 2,
		Toggle = 3
	};

	/// External Event Selection values
	enum class ExtEventSource : uint32_t
	{
		Tiob = 0,
		Xc0 = 1,
		Xc1 = 2,
		Xc2 = 3,
	};

	/// Waveform mode selection values
	enum class WavSel : uint32_t
	{
		Up = 0,        // UP mode without automatic trigger on RC compare
		UpDown = 1,    // UPDOWN mode without automatic trigger on RC compare
		Up_Rc = 2,     // UP mode with automatic trigger on RC compare
		UpDown_Rc = 3  // UPDOWN mode with automatic trigger on RC compare
	};

	/// Input clock selection values
	enum class ClockSource : uint32_t
	{
		MckDiv2 = 0,
		MckDiv8 = 1,
		MckDiv32 = 2,
		MckDiv128 = 3,
		Pck3 = 4,
		Xc0 = 5,
		Xc1 = 6,
		Xc2 = 7
	};

	/// Interrupt flags
	enum class Interrupt : uint32_t
	{
		CounterOverflow = TC_IER_COVFS,
		LoadOverrun = TC_IER_LOVRS,
		RaCompare = TC_IER_CPAS,
		RbCompare = TC_IER_CPBS,
		RcCompare = TC_IER_CPCS,
		RaLoading = TC_IER_LDRAS,
		RbLoading = TC_IER_LDRBS,
		ExternalTrigger = TC_IER_ETRGS,
		EndOfRxTransfer = TC_IER_ENDRX,
		RxBufferFull = TC_IER_RXBUFF,
	};
	MODM_FLAGS32(Interrupt);

	// The CMR register has dual definitions: one for capture mode and one for
	// waveform mode. However it seems that only the capture mode bit definitions
	// made it into the CMSIS device header. Here, the missing waveform mode
	// definitions are defined as needed.
	static const uint32_t TC_CMR_EEVT_Pos = 10;
	static const uint32_t TC_CMR_EEVT_Msk = (0x3) << TC_CMR_EEVT_Pos;
	static const uint32_t TC_CMR_WAVSEL_Pos = 13;
	static const uint32_t TC_CMR_WAVSEL_Msk = (0x3) << TC_CMR_WAVSEL_Pos;
	static const uint32_t TC_CMR_ACPA_Pos = 16;
	static const uint32_t TC_CMR_ACPA_Msk = (0x3) << TC_CMR_ACPA_Pos;
	static const uint32_t TC_CMR_ACPC_Pos = 18;
	static const uint32_t TC_CMR_ACPC_Msk = (0x3) << TC_CMR_ACPC_Pos;
	static const uint32_t TC_CMR_AEEVT_Pos = 20;
	static const uint32_t TC_CMR_AEEVT_Msk = (0x3) << TC_CMR_AEEVT_Pos;
	static const uint32_t TC_CMR_ASWTRG_Pos = 22;
	static const uint32_t TC_CMR_ASWTRG_Msk = (0x3) << TC_CMR_ASWTRG_Pos;
	static const uint32_t TC_CMR_BCPB_Pos = 24;
	static const uint32_t TC_CMR_BCPB_Msk = (0x3) << TC_CMR_BCPB_Pos;
	static const uint32_t TC_CMR_BCPC_Pos = 26;
	static const uint32_t TC_CMR_BCPC_Msk = (0x3) << TC_CMR_BCPC_Pos;
	static const uint32_t TC_CMR_BEEVT_Pos = 28;
	static const uint32_t TC_CMR_BEEVT_Msk = (0x3) << TC_CMR_BEEVT_Pos;
	static const uint32_t TC_CMR_BSWTRG_Pos = 30;
	static const uint32_t TC_CMR_BSWTRG_Msk = (0x3) << TC_CMR_BSWTRG_Pos;
};

}  // namespace modm::platform