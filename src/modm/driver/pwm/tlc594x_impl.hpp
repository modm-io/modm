/*
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2019, Linas Nikiperavicius
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TLC594X_HPP
#	error "Don't include this file directly, use 'tlc594x.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
void
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::initialize(uint16_t channels, uint8_t dots, bool writeCH, bool writeDC, bool enable)
{
	Xlat::setOutput(modm::Gpio::Low);
	Xblank::setOutput(modm::Gpio::High);
	Vprog::setOutput(modm::Gpio::Low);
	Xerr::setInput();

	if (channels != 0xffff) setAllChannels(channels);
	if (dots != 0xff) setAllDotCorrection(dots);

	if (writeCH) RF_CALL_BLOCKING(writeChannels());
	if (writeDC) RF_CALL_BLOCKING(writeDotCorrection());

	if (enable) this->enable();
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
void
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::latch()
{
	if (enabled) Xblank::set();
	Xlat::set();
	// datasheet says 20ns but that is unreliable
	// => wait for at least 1000ns
	modm::delay_us(1);
	Xlat::reset();
	if (enabled) Xblank::reset();
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
void
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::setChannel(uint16_t channel, uint16_t value)
{
	if (channel > CHANNELS-1) return;

	/* Grayscale PWM gs has the following format:
	 *      v =channel
	 *      1              0
	 * bbbbbbbbbbbb   aaaaaaaaaaaa
	 *   |       \     /      |
	 * bbbbbbbb  bbbbaaaa aaaaaaaa
	 *    0         1         2
	 *    ^ =index
	 *
	 * This pattern is repeated every 2 channels.
	 * The channel index relation is like this:
	 * channel index
	 *    0      1
	 *    1      0
	 */
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 1;

	if (channel & 0x01) {
		gs[index] = static_cast<uint8_t>(value >> 4);
		gs[index+1] = static_cast<uint8_t>(value << 4) | (gs[index+1] & 0x0f);
	} else {
		gs[index] = (static_cast<uint8_t>(value >> 8) & 0x0f) | (gs[index] & 0xf0);
		gs[index+1] = value;
	}
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
void
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::setAllChannels(uint16_t value)
{
	uint8_t first  = static_cast<uint8_t>(value >> 4);
	uint8_t second = static_cast<uint8_t>(value << 4) | (static_cast<uint8_t>(value >> 8) & 0x0f);

	for (uint_fast16_t i=0; i < CHANNELS*3/2;) {
		gs[i++] = first;
		gs[i++] = second;
		gs[i++] = static_cast<uint8_t>(value);
	}
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
uint16_t
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::getChannel(uint16_t channel)
{
	if (channel > CHANNELS-1) return 0;

	uint16_t value;
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 1;

	if (channel & 0x01) {
		value = gs[index];
		value = (value << 4) | (gs[index+1] >> 4);
	} else {
		value = (gs[index] & 0x0f);
		value = (value << 8) | gs[index+1];
	}
	return value;
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
void
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::setDotCorrection(uint16_t channel, uint8_t value)
{
	if (channel > CHANNELS-1) return;

	/* Dot Correction data has the following format:
	 *   v =channel
	 *   3      2      1      0
	 * dddddd cccccc bbbbbb aaaaaa
	 *   |       \      /     |
	 * ddddddcc  ccccbbbb bbaaaaaa
	 *    0         1        2
	 *    ^ =index
	 *
	 * This pattern is repeated every 4 channels.
	 * The channel index relation is like this:
	 * channel index
	 *    0      2
	 *    1      1
	 *    2      0
	 *    3      0
	 */
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 2;

	switch (channel & 0x03)
	{
		case 0:
			dc[index] = (dc[index] & 0xc0) | value;
			break;

		case 1:
			dc[index] = (dc[index] & 0xf0) | (value >> 2);
			dc[index+1] = (dc[index+1] & 0x3f) | (value << 6);
			break;

		case 2:
			dc[index] = (dc[index] & 0xfc) | (value >> 4);
			dc[index+1] = (dc[index+1] & 0x0f) | (value << 4);
			break;
//		case 3:
		default:
			dc[index] = (dc[index] & 0x03) | (value << 2);
			break;
	}
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
void
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::setAllDotCorrection(uint8_t value)
{
	uint8_t first  = (value << 2) | (value >> 4);
	uint8_t second = (value << 4) | (value >> 2);
	uint8_t third  = (value << 6) | value;

	for (uint_fast16_t i=0; i < CHANNELS*3/4;) {
		dc[i++] = first;
		dc[i++] = second;
		dc[i++] = third;
	}
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
uint8_t
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::getDotCorrection(uint16_t channel)
{
	if (channel > CHANNELS-1) return 0;

	uint8_t value;
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 2;

	switch (channel & 0x03)
	{
		case 0:
			value = dc[index] & 0x3f;
			break;

		case 1:
			value = ((dc[index] & 0x0f) << 2) | ((dc[index+1] & 0xc0) >> 6);
			break;

		case 2:
			value = ((dc[index] & 0x03) << 4) | ((dc[index+1] & 0xf0) >> 4);
			break;
//		case 3:
		default:
			value = (dc[index] & 0xfc) >> 2;
			break;
	}
	return value;
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
modm::ResumableResult<void>
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::writeChannels(bool flush)
{
	RF_BEGIN();

	RF_CALL(Spi::transfer(gs, status, CHANNELS*3/2));
	if (flush) latch();

	RF_END();
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Xblank, typename Vprog, typename Xerr>
modm::ResumableResult<void>
modm::TLC594X<CHANNELS, Spi, Xlat, Xblank, Vprog, Xerr>::writeDotCorrection(bool flush)
{
	RF_BEGIN();

	Vprog::set();

	// transfer
	RF_CALL(Spi::transfer(dc, nullptr, CHANNELS*3/4));
	if (flush) latch();

	Vprog::reset();

	RF_END();
}
