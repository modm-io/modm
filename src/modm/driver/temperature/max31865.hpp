// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MAX31865_HPP
#define MODM_MAX31865_HPP

#include <array>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/processing/timer.hpp>

namespace modm
{

/// @ingroup modm_driver_max31865
struct max31865
{
	struct Pt
	{
		float R0;
		float Rref;
		float alpha;
		double a;
		double b;
		double c;
	};

	/// Standard values for IEC 751 (PT100) with 430 Ohm reference
	static constexpr Pt pt100{
		.R0 = 100.f,
		.Rref = 430.f,
		.alpha = 3.85055e-3f,
		.a = double{3.90830e-3},
		.b = double{-5.77500e-7},
		.c = double{-4.18301e-12},
	};

	enum class Fault : uint8_t
	{
		RtdHighThreshold = Bit7,
		RtdLowThreshold = Bit6,
		RefinMGreater85Percentbias = Bit5,
		RefinMLess85PercentVbias = Bit4,
		RtdinMLess85PercentVbias = Bit3,
		OverUnderVoltage = Bit2
	};
	MODM_FLAGS8(Fault)

	enum class Config : uint8_t
	{
		VBias = Bit7,
		ConversionModeAuto = Bit6,
		OneShot = Bit5,
		ThreeWire = Bit4,
		FaultDetectionCycle1 = Bit3,
		FaultDetectionCycle0 = Bit2,
		FaultStatusClear = Bit1,
		Rejection = Bit0
	};
	MODM_FLAGS8(Config)

	enum class FaultDetectionWrite : uint8_t
	{
		NoAction = 0b00,
		FaultDetectionAutomaticDelay = 0b01,
		RunFaultDetectionWithManualDelayCycle1 = 0b10,
		FinishFaultDetectionWithManualDelayCycle2 = 0b11,
	};
	using FaultDetectionWrite_t =
		Configuration<Config_t, FaultDetectionWrite, 0b11, 2>;  // Bit 8..10

	enum class FaultDetectionRead : uint8_t
	{
		FaultDetectionFinished = 0b00,
		AutomaticDetectionStillRunning = 0b01,
		ManualCycle1Running =
			0b10,  // waiting for user to write FinishFaultDetectionWithManualDelayCycle2
		ManualCycle2StillRunning = 0b11,
	};
	using FaultDetectionRead_t = Configuration<Config_t, FaultDetectionRead, 0b11, 2>;  // Bit 8..10

	enum class Rejection : uint8_t
	{
		Rejection60Hz = 0b0,
		Rejection50Hz = 0b1,
	};
	using Rejection_t = Configuration<Config_t, Rejection, 0b1>;  // Bit 8..10

	static constexpr uint16_t
	rtdfaultthresh(const uint16_t res)
	{
		return res << 1;
	};

	enum class Register : uint8_t
	{
		ReadConfiguration = 0x00,
		ReadRtdMsb = 0x01,
		ReadRtdLsb = 0x02,
		ReadHighFaultThresholdMsb = 0x03,
		ReadHighFaultThresholdLsb = 0x04,
		ReadLowFaultThresholdMsb = 0x05,
		ReadLowFaultThresholdLsb = 0x06,
		ReadFaultStatus = 0x07,

		WriteConfiguration = 0x80,
		WriteHighFaultThresholdMsb = 0x83,
		WriteHighFaultThresholdLsb = 0x84,
		WriteLowFaultThresholdMsb = 0x85,
		WriteLowFaultThresholdLsb = 0x86,
	};

	template<Pt pt>
	struct modm_packed Data
	{
		/// @return measure resistance in ohm
		constexpr float
		getResistance() const
		{
			const uint16_t adccode = data >> 1;
			return adccode / 32768.f * pt.Rref;
		}

		/// @return fast temperature in degrees celsius, about 0.3 degrees error between 0 and 100
		/// degrees celsius
		constexpr float
		getTemperatureFast() const
		{
			return (getResistance() - pt.R0) / pt.alpha / 100.f;
		}

		/// @return slow but accurate temperature in degrees celsius
		constexpr double
		getTemperaturePrecise() const
		{
			const double res = getResistance();
			double T = getTemperatureFast();
			const double c0 = T <= 0 ? pt.c : 0;

			// Do some fixed number of newton steps for root finding
			// on Callendar Van Dusen equation:
			// R = R0*(1+a*T+b*T*T+c*(T-100)*T*T*T)
			// Newton seems to need double precision to achieve 1.e-10 residual?!
			for (int i = 0; i < 10; i++)
			{
				const double R = double{pt.R0} * (1 + (pt.a * T) + (pt.b * T * T) +
												  (c0 * (T - 100) * T * T * T)) -
								 res;
				const double Rdash =
					double{pt.R0} * (pt.a + (2 * pt.b * T) + c0 * (((4 * T) - 300) * T * T));
				T -= R / Rdash;
				if (std::abs(R) <= 1.e-10) { break; }
			}

			return T;
		}

		uint16_t data;
	};
};  // struct max31865

/**
 * @tparam SpiMaster
 * @tparam Cs
 *
 * @author  Henrik Hose
 * @ingroup modm_driver_max31865
 */
template<typename SpiMaster, typename Cs, max31865::Pt pt = max31865::pt100>
class Max31865 : public max31865,
				 public modm::SpiDevice<SpiMaster>,
				 protected modm::NestedResumable<3>
{
public:
	using Data = max31865::Data<pt>;

	/**
	 * @param data pointer to buffer of the internal data of type Data
	 */
	Max31865(Data &data);

	/// Call this function once before using the device
	modm::ResumableResult<void>
	initialize();

	/// Read the raw data from the sensor
	modm::ResumableResult<void>
	readout();

	/// Get the data object for this sensor
	inline Data &
	getData()
	{
		return data;
	}

private:
	Data &data;
	std::array<uint8_t, 2> buffer;
	Config_t config;
	uint16_t d;

	modm::ShortTimeout timeout;

	modm::ResumableResult<void>
	writeSingleRegister(Register address, uint8_t data);

	modm::ResumableResult<uint8_t>
	readSingleRegister(Register address);

	modm::ResumableResult<uint16_t>
	readTwoRegisters(Register address);
};

}  // namespace modm

#include "max31865_impl.hpp"

#endif  // MODM_MAX31865_HPP
