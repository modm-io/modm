#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

using namespace xpcc::stm32;

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart3, xpcc::IODevice::BufferBehavior::Discard > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

typedef SystemClock<Pll<ExternalClock<MHz8>, MHz168, MHz48> > defaultSystemClock;

typedef GpioOutputB1 LedRed;
typedef GpioOutputB2 LedBlue;

typedef GpioOutputD8 DBG_UART3_TX;
typedef GpioInputD9 DBG_UART3_RX;

typedef GpioOutputB8 M1_PWM_A_T10_1;
typedef GpioOutputB9 M1_PWM_B_T11_1;
typedef GpioOutputE5 M1_PWM_C_T9_1;
typedef GpioOutputE2 NOT_M1_RESET_A; typedef NOT_M1_RESET_A M1_EN_A;
typedef GpioOutputE3 NOT_M1_RESET_B; typedef NOT_M1_RESET_B M1_EN_B;
typedef GpioOutputE4 NOT_M1_RESET_C; typedef NOT_M1_RESET_C M1_EN_C;
typedef GpioInputC0 ADC123_IN10_M1_A;
typedef GpioInputC1 ADC123_IN12_M1_B;
typedef GpioInputC2 ADC123_IN13_M1_C;
typedef GpioInputE0 NOT_M1_OTW;
typedef GpioInputE1 NOT_M1_FAULT;

typedef GpioOutputE6 M2_PWM_A_T9_2;
typedef GpioOutputA0 M2_PWM_B_T5_1;
typedef GpioOutputA1 M2_PWM_C_T5_2;
typedef GpioOutputE7  NOT_M2_RESET_A; typedef NOT_M2_RESET_A M2_EN_A;
typedef GpioOutputE10 NOT_M2_RESET_B; typedef NOT_M2_RESET_B M2_EN_B;
typedef GpioOutputE8  NOT_M2_RESET_C; typedef NOT_M2_RESET_C M2_EN_C;
typedef GpioInputC3  ADC123_IN13_M2_A;
typedef GpioInputA2  ADC123_IN2_M2_B;
typedef GpioInputA3  ADC123_IN3_M2_C;
typedef GpioInputE11 NOT_M2_OTW;
typedef GpioInputE9  NOT_M2_FAULT;

typedef GpioInputA8  ENC1A_T1_1;
typedef GpioInputA9  ENC1B_T1_2;
typedef GpioInputD14 ENC1I;

typedef GpioInputA15 ENC2A_T2_1;
typedef GpioInputB3  ENC2B_T2_2;
typedef GpioInputA10 ENC2I;

typedef GpioInputA6  ENC3A_T3_1;
typedef GpioInputA7  ENC3B_T3_2;
typedef GpioInputE15 ENC3I;

typedef GpioInputD12 ENC4A_T4_1;
typedef GpioInputD13 ENC4B_T4_2;
typedef GpioInputB15 ENC4I;

typedef GpioInputC6  ENC8A_T8_1;
typedef GpioInputC7  ENC8B_T8_2;
typedef GpioInputD10 ENC8I;

typedef GpioInputB12 HALL1_A;
typedef GpioInputB13 HALL1_B;
typedef GpioInputB14 HALL1_C;

namespace CurrentSensors
{
	const float amperePerVolt = 9.09090909090909;
	const float voltPerTick = 3.3 / 4096.0;
	const float milliAmperePerTick = amperePerVolt * voltPerTick * 1000.0;

	int32_t current_M1_A_offset;
	int32_t current_M1_B_offset;
	int32_t current_M1_C_offset;

	int32_t current_M2_A_offset;
	int32_t current_M2_B_offset;
	int32_t current_M2_C_offset;
}

typedef GpioInputA4 ADC12_IN4_BAT;
typedef GpioInputA5 ADC12_IN5_12V;
typedef GpioInputC4 ADC12_IN14_5V;

namespace VoltageSensors
{
	const float voltPerTick = 3.3 / 4096.0;
	const float voltPerTickBat = voltPerTick * ((100.0 + 3.3) / 3.3);
	const float voltPerTick12V = voltPerTick * ((33.3 + 3.3) / 3.3);
	const float voltPerTick5V  = voltPerTick * ((10.0 + 3.3) / 3.3);
}
