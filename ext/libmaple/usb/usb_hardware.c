/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 *  @file usb_hardware.c
 *
 *  @brief init routines to setup clocks and interrupts for usb.
 *
 */

#include "usb_hardware.h"

void nvicInit(NVIC_InitTypeDef* NVIC_InitStruct) {
    u32 tmppriority = 0x00;
    u32 tmpreg      = 0x00;
    u32 tmpmask     = 0x00;
    u32 tmppre      = 0;
    u32 tmpsub      = 0x0F;

    SCB_TypeDef* rSCB = (SCB_TypeDef *) SCB_BASE;
    NVIC_TypeDef* rNVIC = (NVIC_TypeDef *) NVIC_BASE;


    /* Compute the Corresponding IRQ Priority -------------------------------*/
    tmppriority = (0x700 - (rSCB->AIRCR & (u32)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (u32)NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority <<
        tmppre;
    tmppriority |=  NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub;

    tmppriority = tmppriority << 0x04;
    tmppriority = ((u32)tmppriority) <<
        ((NVIC_InitStruct->NVIC_IRQChannel & (u8)0x03) * 0x08);

    tmpreg = rNVIC->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)];
    tmpmask = (u32)0xFF <<
        ((NVIC_InitStruct->NVIC_IRQChannel & (u8)0x03) * 0x08);
    tmpreg &= ~tmpmask;
    tmppriority &= tmpmask;
    tmpreg |= tmppriority;

    rNVIC->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)] = tmpreg;

    /* Enable the Selected IRQ Channels -------------------------------------*/
    rNVIC->ISER[(NVIC_InitStruct->NVIC_IRQChannel >> 0x05)] =
        (u32)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (u8)0x1F);
}

void nvicDisableInterrupts() {
    NVIC_TypeDef* rNVIC = (NVIC_TypeDef *) NVIC_BASE;
    rNVIC->ICER[0] = 0xFFFFFFFF;
    rNVIC->ICER[1] = 0xFFFFFFFF;
    rNVIC->ICPR[0] = 0xFFFFFFFF;
    rNVIC->ICPR[1] = 0xFFFFFFFF;

    /* Disable the systick timer, which operates separately from NVIC */
    SET_REG(STK_CTRL,0x04);
}

void systemHardReset(void) {
    SCB_TypeDef* rSCB = (SCB_TypeDef *) SCB_BASE;
    typedef void (*funcPtr)(void);

    /* Reset */
    rSCB->AIRCR = (u32)AIRCR_RESET_REQ;

    /* Should never get here */
    while (1) {
        asm volatile("nop");
    }
}



