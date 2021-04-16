/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2012-2013, Sascha Schade
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SIEMENS_S75_HPP
#define MODM_SIEMENS_S75_HPP

#include <modm/architecture/interface/delay.hpp>
#include <modm/driver/bus/bitbang_memory_interface.hpp>
#include <modm/ui/display/monochrome_graphic_display_vertical.hpp>

namespace modm
{
	/// @ingroup modm_driver_siemens_s75
	enum class Orientation : uint8_t
	{
		Portrait, 				//< Connector top
		LandscapeRight,			//< Connector right
		LandscapeLeft,			//< Connector left
		PortraitUpsideDown,		//< Connector bottom
	};

	/// @ingroup modm_driver_siemens_s75
	template <
		typename MEMORY,
		typename RESET,
		uint16_t  WIDTH,
		uint16_t  HEIGHT,
		modm::Orientation ORIENTATION>
	class SiemensS75Common :
			public MonochromeGraphicDisplayVertical<WIDTH, HEIGHT>
	{
	public:
		SiemensS75Common(MEMORY& interface) :
			interface(interface)
		{
		}

		void
		update(void);

		void
		initialize(void);

	protected:
		modm_always_inline void
		lcdCls(const uint16_t colour);

		modm_always_inline void
		lcdSettings();

	private:
		MEMORY& interface;
	};

	/// @ingroup modm_driver_siemens_s75
	template <typename MEMORY, typename RESET>
	class SiemensS75Portrait :
		public SiemensS75Common<MEMORY, RESET, 136, 176, modm::Orientation::Portrait>
	{
	};

	/// @ingroup modm_driver_siemens_s75
	template <typename MEMORY, typename RESET>
	class SiemensS75PortraitUpsideDown:
		public SiemensS75Common<MEMORY, RESET, 136, 176, modm::Orientation::PortraitUpsideDown>
	{
	};

	/**
	 * The display in landscape mode does not match the required
	 * alignment of GraphicDisplay which requests that
	 * the vertical resolution can be divided by 8.
	 *
	 * @ingroup modm_driver_siemens_s75
	 */
	template <typename MEMORY, typename RESET>
	class SiemensS75LandscapeLeft :
		public SiemensS75Common<MEMORY, RESET, 176, 136, modm::Orientation::LandscapeLeft>
	{
	public:
		explicit SiemensS75LandscapeLeft(MEMORY& interface) :
		SiemensS75Common<MEMORY, RESET, 176, 136, modm::Orientation::LandscapeLeft>(interface) { }
	};

	/// @ingroup modm_driver_siemens_s75
	template <typename MEMORY, typename RESET>
	class SiemensS75LandscapeRight :
		public SiemensS75Common<MEMORY, RESET, 176, 136, modm::Orientation::LandscapeRight>
	{
	public:
		explicit SiemensS75LandscapeRight(MEMORY& interface) :
		SiemensS75Common<MEMORY, RESET, 176, 136, modm::Orientation::LandscapeRight>(interface) { }
	};
}

#include "siemens_s75_impl.hpp"

#endif // MODM_SIEMENS_S75_HPP
