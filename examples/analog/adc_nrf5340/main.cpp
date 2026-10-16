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

/*
# nRF5340-DK ADC (app core)

SAADC example for the nRF5340 application core.

- Target: `nrf5340-xxaa@app`
- Demonstrates channel reads and periodic logging.
- Useful for validating ADC signal/channel mapping on nRF53.
*/

#include <modm/board.hpp>
#include <modm/processing.hpp>

using Adc = modm::platform::Adc;
using Regulators = modm::platform::Regulators;

int
main()
{
	Board::initialize();
	Regulators::setMainDcdcEnabled();
	Regulators::setRadioDcdcEnabled();
	const bool vddhSupplyActive = Regulators::isVddhSupplyActive();
	const bool usbVbusPresent = Regulators::isUsbVbusPresent();
	const bool usbRegReady = Regulators::isUsbRegulatorOutputReady();
	if (vddhSupplyActive) {
		Regulators::setVddhDcdcEnabled();
		Regulators::setVddhExternalSilentEnabled(false);

		constexpr auto TargetVddhOutput = Regulators::VddhOutputVoltage::V3_0;
		auto voltageToString = [](Regulators::VddhOutputVoltage voltage) {
			switch (voltage)
			{
				case Regulators::VddhOutputVoltage::V1_8: return "1.8V";
				case Regulators::VddhOutputVoltage::V2_1: return "2.1V";
				case Regulators::VddhOutputVoltage::V2_4: return "2.4V";
				case Regulators::VddhOutputVoltage::V2_7: return "2.7V";
				case Regulators::VddhOutputVoltage::V3_0: return "3.0V";
				case Regulators::VddhOutputVoltage::V3_3: return "3.3V";
				case Regulators::VddhOutputVoltage::Default: return "default(1.8V)";
			}
			return "unknown";
		};

		auto currentVddhOutput = Regulators::vddhOutputVoltage();
		MODM_LOG_INFO << "UICR VREGHVOUT: " << voltageToString(currentVddhOutput)
			<< " target=" << voltageToString(TargetVddhOutput) << modm::endl;

		auto writeResult = Regulators::setVddhOutputVoltage(TargetVddhOutput);
		switch (writeResult)
		{
			case Regulators::VddhOutputVoltageWriteResult::Unchanged:
				MODM_LOG_INFO << "VREGHVOUT already configured." << modm::endl;
				break;
			case Regulators::VddhOutputVoltageWriteResult::EraseRequired:
				MODM_LOG_INFO << "VREGHVOUT change requires UICR erase/reprogram (cannot set 0->1 bits)." << modm::endl;
				break;
			case Regulators::VddhOutputVoltageWriteResult::Programmed:
				MODM_LOG_INFO << "VREGHVOUT updated in UICR. Resetting to apply." << modm::endl;
				NVIC_SystemReset();
				break;
		}
	}

	Board::Led1::setOutput();
	Adc::connect<GpioP0_4::Ain0, GpioP0_5::Ain1, GpioP0_6::Ain2, GpioP0_7::Ain3>();
	Adc::initialize<Board::SystemClock>(Adc::Oversampling::Over8x);

	MODM_LOG_INFO << "nRF5340-DK SAADC example" << modm::endl;
	MODM_LOG_INFO << "Main supply mode: " << (vddhSupplyActive ? "VDDH" : "VDD") << modm::endl;
	MODM_LOG_INFO << "USBREG: VBUS=" << (usbVbusPresent ? "present" : "absent")
		<< " output=" << (usbRegReady ? "ready" : "not-ready") << modm::endl;
	MODM_LOG_INFO << "Reading AIN0..AIN3, VDD and optional VDDH" << modm::endl;

	modm::PeriodicTimer timer(500ms);
	constexpr uint32_t FullScaleMilliVolts = 3600;
	constexpr uint32_t FullScaleRaw = (1u << Adc::Resolution) - 1u;
	auto rawToMillivolts = [](uint16_t raw) {
		return (uint32_t(raw) * FullScaleMilliVolts) / FullScaleRaw;
	};

	while (true)
	{
		if (timer.execute())
		{
			auto ain0 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain0));
			auto ain1 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain1));
			auto ain2 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain2));
			auto ain3 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain3));
			auto vdd = rawToMillivolts(Adc::readChannel(Adc::Channel::Vdd));
			bool vddhActive = Regulators::isVddhSupplyActive();
			auto vddh = vddhActive
				? rawToMillivolts(Adc::readChannel(Adc::Channel::VddhDiv5)) * 5u
				: 0u;

			MODM_LOG_INFO.printf("AIN0=%4lumV AIN1=%4lumV AIN2=%4lumV AIN3=%4lumV VDD=%4lumV",
				ain0, ain1, ain2, ain3, vdd);
			if (vddhActive) {
				MODM_LOG_INFO.printf(" VDDH=%4lumV", vddh);
			}
			else {
				MODM_LOG_INFO << " VDDH=n/a";
			}
			MODM_LOG_INFO << modm::endl;

			Board::Led1::toggle();
		}
	}
}
