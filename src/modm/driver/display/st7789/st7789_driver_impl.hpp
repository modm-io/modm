/*
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "st7789_driver.hpp"

namespace modm
{

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::initialize()
{
	Interface::initialize();

	hardReset();

	softReset();
	modm::delay(10ms);

	Interface::sendCommand(Command::SleepOff);
	modm::delay(10ms);

	using detail::st7789::InterfacePixelFormat;
	InterfacePixelFormat ipf;
	ipf.set(InterfacePixelFormat::RGBColorFormat::RGB_65k);
	ipf.set(InterfacePixelFormat::DataColorFormat::RGB_565);
	Interface::sendCommand(Command::InterfacePixelFormat, ipf);

	Interface::sendCommand(Command::MemoryDataAccessControl, madctl_);
	Interface::sendCommand(Command::InverseOn);
	Interface::sendCommand(Command::NormalDisplayModeOn);

	Interface::sendCommand(Command::DisplayOn);
	modm::delay(10ms);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::clear(uint16_t color)
{
	setClipping(0, 0, Width, Height);

	Interface::beginCommand(Command::WriteDisplayData);
	Interface::switchToDataMode();
	for (size_t i = 0; i < Width * Height; ++i)
	{
		auto data = reinterpret_cast<const uint8_t *>(&color);
		Interface::continueData(data[1]);
		Interface::continueData(data[0]);
	}
	Interface::end();
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::setClipping(uint16_t x, uint16_t y, uint16_t width,
													uint16_t height)
{
	x += offset_x_;
	y += offset_y_;

	using SetColumnRow = detail::st7789::SetColumnRow;
	Interface::sendCommand(Command::SetColumn, SetColumnRow{x, (uint16_t)(x + width - 1)});
	Interface::sendCommand(Command::SetRow, SetColumnRow{y, (uint16_t)(y + height - 1)});
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::sendCommand(Command command)
{
	Interface::sendCommand(command);
}

template<typename Interface, uint16_t Width, uint16_t Height>
template<typename Data>
void
St7789Driver<Interface, Width, Height>::sendCommand(Command command, Data &&data)
{
	Interface::sendCommand(command, std::forward<Data>(data));
}

template<typename Interface, uint16_t Width, uint16_t Height>
template<St7789Driver<Interface, Width, Height>::ByteOrder OrderOfBytes>
void
St7789Driver<Interface, Width, Height>::writeData(data data)
{
	Interface::beginCommand(Command::WriteDisplayData);
	Interface::switchToDataMode();
	if constexpr (OrderOfBytes == ByteOrder::Passthrough)
	{
		Interface::continueData(data);
	} else
	{
		for (size_t i = 0; i < data.size() - 1; i += 2)
		{
			Interface::continueData(data[i + 1]);
			Interface::continueData(data[i + 0]);
		}
	}
	Interface::end();
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::hardReset()
{
	Interface::Reset::set();
	modm::delay(10ms);
	Interface::Reset::reset();
	modm::delay(100ms);
	Interface::Reset::set();
	modm::delay(10ms);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::softReset()
{
	Interface::sendCommand(Command::Reset);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::setInversion(bool inversion)
{
	Interface::sendCommand(inversion ? Command::InverseOn : Command::InverseOff);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::turnOn()
{
	Interface::sendCommand(Command::DisplayOn);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::turnOff()
{
	Interface::sendCommand(Command::DisplayOff);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::setIdle(bool idle)
{
	Interface::sendCommand(idle ? Command::IdleModeOn : Command::IdleModeOff);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::setSleep(bool sleep)
{
	Interface::sendCommand(sleep ? Command::SleepOn : Command::SleepOff);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::setRgbBgrOrder(RgbBgrOrder rgb_bgr_order)
{
	madctl_.set(rgb_bgr_order);
	Interface::sendCommand(Command::MemoryDataAccessControl, madctl_.data);
}

template<typename Interface, uint16_t Width, uint16_t Height>
void
St7789Driver<Interface, Width, Height>::setOrientation(Orientation orientation)
{

	using Control = MemoryDataAccessControl;

	switch (orientation_ = orientation)
	{
		case Orientation::Landscape0:
			offset_x_ = 0;
			offset_y_ = 0;
			madctl_.set(Control::PageColumnOrder::NormalMode);
			madctl_.set(Control::ColumnAddressOrder::LeftToRight);
			madctl_.set(Control::PageAddressOrder::TopToBottom);
			break;

		case Orientation::Portrait90:
			offset_x_ = 0;
			offset_y_ = 0;
			madctl_.set(Control::PageColumnOrder::ReverseMode);
			madctl_.set(Control::ColumnAddressOrder::RightToLeft);
			madctl_.set(Control::PageAddressOrder::TopToBottom);
			break;

		case Orientation::Landscape180:
			offset_x_ = detail::st7789::MaxWidth - Width;
			offset_y_ = detail::st7789::MaxHeight - Height;
			madctl_.set(Control::PageColumnOrder::NormalMode);
			madctl_.set(Control::ColumnAddressOrder::RightToLeft);
			madctl_.set(Control::PageAddressOrder::BottomToTop);
			break;

		case Orientation::Portrait270:
			offset_x_ = detail::st7789::MaxHeight - Height;
			offset_y_ = detail::st7789::MaxWidth - Width;
			madctl_.set(Control::PageColumnOrder::ReverseMode);
			madctl_.set(Control::ColumnAddressOrder::LeftToRight);
			madctl_.set(Control::PageAddressOrder::BottomToTop);
			break;
	}

	Interface::sendCommand(Command::MemoryDataAccessControl, madctl_.data);
}

template<typename Interface, uint16_t Width, uint16_t Height>
glcd::Orientation
St7789Driver<Interface, Width, Height>::getOrientation() const
{
	return orientation_;
}

}  // namespace modm
