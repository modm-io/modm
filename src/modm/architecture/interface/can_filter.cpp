/*
 * Copyright (c) 2020, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/architecture/interface/can_filter.hpp>

namespace modm::can
{

modm::IOStream&
operator<<(modm::IOStream& s, const modm::can::Filter f)
{
	if(f.mode == Filter::Mode::ClassicMask) {
		s.printf("id=%04lx,mask=%04lx", f.identifier, f.mask);
	}
	else { // if(f.mode == Filter::Mode::DualIdentifier) 
		s.printf("id=%04lx,id2=%04lx", f.identifier, f.mask);
	}
	if(f.extended) {
		s << ",extended";
	}
	else {
		s << ",standard";
	}
	return s;
}

}
