/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/*
 * A single FreeRTOS task which reads symbols from Usart1
 * and sends them back, toggling the LED for every symbol
 */

using namespace Board;
using namespace modm::platform;

using Uart = BufferedUart<UsartHal1, UartTxBuffer<4>>;
using FreeRtosUart = BufferedUart<UsartHal1, UartRxBufferFreeRtos<8>, UartTxBufferFreeRtos<4>>;

void
taskMain(void *)
{
    // Let's test the old driver first:
    Uart::connect<GpioOutputB6::Tx, GpioInputB7::Rx>();
    Uart::initialize<SystemClock, 115200_Bd>();
    Uart::writeBlocking( (uint8_t *)"Old UART\r\n", 10 );

    while( (USART1->SR & USART_SR_TC) == 0 ); // Making sure the transmission has finished.
                                              // Maybe this should be exposed via UsartHal?

    // The old UART driver and the new one can coexist, and you
    // can even switch between them at runtime:
    FreeRtosUart::connect<GpioOutputB6::Tx, GpioInputB7::Rx>();
    FreeRtosUart::initialize<SystemClock, 115200_Bd>();
    Led::set();

    FreeRtosUart::writeBlocking( (uint8_t *)"FreeRTOS UART\r\n", 15 );

    uint8_t chr;
    while (true)
    {
        FreeRtosUart::read( chr );
        FreeRtosUart::write( chr );
        Led::toggle();
    }
}

constexpr int stack_size = 200;
StackType_t stack[stack_size];
StaticTask_t taskBuffer;

int
main()
{
    Board::initialize();

    TaskHandle_t h_mainTask = xTaskCreateStatic(taskMain, "Main",  stack_size, NULL, 2, stack, &taskBuffer);
    configASSERT( h_mainTask != NULL );
    vTaskStartScheduler();
    return 0;
}
