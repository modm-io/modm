// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "drv832x_spi.hpp"

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::FaultStatus1_t& c) {
	os << "FaultStatus1(";
	if(c & drv832xSpi::FaultStatus1::Fault)
		os << "Fault ";
	if(c & drv832xSpi::FaultStatus1::GateDriveFault)
		os << "GateDriveFault ";
	if(c & drv832xSpi::FaultStatus1::OvertemperatureShutdown)
		os << "OvertemperatureShutdown ";
	if(c & drv832xSpi::FaultStatus1::UndervoltageLockout)
		os << "UndervoltageLockout ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentFault)
		os << "VdsOvercurrentFault ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentHA)
		os << "VdsOvercurrentHA ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentLA)
		os << "VdsOvercurrentLA ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentHB)
		os << "VdsOvercurrentHB ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentLB)
		os << "VdsOvercurrentLB ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentHC)
		os << "VdsOvercurrentHC ";
	if(c & drv832xSpi::FaultStatus1::VdsOvercurrentLC)
		os << "VdsOvercurrentLC ";
	os << ")";
	return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::VgsStatus2_t& c) {
	os << "VgsStatus2(";
	if(c & drv832xSpi::VgsStatus2::ChargePumpUndervoltageFault)
		os << "ChargePumpUndervoltageFault ";
	if(c & drv832xSpi::VgsStatus2::OvertemperatureWarning)
		os << "OvertemperatureWarning ";
	if(c & drv832xSpi::VgsStatus2::SenseAOvercurrent)
		os << "SenseAOvercurrent ";
	if(c & drv832xSpi::VgsStatus2::SenseBOvercurrent)
		os << "SenseBOvercurrent ";
	if(c & drv832xSpi::VgsStatus2::SenseCOvercurrent)
		os << "SenseCOvercurrent ";
	if(c & drv832xSpi::VgsStatus2::VgsGateDriveFaultHA)
		os << "VgsGateDriveFaultHA ";
	if(c & drv832xSpi::VgsStatus2::VgsGateDriveFaultHB)
		os << "VgsGateDriveFaultHB ";
	if(c & drv832xSpi::VgsStatus2::VgsGateDriveFaultHC)
		os << "VgsGateDriveFaultHC ";
	if(c & drv832xSpi::VgsStatus2::VgsGateDriveFaultLA)
		os << "VgsGateDriveFaultLA ";
	if(c & drv832xSpi::VgsStatus2::VgsGateDriveFaultLB)
		os << "VgsGateDriveFaultLB ";
	if(c & drv832xSpi::VgsStatus2::VgsGateDriveFaultLC)
		os << "VgsGateDriveFaultLC ";
	os << ")";
	return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::DriverControl_t& c) {
	os << "DriverControl(";
	if(c & drv832xSpi::DriverControl::Brake)
		os << "Brake ";
	if(c & drv832xSpi::DriverControl::ClearFaultBits)
		os << "ClearFaultBits ";
	if(c & drv832xSpi::DriverControl::Coast)
		os << "Coast ";
	if(c & drv832xSpi::DriverControl::DisableChargePumpUVLO)
		os << "DisableChargePumpUVLO ";
	if(c & drv832xSpi::DriverControl::DisableGateDriveFault)
		os << "DisableGateDriveFault ";
	if(c & drv832xSpi::DriverControl::OvertemperatureReport)
		os << "OvertemperatureReport ";
	if(c & drv832xSpi::DriverControl::Pwm1Com)
		os << "Pwm1Com ";
	if(c & drv832xSpi::DriverControl::Pwm1Dir)
		os << "Pwm1Dir ";
	if(c & drv832xSpi::DriverControl::PwmMode1)
		os << "PwmMode1 ";
	if(c & drv832xSpi::DriverControl::PwmMode2)
		os << "PwmMode2 ";
	os << ")";
	return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::GateDriveHS_t& c) {
	os << "GateDriveHS(";
	if(c & drv832xSpi::GateDriveHS::IDriveN0)
		os << "IDriveN0 ";
	if(c & drv832xSpi::GateDriveHS::IDriveN1)
		os << "IDriveN1 ";
	if(c & drv832xSpi::GateDriveHS::IDriveN2)
		os << "IDriveN2 ";
	if(c & drv832xSpi::GateDriveHS::IDriveN3)
		os << "IDriveN3 ";
	if(c & drv832xSpi::GateDriveHS::IDriveP0)
		os << "IDriveP0 ";
	if(c & drv832xSpi::GateDriveHS::IDriveP1)
		os << "IDriveP1 ";
	if(c & drv832xSpi::GateDriveHS::IDriveP2)
		os << "IDriveP2 ";
	if(c & drv832xSpi::GateDriveHS::IDriveP3)
		os << "IDriveP3 ";
	if(c & drv832xSpi::GateDriveHS::Lock0)
		os << "Lock0 ";
	if(c & drv832xSpi::GateDriveHS::Lock1)
		os << "Lock1 ";
	if(c & drv832xSpi::GateDriveHS::Lock2)
		os << "Lock2 ";
	os << ")";
	return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::GateDriveLS_t& c) {
	os << "GateDriveLS(";
	if(c & drv832xSpi::GateDriveLS::Cbc)
		os << "Cbc ";
	if(c & drv832xSpi::GateDriveLS::IDriveN0)
		os << "IDriveN0 ";
	if(c & drv832xSpi::GateDriveLS::IDriveN1)
		os << "IDriveN1 ";
	if(c & drv832xSpi::GateDriveLS::IDriveN2)
		os << "IDriveN2 ";
	if(c & drv832xSpi::GateDriveLS::IDriveN3)
		os << "IDriveN3 ";
	if(c & drv832xSpi::GateDriveLS::IDriveP0)
		os << "IDriveP0 ";
	if(c & drv832xSpi::GateDriveLS::IDriveP1)
		os << "IDriveP1 ";
	if(c & drv832xSpi::GateDriveLS::IDriveP2)
		os << "IDriveP2 ";
	if(c & drv832xSpi::GateDriveLS::IDriveP3)
		os << "IDriveP3 ";
	if(c & drv832xSpi::GateDriveLS::TDrive0)
		os << "TDrive0 ";
	if(c & drv832xSpi::GateDriveLS::TDrive1)
		os << "TDrive1 ";
	os << ")";
	return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::OcpControl_t& c) {
	os << "OcpControl(";
	if(c & drv832xSpi::OcpControl::Deadtime0)
		os << "Deadtime0 ";
	if(c & drv832xSpi::OcpControl::Deadtime1)
		os << "Deadtime1 ";
	if(c & drv832xSpi::OcpControl::OcpDeglitch0)
		os << "OcpDeglitch0 ";
	if(c & drv832xSpi::OcpControl::OcpDeglitch1)
		os << "OcpDeglitch1 ";
	if(c & drv832xSpi::OcpControl::OcpMode0)
		os << "OcpMode0 ";
	if(c & drv832xSpi::OcpControl::OcpMode1)
		os << "OcpMode1 ";
	if(c & drv832xSpi::OcpControl::TimeRetry)
		os << "TimeRetry ";
	if(c & drv832xSpi::OcpControl::VdsLevel0)
		os << "VdsLevel0 ";
	if(c & drv832xSpi::OcpControl::VdsLevel1)
		os << "VdsLevel1 ";
	if(c & drv832xSpi::OcpControl::VdsLevel2)
		os << "VdsLevel2 ";
	if(c & drv832xSpi::OcpControl::VdsLevel3)
		os << "VdsLevel3 ";
	os << ")";
	return os;
}

modm::IOStream&
modm::operator << (modm::IOStream& os, const drv832xSpi::CsaControl_t& c) {
	os << "CsaControl(";
	if(c & drv832xSpi::CsaControl::CsaCalibrationA)
		os << "CsaCalibrationA ";
	if(c & drv832xSpi::CsaControl::CsaCalibrationB)
		os << "CsaCalibrationB ";
	if(c & drv832xSpi::CsaControl::CsaCalibrationC)
		os << "CsaCalibrationC ";
	if(c & drv832xSpi::CsaControl::CsaFet)
		os << "CsaFet ";
	if(c & drv832xSpi::CsaControl::CsaGain0)
		os << "CsaGain0 ";
	if(c & drv832xSpi::CsaControl::CsaGain1)
		os << "CsaGain1 ";
	if(c & drv832xSpi::CsaControl::DisableOvercurrentSense)
		os << "DisableOvercurrentSense ";
	if(c & drv832xSpi::CsaControl::LowSideReference)
		os << "LowSideReference ";
	if(c & drv832xSpi::CsaControl::SenseOcpLevel0)
		os << "SenseOcpLevel0 ";
	if(c & drv832xSpi::CsaControl::SenseOcpLevel1)
		os << "SenseOcpLevel1 ";
	if(c & drv832xSpi::CsaControl::VrefDiv2)
		os << "VrefDiv2 ";
	os << ")";
	return os;
}
