// coding: utf-8
/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "ma12070p.hpp"

modm::IOStream&
modm::operator << (modm::IOStream& os, const ma12070p::VlpMonitor_t& c)
{
    os << "VlpMonitor(";
    if(c & ma12070p::VlpMonitor::LimiterCh0L)
        os << "LimiterCh0L ";
    if(c & ma12070p::VlpMonitor::LimiterCh0R)
        os << "LimiterCh0R ";
    if(c & ma12070p::VlpMonitor::LimiterCh1L)
        os << "LimiterCh1L ";
    if(c & ma12070p::VlpMonitor::LimiterCh1R)
        os << "LimiterCh1R ";
    if(c & ma12070p::VlpMonitor::ClippingCh0L)
        os << "ClippingCh0L ";
    if(c & ma12070p::VlpMonitor::ClippingCh0R)
        os << "ClippingCh0R ";
    if(c & ma12070p::VlpMonitor::ClippingCh1L)
        os << "ClippingCh1L ";
    if(c & ma12070p::VlpMonitor::ClippingCh1R)
        os << "ClippingCh1R ";
    os << ")";
    return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const ma12070p::ErrorRegister_t& c)
{
    os << "ErrorRegister(";
    if(c & ma12070p::ErrorRegister::FlyingCapOverVolt)
        os << "FlyingCapOverVolt ";
    if(c & ma12070p::ErrorRegister::OverCurrent)
        os << "OverCurrent ";
    if(c & ma12070p::ErrorRegister::Pll)
        os << "Pll ";
    if(c & ma12070p::ErrorRegister::PvddUnderVolt)
        os << "PvddUnderVolt ";
    if(c & ma12070p::ErrorRegister::OverTempWarning)
        os << "OverTempWarning ";
    if(c & ma12070p::ErrorRegister::OverTempError)
        os << "OverTempError ";
    if(c & ma12070p::ErrorRegister::PinToPinLowImpedance)
        os << "PinToPinLowImpedance ";
    if(c & ma12070p::ErrorRegister::DcProtection)
        os << "DcProtection ";
    os << ")";
    return os;
}
