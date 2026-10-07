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
# nRF52840-DK SAADC + regulators

ADC and power-regulator demonstration.

- Reads `Ain0..Ain3`, `Vdd` and `VddhDiv5`.
- Demonstrates regulator APIs (`DCDC` enable/disable, `REGOUT0` handling).
- Uses button input to toggle DCDC state and logs measured voltages.

This example is useful to validate the nRF SAADC channel generation and power
control APIs.
*/

#include <modm/board.hpp>
#include <modm/processing.hpp>

using Adc = modm::platform::Adc;
using Regulators = modm::platform::Regulators;

int
main()
{
	Board::initialize();

	constexpr auto TargetVddOutput = Regulators::VddOutputVoltage::V3_0;
	auto voltageToString = [](Regulators::VddOutputVoltage voltage) {
		switch (voltage)
		{
			case Regulators::VddOutputVoltage::V1_8: return "1.8V";
			case Regulators::VddOutputVoltage::V2_1: return "2.1V";
			case Regulators::VddOutputVoltage::V2_4: return "2.4V";
			case Regulators::VddOutputVoltage::V2_7: return "2.7V";
			case Regulators::VddOutputVoltage::V3_0: return "3.0V";
			case Regulators::VddOutputVoltage::V3_3: return "3.3V";
			case Regulators::VddOutputVoltage::Default: return "default(1.8V)";
		}
		return "unknown";
	};

	auto currentVddOutput = Regulators::vddOutputVoltage();
	MODM_LOG_INFO << "UICR REGOUT0: " << voltageToString(currentVddOutput)
		<< " target=" << voltageToString(TargetVddOutput) << modm::endl;

	auto writeResult = Regulators::setVddOutputVoltage(TargetVddOutput);
	switch (writeResult)
	{
		case Regulators::VddOutputVoltageWriteResult::Unchanged:
			MODM_LOG_INFO << "REGOUT0 already configured." << modm::endl;
			break;
		case Regulators::VddOutputVoltageWriteResult::EraseRequired:
			MODM_LOG_INFO << "REGOUT0 change requires UICR erase/reprogram (cannot set 0->1 bits)." << modm::endl;
			break;
		case Regulators::VddOutputVoltageWriteResult::Programmed:
			MODM_LOG_INFO << "REGOUT0 updated in UICR. Resetting to apply." << modm::endl;
			NVIC_SystemReset();
			break;
	}

	bool dcdcEnabled = true;
	Regulators::setMainDcdcEnabled(dcdcEnabled);
	Regulators::setVddhDcdcEnabled(dcdcEnabled);

	Board::Led1::setOutput();
	Adc::connect<GpioP0_2::Ain0, GpioP0_3::Ain1, GpioP0_4::Ain2, GpioP0_5::Ain3>();
	Adc::initialize<Board::SystemClock>(Adc::Oversampling::Over8x);

	MODM_LOG_INFO << "nRF52840-DK SAADC example" << modm::endl;
	MODM_LOG_INFO << "Button1 toggles DCDC (main + high)" << modm::endl;
	MODM_LOG_INFO << "Reading AIN0..AIN3, VDD and VDDH/5" << modm::endl;

	modm::PeriodicTimer sampleTimer(500ms);
	modm::PeriodicTimer buttonPollTimer(20ms);
	bool previousButtonPressed = false;

	constexpr uint32_t FullScaleMilliVolts = 3600;
	constexpr uint32_t FullScaleRaw = (1u << Adc::Resolution) - 1u;
	auto rawToMillivolts = [](uint16_t raw) {
		return (uint32_t(raw) * FullScaleMilliVolts) / FullScaleRaw;
	};

	while (true)
	{
		if (buttonPollTimer.execute()) {
			const bool buttonPressed = Board::Button1::read();
			if (buttonPressed && !previousButtonPressed) {
				dcdcEnabled = !dcdcEnabled;
				Regulators::setMainDcdcEnabled(dcdcEnabled);
				Regulators::setVddhDcdcEnabled(dcdcEnabled);
				MODM_LOG_INFO << "DCDC " << (dcdcEnabled ? "enabled" : "disabled") << modm::endl;
			}
			previousButtonPressed = buttonPressed;
		}

		if (sampleTimer.execute())
		{
			auto ain0 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain0));
			auto ain1 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain1));
			auto ain2 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain2));
			auto ain3 = rawToMillivolts(Adc::readChannel(Adc::Channel::Ain3));
			auto vdd = rawToMillivolts(Adc::readChannel(Adc::Channel::Vdd));
			auto vddh = rawToMillivolts(Adc::readChannel(Adc::Channel::VddhDiv5)) * 5u;

			MODM_LOG_INFO.printf("AIN0=%4lumV AIN1=%4lumV AIN2=%4lumV AIN3=%4lumV VDD=%4lumV VDDH=%4lumV DCDC=%s",
				ain0, ain1, ain2, ain3, vdd, vddh, dcdcEnabled ? "on" : "off");
			MODM_LOG_INFO << modm::endl;

			Board::Led1::toggle();
		}
	}
}
