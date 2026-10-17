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

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <algorithm>
#include <array>
#include <string_view>

namespace modm
{

/// @ingroup modm_driver_usb3803
struct usb3803
{
	/// Register addresses for the raw register access
	enum class
	Register : uint8_t
	{
		VidL = 0x00,
		VidM = 0x01,
		PidL = 0x02,
		PidM = 0x03,
		DidL = 0x04,
		DidM = 0x05,
		Cfg1 = 0x06,
		Cfg2 = 0x07,
		Cfg3 = 0x08,
		Nrd = 0x09,
		Pds = 0x0A,
		Pdb = 0x0B,
		MaxPs = 0x0C,
		MaxPb = 0x0D,
		HcMcs = 0x0E,
		HcMcb = 0x0F,
		PwrT = 0x10,
		LangIdH = 0x11,
		LangIdL = 0x12,
		MfrSl = 0x13,
		PrdSl = 0x14,
		SerSl = 0x15,
		MfrStr = 0x16,
		PrdStr = 0x54,
		SerStr = 0x92,
		BcEn = 0xD0,
		BattChg = 0xE2,
		PrtPwr = 0xE5,
		Ocs = 0xE6,
		SpIlock = 0xE7,
		IntStatus = 0xE8,
		IntMask = 0xE9,
		I2cAdd0 = 0xEA,
		I2cAdd1 = 0xEB,
		BchgMode = 0xEC,
		ChgDetMask = 0xED,
		Cfgp = 0xEE,
		VsnsUp3 = 0xF4,
		Vsns21 = 0xF5,
		BstUp3 = 0xF6,
		Bst21 = 0xF8,
		PrtSp = 0xFA,
		PrtR12 = 0xFB,
		PrtR34 = 0xFC,
		Stcd = 0xFF,
	};

	/// Downstream ports in the port bit masks. The upstream port is only
	/// valid for the DP/DM swap.
	enum class
	Port : uint8_t
	{
		Upstream = Bit0,
		Port1 = Bit1,
		Port2 = Bit2,
		Port3 = Bit3,
	};
	MODM_FLAGS8(Port);

	/// Events in INT_STATUS and INT_MASK
	enum class
	Interrupt : uint8_t
	{
		ChargerDetectionComplete = Bit0,
		ChargerDetected = Bit1,
		PortPower = Bit2,
		HubConfigured = Bit3,
		HubSuspend = Bit4,
	};
	MODM_FLAGS8(Interrupt);

	/// Result of the upstream battery charger detection in BATT_CHG[7:5]
	enum class
	ChargerType : uint8_t
	{
		NotComplete = 0b000,
		/// Dedicated charger, or with EnhancedChrgDet=0 also a charging port
		Dcp = 0b001,
		/// Charging downstream port, only with EnhancedChrgDet=1
		Cdp = 0b010,
		/// Standard downstream port of a host or hub
		Sdp = 0b011,
		Disabled = 0b111,
	};

	/// CFG1.CURRENT_SNS
	enum class
	CurrentSense : uint8_t
	{
		Ganged = 0b00,
		Individual = 0b01,
		/// Only allowed for bus-powered hubs
		None = 0b10,
	};

	/// USB descriptor strings
	enum class
	String : uint8_t
	{
		Manufacturer,
		Product,
		Serial,
	};
	/// Maximum length of a string in characters
	static constexpr std::size_t StringLength{31};

	/// Ports of the PHY tuning registers
	enum class
	PhyPort : uint8_t
	{
		Upstream,
		Port1,
		Port2,
		Port3,
	};

	/// Squelch threshold relative to the nominal value
	enum class
	Squelch : uint8_t
	{
		Nominal = 0b000,
		Percent90 = 0b001,
		Percent80 = 0b010,
		Percent70 = 0b011,
		Percent60 = 0b100,
		Percent50 = 0b101,
		Percent120 = 0b110,
		Percent110 = 0b111,
	};

	/// High-speed drive current relative to the nominal value
	enum class
	Boost : uint8_t
	{
		Nominal = 0b000,
		Minus5 = 0b001,
		Plus10 = 0b010,
		Plus5 = 0b011,
		Plus20 = 0b100,
		Plus15 = 0b101,
		Plus30 = 0b110,
		Plus25 = 0b111,
	};

	/// USB device descriptor fields idVendor, idProduct and bcdDevice
	struct Identity
	{
		uint16_t vid;
		uint16_t pid;
		uint16_t did;
	};

	/// Snapshot of the status and control registers, see `Usb3803::readStatus()`
	struct Status
	{
		uint8_t cfg1;
		uint8_t battChg;
		uint8_t prtPwr;
		uint8_t spIlock;
		uint8_t intStatus;
		uint8_t bchgMode;
		uint8_t cfgp;
		uint8_t stcd;

		constexpr bool selfPowered() const { return cfg1 & Cfg1SelfPowered; }
		constexpr ChargerType chargerType() const { return ChargerType(battChg >> 5); }
		constexpr bool chargerDetectionComplete() const { return battChg & BattChgComplete; }
		constexpr bool chargerDetectionRunning() const { return battChg & BattChgStartBcDet; }
		/// A charger not masked by CHGDETMASK was detected
		constexpr bool chargerDetected() const { return not (battChg & BattChgChgDetN); }
		/// Runs the charger detection automatically before connecting
		constexpr bool chargerDetectionEnabled() const { return bchgMode & BchgModeEnableChrgDet; }
		/// Tells a CDP apart from a DCP
		constexpr bool chargerDetectionEnhanced() const { return bchgMode & BchgModeEnhancedChrgDet; }
		/// Ports whose power the host enabled
		constexpr Port_t portPower() const { return Port_t(prtPwr & PortMaskDownstream); }
		constexpr bool configurationHeld() const { return spIlock & SpIlockConfigN; }
		constexpr bool connectHeld() const { return spIlock & SpIlockConnectN; }
		/// INT_N is asserted
		constexpr bool interruptAsserted() const { return intStatus & IntStatusInterrupt; }
		constexpr Interrupt_t interrupts() const { return Interrupt_t(intStatus & InterruptMaskAll); }
		/// CFGP.CfgTout, the configuration time-out THUBCONFIG
		constexpr uint8_t configurationTimeout() const { return (cfgp & CfgpCfgToutMask) >> 4; }
		constexpr bool configurationProtected() const { return stcd & StcdConfigProtect; }
	};

protected:
	/// @cond
	/// CFG1 bits
	static constexpr uint8_t Cfg1SelfPowered{Bit7};
	static constexpr uint8_t Cfg1HsDisable{Bit5};
	static constexpr uint8_t Cfg1MttEnable{Bit4};
	static constexpr uint8_t Cfg1EopDisable{Bit3};
	static constexpr uint8_t Cfg1CurrentSenseMask{0b11u << 1};
	static constexpr uint8_t Cfg1PortPowerIndividual{Bit0};

	/// CFG2 bits
	static constexpr uint8_t Cfg2Compound{Bit3};

	/// CFG3 bits
	static constexpr uint8_t Cfg3PortMapEnable{Bit3};
	static constexpr uint8_t Cfg3StringEnable{Bit0};

	/// SP_ILOCK bits
	static constexpr uint8_t SpIlockConfigN{Bit0};
	static constexpr uint8_t SpIlockConnectN{Bit1};

	/// INT_STATUS bit 7, set while the INT_N pin is asserted
	static constexpr uint8_t IntStatusInterrupt{Bit7};

	/// BATT_CHG bits
	static constexpr uint8_t BattChgComplete{Bit4};
	static constexpr uint8_t BattChgChgDetN{Bit1};
	static constexpr uint8_t BattChgStartBcDet{Bit0};

	/// BCHGMODE bits
	static constexpr uint8_t BchgModeEnableChrgDet{Bit4};
	static constexpr uint8_t BchgModeEnhancedChrgDet{Bit2};

	/// CHGDETMASK bits
	static constexpr uint8_t ChgDetMaskCdp{Bit1};
	static constexpr uint8_t ChgDetMaskDcp{Bit0};

	/// CFGP bits
	static constexpr uint8_t CfgpClkSusp{Bit7};
	static constexpr uint8_t CfgpIntSusp{Bit6};
	static constexpr uint8_t CfgpCfgToutMask{0b11u << 4};

	/// STCD bits
	static constexpr uint8_t StcdReset{Bit1};
	static constexpr uint8_t StcdConfigProtect{Bit0};

	/// Downstream ports in the port bit masks
	static constexpr uint8_t PortMaskDownstream{Bit3 | Bit2 | Bit1};
	/// All ports in the port bit masks
	static constexpr uint8_t PortMaskAll{Bit3 | Bit2 | Bit1 | Bit0};
	/// All events in INT_STATUS and INT_MASK
	static constexpr uint8_t InterruptMaskAll{Bit4 | Bit3 | Bit2 | Bit1 | Bit0};
	/// PHY tuning field of the first port in a register
	static constexpr uint8_t PhyMaskUpper{0b111u << 4};
	/// PHY tuning field of the second port in a register
	static constexpr uint8_t PhyMaskLower{0b111u << 0};
	/// @endcond
}; // struct usb3803

/**
 * Microchip USB3803 USB 2.0 hub.
 *
 * The configuration registers 00h-E1h and EFh-FFh only take effect when written
 * during the configuration stage, so hold it with `setInterlock(true)` within
 * 400ms after releasing RESET_N. The serial port only works until the hub
 * connects upstream and only while REFCLK is running, afterwards its behavior
 * is undefined. Hold the connect stage with `setInterlock(false, true)` to keep
 * accessing the control registers.
 *
 * @code
 * modm::Usb3803<MyI2cMaster> hub;
 * // within 400ms after releasing RESET_N
 * hub.setInterlock(true);
 * hub.setString(usb3803::String::Product, "My Hub");
 * hub.setStringsEnabled();
 * // the hub connects and the serial port is no longer available
 * hub.setInterlock(false);
 * @endcode
 *
 * @ingroup modm_driver_usb3803
 * @author Niklas Hauser
 */
template<class I2cMaster>
class Usb3803 : public usb3803, public modm::I2cDevice<I2cMaster>
{
public:
	/// @param	address	0x08 with I2C_ASEL[1:0] = 00, the LSB is I2C_ASEL0
	constexpr Usb3803(uint8_t address=0x08) :
		modm::I2cDevice<I2cMaster>(address)
	{}

	/// Keeps the hub in its configuration stage and/or before connecting
	/// upstream, so that the registers can be accessed without a deadline.
	/// Must first be written within the 400ms of the configuration stage.
	/// Holding the connect has no effect while the HUB_CONNECT pin is high.
	bool
	setInterlock(bool holdConfiguration, bool holdConnect=false)
	{
		return writeRegister(Register::SpIlock,
				(holdConfiguration ? SpIlockConfigN : 0) | (holdConnect ? SpIlockConnectN : 0));
	}

	/// Reads the USB device descriptor's VID, PID and bcdDevice
	bool
	readIdentity(Identity &identity)
	{
		std::array<uint8_t, 6> data;
		if (not readRegisters(Register::VidL, data.data(), data.size())) return false;
		identity.vid = data[0] | (data[1] << 8);
		identity.pid = data[2] | (data[3] << 8);
		identity.did = data[4] | (data[5] << 8);
		return true;
	}

	/// Changes the USB device descriptor's VID, PID and bcdDevice
	bool
	setIdentity(const Identity &identity)
	{
		const uint8_t data[6]{
			uint8_t(identity.vid), uint8_t(identity.vid >> 8),
			uint8_t(identity.pid), uint8_t(identity.pid >> 8),
			uint8_t(identity.did), uint8_t(identity.did >> 8)};
		return writeRegisters(Register::VidL, data, sizeof(data));
	}

	/// Reads the status and control registers
	bool
	readStatus(Status &status)
	{
		return readRegister(Register::Cfg1, status.cfg1) and
			readRegister(Register::BattChg, status.battChg) and
			readRegister(Register::PrtPwr, status.prtPwr) and
			readRegister(Register::SpIlock, status.spIlock) and
			readRegister(Register::IntStatus, status.intStatus) and
			readRegister(Register::BchgMode, status.bchgMode) and
			readRegister(Register::Cfgp, status.cfgp) and
			readRegister(Register::Stcd, status.stcd);
	}

	// Hub configuration ------------------------------------------------------
	/// A self-powered hub offers 500mA on every downstream port, a bus-powered
	/// one only 100mA.
	bool
	setSelfPowered(bool selfPowered=true)
	{ return updateBits(Register::Cfg1, Cfg1SelfPowered, selfPowered); }

	/// Only attaches as a full-speed hub
	bool
	setHighSpeedDisabled(bool disabled=true)
	{ return updateBits(Register::Cfg1, Cfg1HsDisable, disabled); }

	/// One transaction translator per port instead of one for all
	bool
	setMultiTransactionTranslator(bool enable=true)
	{ return updateBits(Register::Cfg1, Cfg1MttEnable, enable); }

	/// Disables the EOP generation at EOF1 in full-speed mode
	bool
	setEopDisabled(bool disabled=true)
	{ return updateBits(Register::Cfg1, Cfg1EopDisable, disabled); }

	/// Over-current sensing and port power switching reported to the host
	bool
	setPortPowerControl(CurrentSense sense, bool individualSwitching)
	{
		return updateRegister(Register::Cfg1, Cfg1CurrentSenseMask | Cfg1PortPowerIndividual,
				(uint8_t(sense) << 1) | (individualSwitching ? Cfg1PortPowerIndividual : 0));
	}

	/// The hub is part of a compound device, its permanently attached
	/// devices must also be set with `setNonRemovablePorts()`.
	bool
	setCompound(bool compound=true)
	{ return updateBits(Register::Cfg2, Cfg2Compound, compound); }

	/// Ports with permanently attached devices of a compound device
	bool
	setNonRemovablePorts(Port_t ports)
	{ return writeRegister(Register::Nrd, ports.value & PortMaskDownstream); }

	/// Disables ports for self-powered and bus-powered operation. The
	/// remaining ports are numbered contiguously, unless remapped.
	bool
	setDisabledPorts(Port_t selfPowered, Port_t busPowered)
	{
		return writeRegister(Register::Pds, selfPowered.value & PortMaskDownstream) and
			writeRegister(Register::Pdb, busPowered.value & PortMaskDownstream);
	}

	/// Maps the physical downstream ports to logical port numbers 1-3, or
	/// disables them with 0. Logical ports must be contiguous from 1.
	bool
	setPortRemap(uint8_t port1, uint8_t port2, uint8_t port3)
	{
		return writeRegister(Register::PrtR12, (port1 & 0xf) | ((port2 & 0xf) << 4)) and
			writeRegister(Register::PrtR34, port3 & 0xf) and
			updateBits(Register::Cfg3, Cfg3PortMapEnable, true);
	}

	/// Uses the port disable registers instead of the remapping again
	bool
	disablePortRemap()
	{ return updateBits(Register::Cfg3, Cfg3PortMapEnable, false); }

	/// Swaps DP and DM of the upstream and/or downstream ports
	bool
	setPortSwap(Port_t ports)
	{ return writeRegister(Register::PrtSp, ports.value & PortMaskAll); }

	/// Power reported in the configuration descriptor in mA, of the hub with
	/// all its circuitry, in 2mA steps. Limited to 100mA when self-powered and
	/// 500mA when bus-powered by the USB 2.0 specification.
	bool
	setMaxPower(uint16_t selfPowered, uint16_t busPowered)
	{
		return writeRegister(Register::MaxPs, std::min<uint16_t>(selfPowered, 100) / 2) and
			writeRegister(Register::MaxPb, std::min<uint16_t>(busPowered, 500) / 2);
	}

	/// Current of the hub controller alone in mA, reported in the hub status.
	/// Limited to 100mA when self-powered by the USB 2.0 specification.
	bool
	setHubControllerCurrent(uint8_t selfPowered, uint8_t busPowered)
	{
		return writeRegister(Register::HcMcs, std::min<uint8_t>(selfPowered, 100)) and
			writeRegister(Register::HcMcb, busPowered);
	}

	/// Time from switching a port's power on until it is good, in 2ms steps.
	/// Set to 0 without a power switch.
	bool
	setPowerOnTime(uint16_t ms)
	{ return writeRegister(Register::PwrT, std::min(ms / 2, 255)); }

	// String descriptors -----------------------------------------------------
	/// Strings are only reported after `setStringsEnabled()`
	bool
	setStringsEnabled(bool enable=true)
	{ return updateBits(Register::Cfg3, Cfg3StringEnable, enable); }

	/// USB language ID of the strings, the default is US English
	bool
	setLanguageId(uint16_t language=0x0409)
	{
		return writeRegister(Register::LangIdH, language >> 8) and
			writeRegister(Register::LangIdL, language);
	}

	/// Stores an ASCII string as UTF-16LE, truncated to 31 characters
	bool
	setString(String string, std::string_view text)
	{
		static constexpr Register address[]{Register::MfrStr, Register::PrdStr, Register::SerStr};
		static constexpr Register length[]{Register::MfrSl, Register::PrdSl, Register::SerSl};
		const std::size_t chars = std::min(text.size(), StringLength);
		std::array<uint8_t, StringLength * 2> data{};
		for (std::size_t ii = 0; ii < chars; ii++) data[2*ii] = text[ii];
		return writeRegisters(address[uint8_t(string)], data.data(), chars * 2) and
			writeRegister(length[uint8_t(string)], chars);
	}

	// Battery charging -------------------------------------------------------
	/// Enables the downstream battery charging support of these ports
	bool
	setDownstreamCharging(Port_t ports)
	{ return writeRegister(Register::BcEn, ports.value & PortMaskDownstream); }

	/// The automatic detection runs after the configuration stage, the
	/// enhanced detection also tells a CDP apart from a DCP.
	bool
	setChargerDetection(bool automatic, bool enhanced=true)
	{
		return updateRegister(Register::BchgMode, BchgModeEnableChrgDet | BchgModeEnhancedChrgDet,
				(automatic ? BchgModeEnableChrgDet : 0) | (enhanced ? BchgModeEnhancedChrgDet : 0));
	}

	/// Charger types that assert CHG_DET_N low, a CDP needs the enhanced detection
	bool
	setChargerDetectionMask(bool cdp, bool dcp)
	{
		// The reserved bits 3:2 default to 1 and must be kept
		return updateRegister(Register::ChgDetMask, ChgDetMaskCdp | ChgDetMaskDcp,
				(cdp ? ChgDetMaskCdp : 0) | (dcp ? ChgDetMaskDcp : 0));
	}

	/// Runs the upstream battery charger detection on USBUP_DP/DM, which is
	/// running while `Status::chargerDetectionRunning()`. Must not be started
	/// while the hub operates, only while its configuration is held.
	bool
	startChargerDetection()
	{ return writeRegister(Register::BattChg, BattChgStartBcDet); }

	// Interrupts and power ---------------------------------------------------
	/// Events that assert INT_N, they are captured in INT_STATUS regardless
	bool
	setInterruptMask(Interrupt_t interrupts)
	{ return writeRegister(Register::IntMask, interrupts.value); }

	/// Reads the captured events in INT_STATUS
	bool
	readInterrupts(Interrupt_t &interrupts)
	{
		uint8_t value;
		if (not readRegister(Register::IntStatus, value)) return false;
		interrupts = Interrupt_t(value & InterruptMaskAll);
		return true;
	}

	/// Clears the events and negates INT_N. The ChargerDetected event is set
	/// again while BATT_CHG.CHG_DET_N is low, so it is negated first.
	bool
	clearInterrupts(Interrupt_t interrupts=Interrupt_t(InterruptMaskAll))
	{
		if (interrupts & Interrupt::ChargerDetected and
			not writeRegister(Register::BattChg, BattChgChgDetN)) return false;
		return writeRegister(Register::IntStatus, ~interrupts.value & InterruptMaskAll);
	}

	/// Reports an over-current condition on these ports to the host and
	/// clears their port power status.
	bool
	setOverCurrent(Port_t ports)
	{ return writeRegister(Register::Ocs, ports.value & PortMaskDownstream); }

	/// Keeps the clock running during suspend for the I2C access, and/or
	/// turns INT_N into a level-sensitive suspend indicator
	bool
	setSuspendBehavior(bool clockRunning, bool interruptIsSuspend)
	{
		return updateRegister(Register::Cfgp, CfgpClkSusp | CfgpIntSusp,
				(clockRunning ? CfgpClkSusp : 0) | (interruptIsSuspend ? CfgpIntSusp : 0));
	}

	// PHY tuning -------------------------------------------------------------
	/// Squelch threshold of a port's receiver
	bool
	setSquelch(PhyPort port, Squelch squelch)
	{ return setPhyField(Register::VsnsUp3, Register::Vsns21, port, uint8_t(squelch)); }

	/// High-speed drive current of a port's transmitter
	bool
	setBoost(PhyPort port, Boost boost)
	{ return setPhyField(Register::BstUp3, Register::Bst21, port, uint8_t(boost)); }

	// Register control -------------------------------------------------------
	/// Resets the configuration registers 00h-E1h and EFh-FFh to their defaults,
	/// but not the control registers E2h-EEh.
	/// @warning Contrary to the datasheet, PDS, PDB and BC_EN keep their values
	///          (DID c1a2), so write them explicitly.
	bool
	resetRegisters()
	{ return writeRegister(Register::Stcd, StcdReset); }

	/// Write-protects the configuration registers 00h-E1h and EFh-FFh until
	/// RESET_N is asserted
	bool
	protectConfiguration()
	{ return writeRegister(Register::Stcd, StcdConfigProtect); }

public:
	bool
	readRegister(Register reg, uint8_t &value)
	{
		return readRegisters(reg, &value, 1);
	}

	/// Reads consecutive registers, the address increments automatically
	bool
	readRegisters(Register reg, uint8_t *data, std::size_t length)
	{
		const uint8_t address = uint8_t(reg);
		return this->writeRead(&address, 1, data, length);
	}

	bool
	writeRegister(Register reg, uint8_t value)
	{
		const uint8_t buffer[2]{uint8_t(reg), value};
		return this->write(buffer, 2);
	}

	/// Writes consecutive registers, at most 62
	bool
	writeRegisters(Register reg, const uint8_t *data, std::size_t length)
	{
		std::array<uint8_t, 1 + StringLength * 2> buffer;
		if (length > buffer.size() - 1) return false;
		buffer[0] = uint8_t(reg);
		std::copy_n(data, length, buffer.begin() + 1);
		return this->write(buffer.data(), length + 1);
	}

	/// Replaces the bits in `mask` with `value`
	bool
	updateRegister(Register reg, uint8_t mask, uint8_t value)
	{
		uint8_t current;
		if (not readRegister(reg, current)) return false;
		return writeRegister(reg, (current & ~mask) | (value & mask));
	}

protected:
	bool
	updateBits(Register reg, uint8_t mask, bool set)
	{ return updateRegister(reg, mask, set ? mask : 0); }

	/// The upstream port and port 3 share the first register, ports 2 and 1
	/// the second one, the first of each in bits 6:4.
	bool
	setPhyField(Register regUp3, Register reg21, PhyPort port, uint8_t value)
	{
		const bool upper = (port == PhyPort::Upstream or port == PhyPort::Port2);
		const Register reg = (port == PhyPort::Upstream or port == PhyPort::Port3) ? regUp3 : reg21;
		return updateRegister(reg, upper ? PhyMaskUpper : PhyMaskLower, (value & 0b111) << (upper ? 4 : 0));
	}
};

} // namespace modm
