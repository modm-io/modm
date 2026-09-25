/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "st7735_driver.hpp"

namespace modm
{

template<typename Interface>
void
St7735Driver<Interface>::initialize()
{
	Interface::initialize();

	// The WeAct panel has no independently controllable reset pin. Its vendor
	// sequence issues two software resets and waits for the ST7735S reset time.
	softReset();
	modm::this_fiber::sleep_for(120ms);
	softReset();
	modm::this_fiber::sleep_for(120ms);

	Interface::sendCommand(Command::SleepOff);
	modm::this_fiber::sleep_for(120ms);

	Interface::sendCommand(Command::FrameRateControlNormal, profile_.frameRateNormal);
	Interface::sendCommand(Command::FrameRateControlIdle, profile_.frameRateIdle);
	Interface::sendCommand(Command::FrameRateControlPartial, profile_.frameRatePartial);
	Interface::sendCommand(Command::FrameInversionControl, profile_.frameInversion);
	Interface::sendCommand(Command::PowerControl1, profile_.powerControl1);
	Interface::sendCommand(Command::PowerControl2, profile_.powerControl2);
	Interface::sendCommand(Command::PowerControl3, profile_.powerControl3);
	Interface::sendCommand(Command::PowerControl4, profile_.powerControl4);
	Interface::sendCommand(Command::PowerControl5, profile_.powerControl5);
	Interface::sendCommand(Command::VcomControl1, profile_.vcomControl);
	Interface::sendCommand(profile_.inverted ? Command::InverseOn : Command::InverseOff);

	constexpr uint8_t rgb565 = 0x05;
	Interface::sendCommand(Command::InterfacePixelFormat, rgb565);
	Interface::sendCommand(Command::PositiveGammaControl, profile_.positiveGamma);
	Interface::sendCommand(Command::NegativeGammaControl, profile_.negativeGamma);
	Interface::sendCommand(Command::NormalDisplayModeOn);
	setOrientation(orientation_);
	Interface::sendCommand(Command::DisplayOn);
}

template<typename Interface>
void
St7735Driver<Interface>::clear(uint16_t color)
{
	setClipping(0, 0, getWidth(), getHeight());
	Interface::beginCommand(Command::WriteDisplayData);
	Interface::switchToDataMode();
	const uint8_t high = static_cast<uint8_t>(color >> 8);
	const uint8_t low = static_cast<uint8_t>(color);
	for (size_t index = 0; index < static_cast<size_t>(getWidth()) * getHeight(); ++index)
	{
		Interface::continueData(high);
		Interface::continueData(low);
	}
	Interface::end();
}

template<typename Interface>
void
St7735Driver<Interface>::setClipping(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	if (width == 0 || height == 0) { return; }

	const auto &orientation = profile_.orientations[detail::st7735::orientationIndex(orientation_)];
	x += orientation.offsetX;
	y += orientation.offsetY;

	using SetColumnRow = detail::st7735::SetColumnRow;
	Interface::sendCommand(Command::SetColumn,
						   SetColumnRow{x, static_cast<uint16_t>(x + width - 1)});
	Interface::sendCommand(Command::SetRow, SetColumnRow{y, static_cast<uint16_t>(y + height - 1)});
}

template<typename Interface>
void
St7735Driver<Interface>::writeColor(uint16_t color)
{
	Interface::beginCommand(Command::WriteDisplayData);
	Interface::switchToDataMode();
	Interface::continueData(static_cast<uint8_t>(color >> 8));
	Interface::continueData(static_cast<uint8_t>(color));
	Interface::end();
}

template<typename Interface>
void
St7735Driver<Interface>::setOrientation(Orientation orientation)
{
	orientation_ = orientation;
	const auto &configuration =
		profile_.orientations[detail::st7735::orientationIndex(orientation_)];
	const uint8_t madctl = configuration.madctl | (bgr_ ? 0x08 : 0x00);
	Interface::sendCommand(Command::MemoryDataAccessControl, madctl);
	setClipping(0, 0, getWidth(), getHeight());
}

template<typename Interface>
glcd::Orientation
St7735Driver<Interface>::getOrientation() const
{ return orientation_; }

template<typename Interface>
uint16_t
St7735Driver<Interface>::getWidth() const
{
	switch (orientation_)
	{
		case Orientation::Portrait90:
		case Orientation::Portrait270:
			return profile_.landscapeHeight;
		default:
			return profile_.landscapeWidth;
	}
}

template<typename Interface>
uint16_t
St7735Driver<Interface>::getHeight() const
{
	switch (orientation_)
	{
		case Orientation::Portrait90:
		case Orientation::Portrait270:
			return profile_.landscapeWidth;
		default:
			return profile_.landscapeHeight;
	}
}

template<typename Interface>
void
St7735Driver<Interface>::sendCommand(Command command)
{ Interface::sendCommand(command); }

template<typename Interface>
template<typename Data>
void
St7735Driver<Interface>::sendCommand(Command command, const Data &data)
{ Interface::sendCommand(command, data); }

template<typename Interface>
template<St7735Driver<Interface>::ByteOrder OrderOfBytes>
void
St7735Driver<Interface>::writeData(data data)
{
	Interface::beginCommand(Command::WriteDisplayData);
	Interface::switchToDataMode();
	if constexpr (OrderOfBytes == ByteOrder::Passthrough)
	{
		Interface::continueData(data);
	} else
	{
		for (size_t index = 0; index + 1 < data.size(); index += 2)
		{
			Interface::continueData(data[index + 1]);
			Interface::continueData(data[index]);
		}
	}
	Interface::end();
}

template<typename Interface>
void
St7735Driver<Interface>::hardReset()
{
	Interface::Reset::reset();
	modm::this_fiber::sleep_for(10us);
	Interface::Reset::set();
	modm::this_fiber::sleep_for(5ms);
}

template<typename Interface>
void
St7735Driver<Interface>::softReset()
{ Interface::sendCommand(Command::Reset); }

template<typename Interface>
void
St7735Driver<Interface>::turnOn()
{ Interface::sendCommand(Command::DisplayOn); }

template<typename Interface>
void
St7735Driver<Interface>::turnOff()
{ Interface::sendCommand(Command::DisplayOff); }

template<typename Interface>
void
St7735Driver<Interface>::setInversion(bool inversion)
{ Interface::sendCommand(inversion ? Command::InverseOn : Command::InverseOff); }

template<typename Interface>
void
St7735Driver<Interface>::setRgbBgrOrder(bool bgr)
{
	bgr_ = bgr;
	setOrientation(orientation_);
}

}  // namespace modm
