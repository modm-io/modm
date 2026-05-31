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

// Nucleo144 Arduino Header Footprint for STM32H5xxZx
// Derived from UM3115 rev 3, Table 21-25.

#ifndef MODM_STM32_NUCLEO144_ARDUINO_H5_HPP
#define MODM_STM32_NUCLEO144_ARDUINO_H5_HPP

// Arduino Footprint
using A0 = GpioA6;  // free
using A1 = GpioC0;  // free
using A2 = GpioC3;  // free
using A3 = GpioB1;  // free
// Default analog routing on CN9; these can be remapped to I2C1 on PB9/PB8.
using A4 = GpioC2;  // free by default; I2C1_SDA if Arduino I2C bridges are enabled
using A5 = GpioF11; // free by default; I2C1_SCL if Arduino I2C bridges are enabled
// Zio Footprint
using A6 = GpioF12; // free
using A7 = GpioF13; // free
using A8 = GpioF14; // free

// Arduino Footprint
using D0  = GpioB7;  // free by default; reroutable to VCP
using D1  = GpioB6;  // free by default; reroutable to VCP
using D2  = GpioG14; // free
using D3  = GpioE13; // free
using D4  = GpioE14; // free
using D5  = GpioE11; // free
using D6  = GpioE9;  // free
using D7  = GpioG12; // free
using D8  = GpioF3;  // free
using D9  = GpioD15; // free
using D10 = GpioD14; // free
using D11 = GpioB5;  // free by default
using D12 = GpioG9;  // free
using D13 = GpioA5;  // free by default; LD1 if SB51 is ON
using D14 = GpioB9;  // free
using D15 = GpioB8;  // free
// Zio Footprint
using D16 = GpioC6;  // free
using D17 = GpioB15; // shared with RMII_TXD1 when Ethernet is linked in
using D18 = GpioB13; // UCPD_CC1 by default
using D19 = GpioB12; // free
using D20 = GpioA15; // free
using D21 = GpioC7;  // free
using D22 = GpioB5;  // free by default
using D23 = GpioB3;  // SWO by default
using D24 = GpioG10; // free
using D25 = GpioB4;  // free
using D26 = GpioG6;  // free
using D27 = GpioB2;  // free by default
using D28 = GpioD13; // free
using D29 = GpioD12; // free
using D30 = GpioD11; // free
using D31 = GpioE2;  // free by default
using D32 = GpioA0;  // free by default; B1 if rerouted
using D33 = GpioB0;  // LD1 green LED by default
using D34 = GpioE0;  // free
using D35 = GpioA3;  // free
using D36 = GpioB10; // free
using D37 = GpioE15; // free
using D38 = GpioE6;  // free by default
using D39 = GpioE12; // free
using D40 = GpioE10; // free
using D41 = GpioE7;  // free
using D42 = GpioE8;  // free
using D43 = GpioC8;  // free
using D44 = GpioC9;  // free
using D45 = GpioC10; // free
using D46 = GpioC11; // free
using D47 = GpioC12; // free
using D48 = GpioD2;  // free
using D49 = GpioG2;  // free
using D50 = GpioG3;  // free
using D51 = GpioD7;  // free
using D52 = GpioD6;  // free
using D53 = GpioD5;  // free
using D54 = GpioD4;  // free
using D55 = GpioD3;  // free
using D56 = GpioE2;  // free by default
using D57 = GpioE4;  // free by default
using D58 = GpioE5;  // free by default
using D59 = GpioE6;  // free by default
using D60 = GpioE3;  // free by default
using D61 = GpioF8;  // free
using D62 = GpioF7;  // free
using D63 = GpioF9;  // free
using D64 = GpioG1;  // free
using D65 = GpioG0;  // free
using D66 = GpioD1;  // free
using D67 = GpioD0;  // free
using D68 = GpioF0;  // free
using D69 = GpioF1;  // free
using D70 = GpioF2;  // free
using D71 = GpioE9;  // free
using D72 = GpioB2;  // not connected on Zio by default; free if PB2 is rerouted from CN10

namespace morpho
{
// Left ST Morpho connector (CN11)
using L1  = GpioC10;    // free
using L2  = GpioC11;    // free
using L3  = GpioC12;    // free
using L4  = GpioD2;     // free
using L5  = GpioUnused; // power: VDD_MCU
using L6  = GpioUnused; // power: 5V_EXT
using L7  = GpioUnused; // BOOT0, default 0
using L8  = GpioUnused; // ground
using L9  = GpioF6;     // free
using L10 = GpioUnused; // NC
using L11 = GpioF7;     // free
using L12 = GpioUnused; // power strap: IOREF, VDD by default
using L13 = GpioA13;    // free on morpho if SWD debug is disabled; SWDIO shared with ST-LINK by default
using L14 = GpioUnused; // reset: NRST
using L15 = GpioA14;    // free on morpho if SWD debug is disabled; SWCLK shared with ST-LINK by default
using L16 = GpioUnused; // power: 3V3
using L17 = GpioA15;    // free
using L18 = GpioUnused; // power: 5V
using L19 = GpioUnused; // ground
using L20 = GpioUnused; // ground
using L21 = GpioB7;     // free on morpho if SB40 and SB63 are OFF; LPUART1_RX on Arduino D0 by default, reroutable to VCP
using L22 = GpioUnused; // ground
using L23 = GpioC13;    // B1 user button by default
using L24 = GpioUnused; // power: VIN
using L25 = GpioC14;    // LSE crystal by default, morpho only if SB44 is ON
using L26 = GpioUnused; // NC
using L27 = GpioC15;    // LSE crystal by default, morpho only if SB45 is ON
using L28 = GpioA0;     // free by default, B1 wake-up if SB59 is ON
using L29 = GpioH0;     // HSE clock input, default 8 MHz ST-LINK MCO / optional crystal
using L30 = GpioA1;     // free on morpho if Ethernet RMII routing is disabled (SB58 OFF); RMII_REF_CLK by default
using L31 = GpioH1;     // HSE clock output, occupied by the board clock source in default config
using L32 = GpioA4;     // VBUS_SENSE by default, free if SB56 is OFF
using L33 = GpioUnused; // power: VBAT, tied to VDD_MCU by default
using L34 = GpioB0;     // free on morpho if LD1 is disconnected (SB43 and SB51 are OFF); LD1 green LED by default
using L35 = GpioC2;     // free
using L36 = GpioC1;     // free on morpho if Ethernet RMII routing is disabled (SB62 OFF); RMII_MDC by default
using L37 = GpioC3;     // free
using L38 = GpioC0;     // free
using L39 = GpioD4;     // free
using L40 = GpioD3;     // free
using L41 = GpioD5;     // free
using L42 = GpioG2;     // free
using L43 = GpioD6;     // free
using L44 = GpioG3;     // free
using L45 = GpioD7;     // free
using L46 = GpioE2;     // free, can be rerouted to trace CN5
using L47 = GpioE3;     // free, can be rerouted to trace CN5
using L48 = GpioE4;     // free, can be rerouted to trace CN5
using L49 = GpioUnused; // ground
using L50 = GpioE5;     // free, can be rerouted to trace CN5
using L51 = GpioF1;     // free
using L52 = GpioF2;     // free
using L53 = GpioF0;     // free
using L54 = GpioF8;     // free
using L55 = GpioD1;     // free
using L56 = GpioF9;     // free
using L57 = GpioD0;     // free
using L58 = GpioG1;     // free
using L59 = GpioG0;     // free
using L60 = GpioUnused; // ground
using L61 = GpioUnused; // NC
using L62 = GpioE6;     // free, can be rerouted to trace CN5
using L63 = GpioG9;     // free
using L64 = GpioG15;    // free
using L65 = GpioG12;    // free
using L66 = GpioG10;    // free
using L67 = GpioUnused; // NC
using L68 = GpioG13;    // free on morpho if Ethernet RMII routing is disabled (SB37 OFF); RMII_TXD0 by default
using L69 = GpioD9;     // free on morpho if USART3 VCP routing is disabled (SB75 ON, SB18 and SB65 OFF); USART3_RX to ST-LINK VCP by default
using L70 = GpioG11;    // free on morpho if Ethernet RMII routing is disabled (SB34 OFF); RMII_TX_EN by default

// Right ST Morpho connector (CN12)
using R1  = GpioC9;     // free
using R2  = GpioC8;     // free
using R3  = GpioB8;     // Board Bus: I2C1_SCL on Zio if SB57 is ON
using R4  = GpioC6;     // free
using R5  = GpioB9;     // Board Bus: I2C1_SDA on Zio if SB52 is ON
using R6  = GpioC5;     // free on morpho if Ethernet RMII routing is disabled (SB36 OFF); RMII_RXD1 by default
using R7  = GpioUnused; // analog reference: VREFP
using R8  = GpioUnused; // power: 5V_STLK from ST-LINK
using R9  = GpioUnused; // ground
using R10 = GpioD8;     // free on morpho if USART3 VCP routing is disabled (SB13 ON, SB23 and SB24 OFF); USART3_TX to ST-LINK VCP by default
using R11 = GpioA5;     // free by default, optional LD1 routing via SB51
using R12 = GpioA12;    // USB_FS_DP by default, free if SB22 is ON
using R13 = GpioA6;     // free
using R14 = GpioA11;    // USB_FS_DM by default, free if SB21 is ON
using R15 = GpioA7;     // free on morpho if Ethernet RMII routing is disabled (SB38 OFF); RMII_CRS_DV by default
using R16 = GpioB12;    // free
using R17 = GpioB6;     // free on morpho if SB14 and SB15 are OFF; LPUART1_TX on Arduino D1 by default, reroutable to VCP
using R18 = GpioUnused; // NC
using R19 = GpioC7;     // free
using R20 = GpioUnused; // ground
using R21 = GpioA9;     // free on morpho if SB31 is OFF; UCPD_DBn by default
using R22 = GpioB2;     // free on morpho if SB61 and SB66 are OFF; QSPI_CLK on Zio by default
using R23 = GpioA8;     // free
using R24 = GpioB1;     // free
using R25 = GpioB10;    // free
using R26 = GpioB15;    // free on morpho if Ethernet RMII routing is disabled (JP6 OFF); usable on Zio if SB10 is ON and the pin is not used on morpho
using R27 = GpioB4;     // free
using R28 = GpioB14;    // free on morpho if SB30 is OFF; UCPD_CC2 by default
using R29 = GpioB5;     // free by default, Zio D11 only if SB17 is ON
using R30 = GpioB13;    // free on morpho if SB29 is OFF; UCPD_CC1 by default
using R31 = GpioB3;     // free on morpho if SB33 and SB39 are OFF; SWO by default
using R32 = GpioUnused; // analog ground
using R33 = GpioA10;    // free
using R34 = GpioC4;     // free on morpho if Ethernet RMII routing is disabled (SB42 OFF); RMII_RXD0 by default
using R35 = GpioA2;     // free on morpho if Ethernet RMII routing is disabled (SB69 OFF); RMII_MDIO by default
using R36 = GpioF5;     // free
using R37 = GpioA3;     // free
using R38 = GpioF4;     // LD2 yellow LED
using R39 = GpioUnused; // ground
using R40 = GpioE8;     // free
using R41 = GpioD13;    // free
using R42 = GpioF10;    // free
using R43 = GpioD12;    // free
using R44 = GpioE7;     // free
using R45 = GpioD11;    // free
using R46 = GpioD14;    // free
using R47 = GpioE10;    // free
using R48 = GpioD15;    // free
using R49 = GpioE12;    // free
using R50 = GpioF14;    // free
using R51 = GpioE14;    // free
using R52 = GpioE9;     // free on morpho if SB35 is OFF and SB67 is ON; TIM1_CH1 on Zio by default
using R53 = GpioE15;    // free
using R54 = GpioUnused; // ground
using R55 = GpioE13;    // free
using R56 = GpioE11;    // free
using R57 = GpioF13;    // free
using R58 = GpioF3;     // free
using R59 = GpioF12;    // free
using R60 = GpioF15;    // free
using R61 = GpioG14;    // free
using R62 = GpioF11;    // free
using R63 = GpioUnused; // ground
using R64 = GpioE0;     // free
using R65 = GpioD10;    // free
using R66 = GpioG8;     // free
using R67 = GpioG7;     // free on morpho if SB74 is OFF; UCPD_FLT by default
using R68 = GpioG5;     // free
using R69 = GpioG4;     // LD3 red LED
using R70 = GpioG6;     // free
}

#endif // MODM_STM32_NUCLEO144_ARDUINO_H5_HPP