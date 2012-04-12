// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
/*
 * The AD7280A supports only 32-bit SPI operations.
 * 
 * 5-Bit Device Address
 * 6-Bit Register Address
 * 
 * Write cycle:
 *    3                   2                   1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Device  |  Register |     Data      |A|0|      CRC      |0|1|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 * A = Address All
 * 
 * Read register cycle:
 *    3                   2                   1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Device  |  Register |     Data      |0|0|C|      CRC      |0|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 * C = Acknowledge
 * 
 * Read conversion result cycle:
 *    3                   2                   1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Device  |Channel|         Data          |C|      CRC      |0|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 * C = Acknowledge
 * 
 * Device Adress is send LSB first, all other data is send MSB first.
 * 
 * The CRC is calculated over Bit 31-11 on write and Bit 31-10 on read
 * (everything before the CRC).
 */

#ifndef XPCC__AD7280A_HPP
#	error "Don't include this file directly! Use 'ad7280a.hpp' instead."
#endif

#include <xpcc/math/utils/bit_operation.hpp>

// TODO DEBUG!
#include <xpcc/debug/logger.hpp>
#undef XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL	xpcc::log::DEBUG

// ----------------------------------------------------------------------------
// Bits and Masks

// Control Register HIGH Byte
//@{
#define AD7280A_CTRL_HB_CONV_INPUT_ALL                  (0 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL_AUX1_3_4       (1 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL                (2 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST            (3 << 6)

#define AD7280A_CTRL_HB_CONV_RES_READ_ALL               (0 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL_AUX1_3_4    (1 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL             (2 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_NO                (3 << 4)

#define AD7280A_CTRL_HB_CONV_START_CNVST                (0 << 3)
#define AD7280A_CTRL_HB_CONV_START_CS                   (1 << 3)

#define AD7280A_CTRL_HB_CONV_AVG_DIS                    (0 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_2                      (1 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_4                      (2 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_8                      (3 << 1)
#define AD7280A_CTRL_HB_CONV_AVG(x)                     ((x) << 1)

#define AD7280A_CTRL_HB_PWRDN_SW                        (1 << 0)
//@}

// Control Register Low Byte
//@{
#define AD7280A_CTRL_LB_SWRST                           (1 << 7)

#define AD7280A_CTRL_LB_ACQ_TIME_400ns                  (0 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_800ns                  (1 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_1200ns                 (2 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_1600ns                 (3 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME(x)                     ((x) << 5)

#define AD7280A_CTRL_LB_MUST_SET                        (1 << 4)
#define AD7280A_CTRL_LB_THERMISTOR_EN                   (1 << 3)
#define AD7280A_CTRL_LB_LOCK_DEV_ADDR                   (1 << 2)
#define AD7280A_CTRL_LB_INC_DEV_ADDR                    (1 << 1)
#define AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN               (1 << 0)
//@}

#define AD7280A_ALERT_GEN_STATIC_HIGH                   (1 << 6)
#define AD7280A_ALERT_RELAY_SIG_CHAIN_DOWN              (3 << 6)

#define AD7280A_ALL_CELLS                               (0xAD << 16)

#define AD7280A_MAX_CHAIN               8
#define AD7280A_CELLS_PER_DEV           6
#define AD7280A_BITS                    12
#define AD7280A_NUM_CH                  (AD7280A_AUX_ADC_6 - \
                                         AD7280A_CELL_VOLTAGE_1 + 1)

#define AD7280A_DEVADDR_MASTER          0
#define AD7280A_DEVADDR_ALL             0x1F
// 5-bit device address is sent LSB first
/*#define AD7280A_DEVADDR(addr)   (((addr & 0x1) << 4) | ((addr & 0x2) << 3) | \
                                (addr & 0x4) | ((addr & 0x8) >> 3) | \
                                ((addr & 0x10) >> 4))*/

/* During a read a valid write is mandatory.
 * So writing to the highest available address (Address 0x1F)
 * and setting the address all parts bit to 0 is recommended
 * So the TXVAL is AD7280A_DEVADDR_ALL + CRC
 */
#define AD7280A_READ_TXVAL      0xF800030A

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
void
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::initialize()
{
	XPCC__STATIC_ASSERT(N == 1, "Daisy chain length is currently limited to 1!");
	
	Cs::setOutput(xpcc::gpio::HIGH);
	Cnvst::setOutput(xpcc::gpio::HIGH);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::chainSetup()
{
	// Set reset bit for all devices
	write(ad7280a::MASTER, ad7280a::CTRL_LB, true,
			AD7280A_CTRL_LB_SWRST |
			AD7280A_CTRL_LB_MUST_SET |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN);
	
	// A single command should be sent to all devices in the
	// chain to assert the lock device address bit (D2), to deassert
	// the increment device address bit (D1), and to assert the
	// daisy-chain register readback bit (D0).
	write(ad7280a::MASTER, ad7280a::CTRL_LB, true,
			AD7280A_CTRL_LB_MUST_SET |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN);
	
	// A second command should be sent to all devices in the
	// chain to write the address of the lower byte of the control
	// register, 0x0E, to the read register on all devices.
	write(ad7280a::MASTER, ad7280a::READ, true, ad7280a::CTRL_LB << 2);
	
	// To verify that all AD7280As in the chain have received and
	// locked their unique device address, a daisy-chain register read
	// should be requested from all devices. This can be done by
	// continuing to apply sets of 32 SCLKs framed by CS until
	// the lower byte of the control register of each device in the
	// daisy chain has been read back. The user should confirm
	// that all device addresses are in sequence.
	// 
	// This command should be repeated until the control
	// register data has been read back from all devices in the
	// daisy chain.
	bool success = true;
	for (uint_fast8_t i = 0; i < 1; ++i)
	{
		ad7280a::RegisterValue reg;
		readRegister(&reg);
		
		// TODO check result
	}
	// -> 11 c2 65 dc
	// -> f9 c2 61 84
	
	return success;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
void
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::enableBalancer(uint8_t device, uint8_t cells)
{
	write(device, ad7280a::CELL_BALANCE, false, cells);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::performSelftest()
{
	// To select the self-test conversion, set Bits[D15:D14] of the
	// control register to 1, and set Bits[D13:D12] of the control
	// register to 0 on all parts.
	write(0, ad7280a::CTRL_HB, true, AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST);
	
	// Set Bit D0 of the control register to 1 on all parts. This
	// setting enables the daisy-chain register read operation on
	// all parts.
	write(0, ad7280a::CTRL_LB, true,
			AD7280A_CTRL_LB_MUST_SET |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN);
	
	// The register address corresponding to the self-test
	// conversion should be written to the read register of all
	// parts (see Table 13 for register addresses). The 32-bit write
	// command is 0x03 86 17 CA (see Table 29, Write 3).
	write(0, ad7280a::READ, true, ad7280a::SELF_TEST << 2);
	
	// Program the CNVST control register to 0x02 on all parts
	// to allow conversions to be initiated using the CNVST pin.
	// The 32-bit write command is 0x03 A0 54 6A (see Table 29,
	// Write 4).
	write(0, ad7280a::CNVST_CONTROL, true, 0x02);
	
	// Initiate conversions through the falling edge of CNVST.
	Cnvst::reset();
	
	// Allow sufficient time for the self-test conversions to be
	// completed plus tWAIT.
	xpcc::delay_ms(50);		// TODO
	Cnvst::set();
	
	// The CNVST control register should be programmed to
	// gate the CNVST signal on all parts. The 32-bit write
	// command is 0x03 A0 34 0A (see Table 29, Write 5). This
	// write prevents unintentional conversions from being
	// initiated by noise or glitches on the CNVST pin. This write
	// also updates the on-chip output registers of all devices in
	// the daisy chain.
	write(0, ad7280a::CNVST_CONTROL, true, 0x01);
	
	//XPCC_LOG_DEBUG << "result:" << xpcc::endl;
	// Apply a CS low pulse that frames 32 SCLKs to read back
	// the desired voltage. This frame should simultaneously
	// write the 32-bit command 0xF800030A, as described in
	// the Serial Interface section (see Table 29, Write 6).
	// 
	// The self-test conversion
	// result typically varies between Code 970 and Code 990.
	ad7280a::ConversionValue conversion;
	if (readConversionResult(&conversion)) {
		if (conversion.value > 970 && conversion.value < 990) {
			return true;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
void
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::softwareReset()
{
	// Set reset bit for all devices
	write(ad7280a::MASTER, ad7280a::CTRL_LB, true,
			AD7280A_CTRL_LB_SWRST |
			AD7280A_CTRL_LB_MUST_SET |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR | AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN);
	
	// Clear reset bit
	write(ad7280a::MASTER, ad7280a::CTRL_LB, true,
			AD7280A_CTRL_LB_MUST_SET |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR | AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::readChannel(uint8_t device,
		ad7280a::Channel channel, uint16_t *value)
{
	write(ad7280a::MASTER, ad7280a::CTRL_HB, true,
			AD7280A_CTRL_HB_CONV_INPUT_ALL |
			AD7280A_CTRL_HB_CONV_RES_READ_NO);
	
	write(device, ad7280a::CTRL_HB, false,
			AD7280A_CTRL_HB_CONV_INPUT_ALL |
			AD7280A_CTRL_HB_CONV_RES_READ_ALL |
			AD7280A_CTRL_HB_CONV_START_CS);
	
	// Wait for the conversion to finish
	xpcc::delay_ms(5);
	
	ad7280a::ConversionValue conversion;
	if (readConversionResult(&conversion) &&
			(conversion.device == device) &&
			(conversion.channel == channel))
	{
		*value = conversion.value;
		return true;
	}
	
	return false;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::readAllChannels(uint16_t *values)
{
	// Write Register Address 0x00 to the read register on all
	// parts. A device address of 0x00 is used when computing
	// the CRC for commands to write to all parts.
	write(0, ad7280a::READ, true, 0x00);
	
	write(0, ad7280a::CTRL_HB, true,
			AD7280A_CTRL_HB_CONV_INPUT_6CELL |
			AD7280A_CTRL_HB_CONV_RES_READ_6CELL);
	
	// Program the CNVST control register to 0x02 on all parts
	// to allow conversions to be initiated using the CNVST pin.
	write(0, ad7280a::CNVST_CONTROL, true, 0x02);
	
	// Initiate conversions through the falling edge of CNVST.
	Cnvst::reset();
	
	// Allow sufficient time for all conversions to be completed plus tWAIT.
	xpcc::delay_ms(5);
	Cnvst::set();
	
	// Apply a CS low pulse that frames 32 SCLKs to read back
	// the desired voltage. This frame should simultaneously
	// write the 32-bit command 0xF800030A, as described in
	// the Serial Interface section (see Table 29, Write 6).
	bool success = true;
	for (uint_fast8_t i = 0; i < 6; ++i)
	{
		ad7280a::ConversionValue result;
		if (readConversionResult(&result)) {
			*values = result.value;
		}
		else {
			success = false;
			*values = 0;
		}
		values++;
	}
	
	return success;
}

// ----------------------------------------------------------------------------
/*
 * P(x) = x^8 + x^5 + x^3 + x^2 + x^1 + x^0 = 0b100101111 => 0x2F
 */
template <typename Spi, typename Cs, typename Cnvst, int N>
uint8_t
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::updateCrc(uint8_t data)
{
	for (uint_fast8_t i = 0; i < 8; i++) {
		uint8_t bit = data & 0x80;
		data <<= 1;
		if (bit) {
			data ^= 0x2F;
		}
	}
	
	return data;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
uint8_t
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::calculateCrc(uint32_t data)
{
	uint8_t crc;
	
	crc = updateCrc((data >> 16) & 0xFF);
	crc = updateCrc(crc ^ ((data >> 8) & 0xFF));
	
	return  crc ^ (data & 0xFF);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::write(uint8_t device, ad7280a::Register reg,
		bool addressAll, uint8_t value)
{
	// The device address is send with LSB first
	uint8_t a = bitReverse(device);
	uint32_t t = (a << (27-3) | reg << 21 | value << 13 | addressAll << 12);
	
	t |= calculateCrc(t >> 11) << 3 | 0x2;
	
	Cs::reset();
	Spi::write((t >> 24) & 0xff);
	Spi::write((t >> 16) & 0xff);
	Spi::write((t >> 8) & 0xff);
	Spi::write((t >> 0) & 0xff);
	Cs::set();
	
	// TODO remove this
	xpcc::delay_us(1);
	return true;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::read(uint32_t *value)
{
	Cs::reset();
	*value  = Spi::write(0xF8) << 24;
	*value |= Spi::write(0x00) << 16;
	*value |= Spi::write(0x03) << 8;
	*value |= Spi::write(0x0A);
	Cs::set();
	
	uint8_t crc = calculateCrc(*value >> 10);
	if (crc == ((*value >> 2) & 0xff)) {
		return true;
	}
	else {
		//XPCC_LOG_DEBUG << "expected=" << crc << ", got=" << ((value >> 2) & 0xff) << xpcc::endl;
		return false;
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::readRegister(ad7280a::RegisterValue* result)
{
	uint32_t value;
	if (read(&value))
	{
		result->device = bitReverse(static_cast<uint8_t>(value >> 24)) & 0x1f;
		result->registerAddress = (value >> 21) & 0x3f;
		result->value = (value >> 13) & 0xff;
		result->acknowledge = (value & 0x400);
		
		return true;
	}
	
	return false;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Cnvst, int N>
bool
xpcc::Ad7280a<Spi, Cs, Cnvst, N>::readConversionResult(ad7280a::ConversionValue* result)
{
	uint32_t value;
	if (read(&value))
	{
		result->device = bitReverse(static_cast<uint8_t>(value >> 24)) & 0x1f;
		result->channel = (value >> 23) & 0x0f;
		result->value = (value >> 11) & 0xfff;
		result->acknowledge = (value & 0x400);
		
		return true;
	}
	
	return false;
}

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const ad7280a::RegisterValue& c)
{
	s	<< " (dev=" << c.device
		<< ", reg=" << c.registerAddress
		<< ", val=" << c.value
		<< ", ack=" << c.acknowledge << ")";
	
	return s;
}

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const ad7280a::ConversionValue& c)
{
	s	<< " (dev=" << c.device
		<< ", ch =" << c.channel
		<< ", val=" << c.value
		<< ", ack=" << c.acknowledge << ")";
	
	return s;
}
