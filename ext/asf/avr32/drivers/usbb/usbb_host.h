/**
 * \file
 *
 * \brief USBB Host Driver header file.
 *
 * Copyright (C) 2011 - 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _USBB_HOST_H_
#define _USBB_HOST_H_

#include "compiler.h"
#include "preprocessor.h"
#include "usbb_otg.h"

// These defines are missing from or wrong in the toolchain header files
#if defined(AVR32_UC3A364_H_INCLUDED) || defined(AVR32_UC3A364S_H_INCLUDED) \
    || defined(AVR32_UC3A3128S_H_INCLUDED) || defined(AVR32_UC3A3128_H_INCLUDED) \
    || defined(AVR32_UC3A3256S_H_INCLUDED) || defined(AVR32_UC3A3256_H_INCLUDED)
#  undef  AVR32_USBB_EPT_NUM // define value is wrong.
#  define AVR32_USBB_EPT_NUM 8
#endif

//! \ingroup uhd_group
//! \defgroup uhd_usbb_group USBB Host Driver
//! USBB low-level driver for USB host mode
//!
//! @warning Bit-masks are used instead of bit-fields because PB registers
//! require 32-bit write accesses while AVR32-GCC 4.0.2 builds 8-bit
//! accesses even when volatile unsigned int bit-fields are specified.
//! @{


//! @name USBB Host IP properties
//! These macros give access to IP properties
//! @{
//! Get maximal number of endpoints
#define uhd_get_pipe_max_nbr() \
	(((USBB_RD_BITFIELD(UFEATURES_EPT_NBR,MAX) - 1) \
	& ((1 << AVR32_USBB_UFEATURES_EPT_NBR_MAX_SIZE) - 1)) + 1)
//! @}

//! @name Host Vbus line control
//!
//! VBOF is an optional output pin which allows to enable or disable
//! the external VBus generator.
//!
//! @{
//! Enables hardware control of USB_VBOF output pin when a Vbus error occur
#define uhd_enable_vbus_error_hw_control()    USBB_CLR_BITS(USBCON,VBUSHWC)
//! Disables hardware control of USB_VBOF output pin when a Vbus error occur
#define uhd_disable_vbus_error_hw_control()   USBB_SET_BITS(USBCON,VBUSHWC)

//! Pin and function for USB_VBOF according to configuration from USB_VBOF
#define USB_VBOF_PIN            ATPASTE2(USB_VBOF, _PIN)
#define USB_VBOF_FUNCTION       ATPASTE2(USB_VBOF, _FUNCTION)
//! Output USB_VBOF onto its pin
#define uhd_output_vbof_pin() {\
	(Tst_bits(USB_VBOF_FUNCTION, 0x01)) ?\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0s = 1 << (USB_VBOF_PIN & 0x1F)) :\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0c = 1 << (USB_VBOF_PIN & 0x1F)); \
	(Tst_bits(USB_VBOF_FUNCTION, 0x02)) ?\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1s = 1 << (USB_VBOF_PIN & 0x1F)) :\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1c = 1 << (USB_VBOF_PIN & 0x1F)); \
	AVR32_GPIO.port[USB_VBOF_PIN >> 5].gperc  = 1 << (USB_VBOF_PIN & 0x1F);\
	AVR32_GPIO.port[USB_VBOF_PIN >> 5].odmerc = 1 << (USB_VBOF_PIN & 0x1F);\
	AVR32_GPIO.port[USB_VBOF_PIN >> 5].puerc  = 1 << (USB_VBOF_PIN & 0x1F); }
//! Set USB_VBOF output pin polarity
#define uhd_set_vbof_active_high()            USBB_CLR_BITS(USBCON,VBUSPO)
#define uhd_set_vbof_active_low()             USBB_SET_BITS(USBCON,VBUSPO)
//! Requests VBus activation
#define uhd_enable_vbus()                     USBB_REG_SET(USBSTA,VBUSRQ)
//! Requests VBus deactivation
#define uhd_disable_vbus()                    USBB_REG_CLR(USBSTA,VBUSRQ)
//! Tests if VBus activation has been requested
#define Is_uhd_vbus_enabled()                 USBB_TST_BITS(USBSTA,VBUSRQ)
//! @}

//! @name Host Vbus line monitoring
//!
//! The VBus level is always checked by USBB hardware.
//!
//! @{
#define uhd_enable_vbus_error_interrupt()     USBB_SET_BITS(USBCON,VBERRE)
#define uhd_disable_vbus_error_interrupt()    USBB_CLR_BITS(USBCON,VBERRE)
#define Is_uhd_vbus_error_interrupt_enabled() USBB_TST_BITS(USBCON,VBERRE)
#define uhd_ack_vbus_error_interrupt()        USBB_REG_CLR(USBSTA,VBERRI)
#define Is_uhd_vbus_error_interrupt()         USBB_TST_BITS(USBSTA,VBERRI)
//! @}

//! @name USB device connection/disconnection monitoring
//! @{
#define uhd_enable_connection_int()           USBB_REG_SET(UHINTE,DCONNIE)
#define uhd_disable_connection_int()          USBB_REG_CLR(UHINTE,DCONNIE)
#define Is_uhd_connection_int_enabled()       USBB_TST_BITS(UHINTE,DCONNIE)
#define uhd_ack_connection()                  USBB_REG_CLR(UHINT,DCONNI)
#define Is_uhd_connection()                   USBB_TST_BITS(UHINT,DCONNI)

#define uhd_enable_disconnection_int()        USBB_REG_SET(UHINTE,DDISCIE)
#define uhd_disable_disconnection_int()       USBB_REG_CLR(UHINTE,DDISCIE)
#define Is_uhd_disconnection_int_enabled()    USBB_TST_BITS(UHINTE,DDISCIE)
#define uhd_ack_disconnection()               USBB_REG_CLR(UHINT,DDISCI)
#define Is_uhd_disconnection()                USBB_TST_BITS(UHINT,DDISCI)
//! @}

//! @name USB device speed control
//! @{
#define uhd_get_speed_mode()                  USBB_RD_BITFIELD(USBSTA,SPEED)
#define Is_uhd_low_speed_mode() \
	(USBB_RD_BITFIELD(USBSTA,SPEED) == AVR32_USBB_USBSTA_SPEED_LOW)
#define Is_uhd_full_speed_mode() \
	(USBB_RD_BITFIELD(USBSTA,SPEED) == AVR32_USBB_USBSTA_SPEED_FULL)

#ifdef AVR32_USBB_USBSTA_SPEED_HIGH
#  define Is_uhd_high_speed_mode() \
	(USBB_RD_BITFIELD(USBSTA,SPEED) == AVR32_USBB_USBSTA_SPEED_HIGH)
//! Enable high speed mode
#  define uhd_enable_high_speed_mode()        USBB_WR_BITFIELD(UHCON,SPDCONF,0)
//! Disable high speed mode
#  define uhd_disable_high_speed_mode()       USBB_WR_BITFIELD(UHCON,SPDCONF,3)
#else
#  define Is_uhd_high_speed_mode()            false
#endif
//! @}

//! @name Bus events control
//! These macros manage the bus events: reset, SOF, resume, wakeup.
//! @{

//! Initiates a reset event
//! @{
#define uhd_start_reset()                            USBB_SET_BITS(UHCON,RESET)
#define Is_uhd_starting_reset()                      USBB_TST_BITS(UHCON,RESET)
#define uhd_stop_reset()                             USBB_CLR_BITS(UHCON,RESET)

#define uhd_enable_reset_sent_interrupt()            USBB_REG_SET(UHINTE,RSTIE)
#define uhd_disable_reset_sent_interrupt()           USBB_REG_CLR(UHINTE,RSTIE)
#define Is_uhd_reset_sent_interrupt_enabled()        USBB_TST_BITS(UHINTE,RSTIE)
#define uhd_ack_reset_sent()                         USBB_REG_CLR(UHINT,RSTI)
#define Is_uhd_reset_sent()                          USBB_TST_BITS(UHINT,RSTI)
//! @}

//! Initiates a SOF events
//! @{
#define uhd_enable_sof()                             USBB_SET_BITS(UHCON,SOFE)
#define uhd_disable_sof()                            USBB_CLR_BITS(UHCON,SOFE)
#define Is_uhd_sof_enabled()                         USBB_TST_BITS(UHCON,SOFE)
#define uhd_get_sof_number()                         USBB_RD_BITFIELD(UHFNUM,FNUM)
#define uhd_get_microsof_number() \
		(Rd_bitfield(AVR32_USBB.uhfnum, \
		AVR32_USBB_UHFNUM_FNUM_MASK|AVR32_USBB_UHFNUM_MFNUM_MASK))
#define uhd_get_frame_position()                     USBB_RD_BITFIELD(UHFNUM,FLENHIGH)
#define uhd_enable_sof_interrupt()                   USBB_REG_SET(UHINTE,HSOFIE)
#define uhd_disable_sof_interrupt()                  USBB_REG_CLR(UHINTE,HSOFIE)
#define Is_uhd_sof_interrupt_enabled()               USBB_TST_BITS(UHINTE,HSOFIE)
#define uhd_ack_sof()                                USBB_REG_CLR(UHINT,HSOFI)
#define Is_uhd_sof()                                 USBB_TST_BITS(UHINT,HSOFI)
//! @}

//! Initiates a resume event
//! It is called downstream resume event.
//! @{
#define uhd_send_resume()                            USBB_SET_BITS(UHCON,RESUME)
#define Is_uhd_sending_resume()                      USBB_TST_BITS(UHCON,RESUME)

#define uhd_enable_downstream_resume_interrupt()     USBB_REG_SET(UHINTE,RSMEDIE)
#define uhd_disable_downstream_resume_interrupt()    USBB_REG_CLR(UHINTE,RSMEDIE)
#define Is_uhd_downstream_resume_interrupt_enabled() USBB_TST_BITS(UHINTE,RSMEDIE)
#define uhd_ack_downstream_resume()                  USBB_REG_CLR(UHINT,RSMEDI)
#define Is_uhd_downstream_resume()                   USBB_TST_BITS(UHINT,RSMEDI)
//! @}

//! Detection of a wake-up event
//! A wake-up event is received when the host controller is in the suspend mode:
//! - and an upstream resume from the peripheral is detected.
//! - and a peripheral disconnection is detected.
//! @{
#define uhd_enable_wakeup_interrupt()                USBB_REG_SET(UHINTE,HWUPIE)
#define uhd_disable_wakeup_interrupt()               USBB_REG_CLR(UHINTE,HWUPIE)
#define Is_uhd_wakeup_interrupt_enabled()            USBB_TST_BITS(UHINTE,HWUPIE)
#define uhd_ack_wakeup()                             USBB_REG_CLR(UHINT,HWUPI)
#define Is_uhd_wakeup()                              USBB_TST_BITS(UHINT,HWUPI)

#define uhd_enable_upstream_resume_interrupt()       USBB_REG_SET(UHINTE,RXRSMIE)
#define uhd_disable_upstream_resume_interrupt()      USBB_REG_CLR(UHINTE,RXRSMIE)
#define Is_uhd_upstream_resume_interrupt_enabled()   USBB_TST_BITS(UHINTE,RXRSMIE)
#define uhd_ack_upstream_resume()                    USBB_REG_CLR(UHINT,RXRSMI)
#define Is_uhd_upstream_resume()                     USBB_TST_BITS(UHINT,RXRSMI)
//! @}
//! @}


//! @name Pipes management
//! @{

//! Generic macros for USBB pipe registers that can be arrayed
//! @{
#define USBB_ARRAY(reg,index)   (((volatile unsigned long*)(&AVR32_USBB.reg))[index])
#define USBB_P_CLR_BITS(reg, bit, pipe) \
		(Clr_bits(USBB_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBB_,reg,0_,bit,_MASK)))
#define USBB_P_SET_BITS(reg, bit, pipe) \
		(Set_bits(USBB_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBB_,reg,0_,bit,_MASK)))
#define USBB_P_TST_BITS(reg, bit, pipe) \
		(Tst_bits(USBB_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBB_,reg,0_,bit,_MASK)))
#define USBB_P_RD_BITFIELD(reg, bit, pipe) \
		(Rd_bitfield(USBB_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBB_,reg,0_,bit,_MASK)))
#define USBB_P_WR_BITFIELD(reg, bit, pipe, value) \
		(Wr_bitfield(USBB_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBB_,reg,0_,bit,_MASK), value))
#define USBB_P_REG_CLR(reg, bit, pipe) \
		(USBB_ARRAY(TPASTE2(reg,0CLR),pipe) \
		 = TPASTE5(AVR32_USBB_,reg,0CLR_,bit,C_MASK))
#define USBB_P_REG_SET(reg, bit, pipe) \
		(USBB_ARRAY(TPASTE2(reg,0SET),pipe) \
		 = TPASTE5(AVR32_USBB_,reg,0SET_,bit,S_MASK))
//! @}

//! USB address of pipes
//! @{
#define uhd_configure_address(p, addr) \
		(Wr_bitfield((&AVR32_USBB.uhaddr1)[(p)>>2], \
		AVR32_USBB_UHADDR1_UHADDR_P0_MASK << (((p)&0x03)<<3), addr))
#define uhd_get_configured_address(p) \
		(Rd_bitfield((&AVR32_USBB.uhaddr1)[(p)>>2], \
		AVR32_USBB_UHADDR1_UHADDR_P0_MASK << (((p)&0x03)<<3)))
//! @}

//! Pipe enable
//! Enable, disable, reset, freeze
//! @{
#define uhd_enable_pipe(p) \
		(Set_bits(AVR32_USBB.uprst, AVR32_USBB_UPRST_PEN0_MASK << (p)))
#define uhd_disable_pipe(p) \
		(Clr_bits(AVR32_USBB.uprst, AVR32_USBB_UPRST_PEN0_MASK << (p)))
#define Is_uhd_pipe_enabled(p) \
		(Tst_bits(AVR32_USBB.uprst, AVR32_USBB_UPRST_PEN0_MASK << (p)))
#define uhd_reset_pipe(p) \
		(Set_bits(AVR32_USBB.uprst, AVR32_USBB_UPRST_PRST0_MASK << (p))); \
		(Clr_bits(AVR32_USBB.uprst, AVR32_USBB_UPRST_PRST0_MASK << (p)))
#define Is_uhd_resetting_pipe(p) \
		(Tst_bits(AVR32_USBB.uprst, AVR32_USBB_UPRST_PRST0_MASK << (p)))
#define uhd_freeze_pipe(p)                       USBB_P_REG_SET(UPCON,PFREEZE,p)
#define uhd_unfreeze_pipe(p)                     USBB_P_REG_CLR(UPCON,PFREEZE,p)
#define Is_uhd_pipe_frozen(p)                    USBB_P_TST_BITS(UPCON,PFREEZE,p)
#define uhd_reset_data_toggle(p)                 USBB_P_REG_SET(UPCON,RSTDT,p)
#define Is_uhd_data_toggle_reset(p)              USBB_P_TST_BITS(UPCON,RSTDT,p)
//! @}

//! Pipe configuration
//! @{
#define uhd_configure_pipe_int_req_freq(p,freq)  USBB_P_WR_BITFIELD(UPCFG,INTFRQ,p,freq)
#define uhd_get_pipe_int_req_freq(p)             USBB_P_RD_BITFIELD(UPCFG,INTFRQ,p)
#define uhd_configure_pipe_endpoint_number(p,ep) USBB_P_WR_BITFIELD(UPCFG,PEPNUM,p,ep)
#define uhd_get_pipe_endpoint_address(p) \
		(uhd_get_pipe_endpoint_number(p) \
		| (uhd_is_pipe_in(p)? USB_EP_DIR_IN : USB_EP_DIR_OUT))
#define uhd_get_pipe_endpoint_number(p)          USBB_P_RD_BITFIELD(UPCFG,PEPNUM,p)
#define uhd_configure_pipe_type(p, type)         USBB_P_WR_BITFIELD(UPCFG,PTYPE,p, type)
#define uhd_get_pipe_type(p)                     USBB_P_RD_BITFIELD(UPCFG,PTYPE,p)
#define uhd_enable_pipe_bank_autoswitch(p)       USBB_P_SET_BITS(UPCFG,AUTOSW,p)
#define uhd_disable_pipe_bank_autoswitch(p)      USBB_P_CLR_BITS(UPCFG,AUTOSW,p)
#define Is_uhd_pipe_bank_autoswitch_enabled(p)   USBB_P_TST_BITS(UPCFG,AUTOSW,p)
#define uhd_configure_pipe_token(p, token)       USBB_P_WR_BITFIELD(UPCFG,PTOKEN,p, token)
#define uhd_get_pipe_token(p)                    USBB_P_RD_BITFIELD(UPCFG,PTOKEN,p)
#define uhd_is_pipe_in(p)                        (AVR32_USBB_UPCFG0_PTOKEN_IN==uhd_get_pipe_token(p))
#define uhd_is_pipe_out(p)                       (AVR32_USBB_UPCFG0_PTOKEN_OUT==uhd_get_pipe_token(p))
//! Bounds given integer size to allowed range and rounds it up to the nearest
//! available greater size, then applies register format of USBB controller
//! for pipe size bit-field.
#define uhd_format_pipe_size(size) \
		(32 - clz(((uint32_t)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
#define uhd_configure_pipe_size(p,size) \
		USBB_P_WR_BITFIELD(UPCFG,PSIZE,p, uhd_format_pipe_size(size))
#define uhd_get_pipe_size(p)                     (8 << USBB_P_RD_BITFIELD(UPCFG,PSIZE,p))
#define uhd_configure_pipe_bank(p,bank)          USBB_P_WR_BITFIELD(UPCFG,PBK,p, bank)
#define uhd_get_pipe_bank(p)                     USBB_P_RD_BITFIELD(UPCFG,PBK,p)
#define uhd_allocate_memory(p)                   USBB_P_SET_BITS(UPCFG,ALLOC,p)
#define uhd_unallocate_memory(p)                 USBB_P_CLR_BITS(UPCFG,ALLOC,p)
#define Is_uhd_memory_allocated(p)               USBB_P_TST_BITS(UPCFG,ALLOC,p)

#ifdef AVR32_USBB_UPCFG0_PINGEN_MASK
//! Enable PING management only available in HS mode
#  define uhd_enable_ping(p)                     USBB_P_SET_BITS(UPCFG,PINGEN,p)
#endif
#define uhd_configure_pipe(p, freq, ep_num, type, token, size, bank, bank_switch) \
	USBB_ARRAY(upcfg0,p) = \
	(((uint32_t)(freq  ) << AVR32_USBB_UPCFG0_INTFRQ_OFFSET) & AVR32_USBB_UPCFG0_INTFRQ_MASK) |\
	(((uint32_t)(ep_num) << AVR32_USBB_UPCFG0_PEPNUM_OFFSET) & AVR32_USBB_UPCFG0_PEPNUM_MASK) |\
	(((uint32_t)(type  ) << AVR32_USBB_UPCFG0_PTYPE_OFFSET ) & AVR32_USBB_UPCFG0_PTYPE_MASK ) |\
	(((uint32_t)(token ) << AVR32_USBB_UPCFG0_PTOKEN_OFFSET) & AVR32_USBB_UPCFG0_PTOKEN_MASK) |\
	((uint32_t)uhd_format_pipe_size(size) << AVR32_USBB_UPCFG0_PSIZE_OFFSET               ) |\
	bank_switch |\
	(((uint32_t)(bank  ) << AVR32_USBB_UPCFG0_PBK_OFFSET   ) & AVR32_USBB_UPCFG0_PBK_MASK   )
#define Is_uhd_pipe_configured(p)                USBB_P_TST_BITS(UPSTA,CFGOK,p)
//! @}

//! Pipe main interrupts management
//! @{
#define uhd_enable_pipe_interrupt(p) \
		(AVR32_USBB.uhinteset = AVR32_USBB_UHINTESET_P0INTES_MASK << (p))
#define uhd_disable_pipe_interrupt(p) \
		(AVR32_USBB.uhinteclr = AVR32_USBB_UHINTECLR_P0INTEC_MASK << (p))
#define Is_uhd_pipe_interrupt_enabled(p) \
		(Tst_bits(AVR32_USBB.uhinte, AVR32_USBB_UHINTE_P0INTE_MASK << (p)))
#define Is_uhd_pipe_interrupt(p) \
		(Tst_bits(AVR32_USBB.uhint, AVR32_USBB_UHINT_P0INT_MASK << (p)))
//! returns the lowest pipe number generating a pipe interrupt or AVR32_USBB_EPT_NUM if none
#define uhd_get_interrupt_pipe_number() \
	(ctz(((AVR32_USBB.uhint >> AVR32_USBB_UHINT_P0INT_OFFSET) & \
	(AVR32_USBB.uhinte >> AVR32_USBB_UHINTE_P0INTE_OFFSET)) | (1 << AVR32_USBB_EPT_NUM)))
//! @}

//! Pipe overflow and underflow for isochronous and interrupt endpoints
//! @{
#define uhd_enable_overflow_interrupt(p)         USBB_P_REG_SET(UPCON,OVERFIE,p)
#define uhd_disable_overflow_interrupt(p)        USBB_P_REG_CLR(UPCON,OVERFIE,p)
#define Is_uhd_overflow_interrupt_enabled(p)     USBB_P_TST_BITS(UPCON,OVERFIE,p)
#define uhd_ack_overflow_interrupt(p)            USBB_P_REG_CLR(UPSTA,OVERFI,p)
#define Is_uhd_overflow(p)                       USBB_P_TST_BITS(UPSTA,OVERFI,p)

#define uhd_enable_underflow_interrupt(p)        USBB_P_REG_SET(UPCON,UNDERFE,p)
#define uhd_disable_underflow_interrupt(p)       USBB_P_REG_CLR(UPCON,UNDERFE,p)
#define Is_uhd_underflow_interrupt_enabled(p)    USBB_P_TST_BITS(UPCON,UNDERFE,p)
#define uhd_ack_underflow_interrupt(p)           USBB_P_REG_CLR(UPSTA,UNDERFI,p)
#define Is_uhd_underflow(p)                      USBB_P_TST_BITS(UPSTA,UNDERFI,p)
//! @}

//! USB packet errors management
//! @{
#define uhd_enable_stall_interrupt(p)            USBB_P_REG_SET(UPCON,RXSTALLDE,p)
#define uhd_disable_stall_interrupt(p)           USBB_P_REG_CLR(UPCON,RXSTALLDE,p)
#define Is_uhd_stall_interrupt_enabled(p)        USBB_P_TST_BITS(UPCON,RXSTALLDE,p)
#define uhd_ack_stall(p)                         USBB_P_REG_CLR(UPSTA,RXSTALLDI,p)
#define Is_uhd_stall(p)                          USBB_P_TST_BITS(UPSTA,RXSTALLDI,p)

#define uhd_enable_crc_error_interrupt(p)        USBB_P_REG_SET(UPCON,RXSTALLDE,p)
#define uhd_disable_crc_error_interrupt(p)       USBB_P_REG_CLR(UPCON,RXSTALLDE,p)
#define Is_uhd_crc_error_interrupt_enabled(p)    USBB_P_TST_BITS(UPCON,RXSTALLDE,p)
#define uhd_ack_crc_error(p)                     USBB_P_REG_CLR(UPSTA,RXSTALLDI,p)
#define Is_uhd_crc_error(p)                      USBB_P_TST_BITS(UPSTA,RXSTALLDI,p)

#define uhd_enable_pipe_error_interrupt(p)       USBB_P_REG_SET(UPCON,PERRE,p)
#define uhd_disable_pipe_error_interrupt(p)      USBB_P_REG_CLR(UPCON,PERRE,p)
#define Is_uhd_pipe_error_interrupt_enabled(p)   USBB_P_TST_BITS(UPCON,PERRE,p)
#define uhd_ack_all_errors(p)                    (USBB_ARRAY(uperr0,p) = 0)
#define Is_uhd_pipe_error(p)                     USBB_P_TST_BITS(UPSTA,PERRI,p)
#define uhd_error_status(p)                      (USBB_ARRAY(uperr0,p))
#define Is_uhd_bad_data_toggle(p)                USBB_P_TST_BITS(UPERR,DATATGL,p)
#define Is_uhd_data_pid_error(p)                 USBB_P_TST_BITS(UPERR,DATAPID,p)
#define Is_uhd_pid_error(p)                      USBB_P_TST_BITS(UPERR,PID,p)
#define Is_uhd_timeout_error(p)                  USBB_P_TST_BITS(UPERR,TIMEOUT,p)
#define Is_uhd_crc16_error(p)                    USBB_P_TST_BITS(UPERR,CRC16,p)
#define uhd_get_error_counter(p)                 USBB_P_RD_BITFIELD(UPERR,COUNTER,p)
//! @}

//! Pipe data management
//! @{
#define uhd_data_toggle(p)                       USBB_P_RD_BITFIELD(UPSTA,DTSEQ,p)

#define uhd_enable_bank_interrupt(p)             USBB_P_REG_SET(UPCON,NBUSYBKE,p)
#define uhd_disable_bank_interrupt(p)            USBB_P_REG_CLR(UPCON,NBUSYBKE,p)
#define Is_uhd_bank_interrupt_enabled(p)         USBB_P_TST_BITS(UPCON,NBUSYBKE,p)
#define uhd_nb_busy_bank(p)                      USBB_P_RD_BITFIELD(UPSTA,NBUSYBK,p)
#define uhd_current_bank(p)                      USBB_P_RD_BITFIELD(UPSTA,CURRBK,p)

#define uhd_enable_short_packet_interrupt(p)     USBB_P_REG_SET(UPCON,SHORTPACKETIE,p)
#define uhd_disable_short_packet_interrupt(p)    USBB_P_REG_CLR(UPCON,SHORTPACKETIE,p)
#define Is_uhd_short_packet_interrupt_enabled(p) USBB_P_TST_BITS(UPCON,SHORTPACKETIE,p)
#define uhd_ack_short_packet(p)                  USBB_P_REG_CLR(UPSTA,SHORTPACKETI,p)
#define Is_uhd_short_packet(p)                   USBB_P_TST_BITS(UPSTA,SHORTPACKETI,p)
#define uhd_byte_count(p)                        USBB_P_RD_BITFIELD(UPSTA,PBYCT,p)

#define Is_uhd_fifocon(p)                        USBB_P_TST_BITS(UPCON,FIFOCON,p)
#define uhd_ack_fifocon(p)                       USBB_P_REG_CLR(UPCON,FIFOCON,p)

#define uhd_enable_setup_ready_interrupt(p)      USBB_P_REG_SET(UPCON,TXSTPE,p)
#define uhd_disable_setup_ready_interrupt(p)     USBB_P_REG_CLR(UPCON,TXSTPE,p)
#define Is_uhd_setup_ready_interrupt_enabled(p)  USBB_P_TST_BITS(UPCON,TXSTPE,p)
#define uhd_ack_setup_ready(p)                   USBB_P_REG_CLR(UPSTA,TXSTPI,p)
#define Is_uhd_setup_ready(p)                    USBB_P_TST_BITS(UPSTA,TXSTPI,p)

#define uhd_enable_in_received_interrupt(p)      USBB_P_REG_SET(UPCON,RXINE,p)
#define uhd_disable_in_received_interrupt(p)     USBB_P_REG_CLR(UPCON,RXINE,p)
#define Is_uhd_in_received_interrupt_enabled(p)  USBB_P_TST_BITS(UPCON,RXINE,p)
#define uhd_ack_in_received(p)                   USBB_P_REG_CLR(UPSTA,RXINI,p)
#define Is_uhd_in_received(p)                    USBB_P_TST_BITS(UPSTA,RXINI,p)

#define uhd_enable_out_ready_interrupt(p)        USBB_P_REG_SET(UPCON,TXOUTE,p)
#define uhd_disable_out_ready_interrupt(p)       USBB_P_REG_CLR(UPCON,TXOUTE,p)
#define Is_uhd_out_ready_interrupt_enabled(p)    USBB_P_TST_BITS(UPCON,TXOUTE,p)
#define uhd_ack_out_ready(p)                     USBB_P_REG_CLR(UPSTA,TXOUTI,p)
#define Is_uhd_out_ready(p)                      USBB_P_TST_BITS(UPSTA,TXOUTI,p)
#define uhd_raise_out_ready(p)                   USBB_P_REG_SET(UPSTA,TXOUTI,p)

#define uhd_enable_nak_received_interrupt(p)     USBB_P_REG_SET(UPCON,NAKEDE,p)
#define uhd_disable_nak_received_interrupt(p)    USBB_P_REG_CLR(UPCON,NAKEDE,p)
#define Is_uhd_nak_received_interrupt_enabled(p) USBB_P_TST_BITS(UPCON,NAKEDE,p)
#define uhd_ack_nak_received(p)                  USBB_P_REG_CLR(UPSTA,NAKEDI,p)
#define Is_uhd_nak_received(p)                   USBB_P_TST_BITS(UPSTA,NAKEDI,p)

#define Is_uhd_read_enabled(p)                   USBB_P_TST_BITS(UPSTA,RWALL,p)
#define Is_uhd_write_enabled(p)                  USBB_P_TST_BITS(UPSTA,RWALL,p)

#define uhd_enable_continuous_in_mode(p)         USBB_P_SET_BITS(UPINRQ,INMODE,p)
#define uhd_disable_continuous_in_mode(p)        USBB_P_CLR_BITS(UPINRQ,INMODE,p)
#define Is_uhd_continuous_in_mode_enabled(p)     USBB_P_TST_BITS(UPINRQ,INMODE,p)

#define uhd_in_request_number(p, in_num)         USBB_P_WR_BITFIELD(UPINRQ,INRQ,p,in_num-1)
#define uhd_get_in_request_number(p)             (USBB_P_RD_BITFIELD(UPINRQ,INRQ,p)+1)

  //! Get 64-, 32-, 16- or 8-bit access to FIFO data register of selected pipe.
  //! @param p      Target Pipe number
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       Volatile 64-, 32-, 16- or 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define uhd_get_pipe_fifo_access(p, scale) \
          (((volatile TPASTE3(uint, scale, _t) (*)[0x10000 / ((scale) / 8)])AVR32_USBB_SLAVE)[(p)])
//! @}


//! Pipe DMA management
//! @{

//! Maximum transfer size on USB DMA
#define UHD_PIPE_MAX_TRANS 0x10000

#define uhd_enable_pipe_int_dis_hdma_req(p)      USBB_P_REG_SET(UPCON,PDISHDMA,p)
#define uhd_disable_pipe_int_dis_hdma_req(p)     USBB_P_REG_CLR(UPCON,PDISHDMA,p)
#define Is_uhd_pipe_int_dis_hdma_req_enabled(p)  USBB_P_TST_BITS(UPCON,PDISHDMA,p)

#define uhd_enable_pipe_dma_interrupt(p) \
	(AVR32_USBB.uhinteset = AVR32_USBB_UHINTESET_DMA1INTES_MASK << ((p) - 1))
#define uhd_disable_pipe_dma_interrupt(p) \
	(AVR32_USBB.uhinteclr = AVR32_USBB_UHINTECLR_DMA1INTEC_MASK << ((p) - 1))
#define Is_uhd_pipe_dma_interrupt_enabled(p) \
	(Tst_bits(AVR32_USBB.uhinte, AVR32_USBB_UHINTE_DMA1INTE_MASK << ((p) - 1)))
#define Is_uhd_pipe_dma_interrupt(p) \
	(Tst_bits(AVR32_USBB.uhint, AVR32_USBB_UHINT_DMA1INT_MASK << ((p) - 1)))
#define uhd_get_pipe_dma_interrupt_number() \
	((ctz(((AVR32_USBB.uhint >> AVR32_USBB_UHINT_DMA1INT_OFFSET) & \
	(AVR32_USBB.uhinte >> AVR32_USBB_UHINTE_DMA1INTE_OFFSET)) \
	| (1 << (AVR32_USBB_EPT_NUM-1))))+1)

//! Structure for DMA registers
typedef struct {
	union {
		unsigned long nextdesc;
		avr32_usbb_uhdma1_nextdesc_t NEXTDESC;
	};
	unsigned long addr;
	union {
		unsigned long control;
		avr32_usbb_uhdma1_control_t CONTROL;
	};
	union {
		unsigned long status;
		avr32_usbb_uhdma1_status_t STATUS;
	};
} avr32_usbb_uhdmax_t;
//! Structure for DMA registers
#define  USBB_UHDMA_ARRAY(p) \
	(((volatile avr32_usbb_uhdmax_t *)&AVR32_USBB.uhdma1_nextdesc)[(p) - 1])

//! Set control desc to selected endpoint DMA channel
#define  uhd_pipe_dma_set_control(p,desc)     (USBB_UHDMA_ARRAY(p).control=desc)
//! Get control desc to selected endpoint DMA channel
#define  uhd_pipe_dma_get_control(p)          (USBB_UHDMA_ARRAY(p).control)
//! Set RAM address to selected endpoint DMA channel
#define  uhd_pipe_dma_set_addr(p,add)         (USBB_UHDMA_ARRAY(p).addr=add)
//! Get status to selected endpoint DMA channel
#define  uhd_pipe_dma_get_status(p)           (USBB_UHDMA_ARRAY(p).status)
//! @}
//! @}

//! @}

#endif // _USBB_HOST_H_
