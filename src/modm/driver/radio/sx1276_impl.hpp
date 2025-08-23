/*
 * Copyright (c) 2020, Benjamin Carrick
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SX1276_HPP
#   error "Don't include this file directly, use 'sx1276.hpp' instead!"
#endif

#include "sx1276.hpp"
#include <string.h>
#include <stdint.h>

namespace modm
{

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
Sx1276<SpiMaster, Cs>::Sx1276()
{
	//init the config shadow registers with the default values
	opModeShadow.value = 0x09;
	modemConf1Shadow.value = 0x72;
	modemConf2Shadow.value = 0x70;
	modemConf3Shadow.value = 0x00;
	irqMaskShadow.value = 0x00;
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::initialize()
{
	// set the mode to sleep
	changeMode(ModemMode::SLEEP);

	// change to the Lora mode
	opModeShadow.set(OpModeRegister::LoraMode);
	opModeShadow.reset(OpModeRegister::AccessSharedReg);
	opModeShadow.set(OpModeRegister::LowFrequencyMode);

	writeRegister(Sx1276Register::OpMode,opModeShadow.value);

	//mask out all interrupts
	irqMaskShadow.value = 0xFF;
	writeRegister(Sx1276Register::IrqFlagsMask,irqMaskShadow.value);

	// set the mode back to standby
	changeMode(ModemMode::STDBY);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::setCarrierFrequency(frequency_t freq)
{
	frfShadow = static_cast<uint32_t>(freq * (static_cast<float>(1<<19) / static_cast<float>(osc_freq)));

	// do a burst write to the three frequency registers instead of seperate register write calls
	buffer[0] = 0x80 | static_cast<uint8_t>(Sx1276Register::FrMsb);
	buffer[1] = static_cast<uint8_t>((frfShadow >> 16) & 0xFF);
	buffer[2] = static_cast<uint8_t>((frfShadow >> 8) & 0xFF);
	buffer[3] = static_cast<uint8_t>(frfShadow & 0xFF);

	transferBuffer(4);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::setModemParams(  Bandwidth bw,
										SpreadingFactor sf,
										CodingRate cr,
										bool implicitHeader,
										bool payloadCrc)
{
	//configure the modem config shadow registers
	Bandwidth_t::set(modemConf1Shadow,bw);
	CodingRate_t::set(modemConf1Shadow,cr);
	modemConf1Shadow.update(ModemConfig1::ImplicitHeaderModeOn,implicitHeader);

	SpreadingFactor_t::set(modemConf2Shadow,sf);
	modemConf2Shadow.reset(ModemConfig2::TxContinousMode);
	modemConf2Shadow.update(ModemConfig2::RxPayloadCrc,payloadCrc);

	//determine if the low bandwith optimization flag needs to be set
	if( (sf == SpreadingFactor::SF_7 && bw < Bandwidth::BW_1) ||
		(sf == SpreadingFactor::SF_8 && bw < Bandwidth::BW_3)  ||
		(sf == SpreadingFactor::SF_9 && bw < Bandwidth::BW_5)  ||
		(sf == SpreadingFactor::SF_10 && bw < Bandwidth::BW_7) ||
		(sf == SpreadingFactor::SF_11 && bw < Bandwidth::BW_8) ||
		(sf == SpreadingFactor::SF_12 && bw < Bandwidth::BW_9)
	  )
	{
		modemConf3Shadow.set(ModemConfig3::LowDataRateOptimize);
	}
	else
	{
		modemConf3Shadow.reset(ModemConfig3::LowDataRateOptimize);
	}

	// Write out the config registers
	writeRegister(Sx1276Register::ModemConfig1,modemConf1Shadow.value);
	writeRegister(Sx1276Register::ModemConfig2,modemConf2Shadow.value);
	writeRegister(Sx1276Register::ModemConfig3,modemConf3Shadow.value);

	//Configure the optimizations
	if(sf == SpreadingFactor::SF_6)
	{
		writeRegister(Sx1276Register::DetectOptimize,0x05);
		writeRegister(Sx1276Register::DetectionThreshold,0x0C);
	}
	else
	{
		writeRegister(Sx1276Register::DetectOptimize,0x03);
		writeRegister(Sx1276Register::DetectionThreshold,0x0A);
	}
}

// -----------------------------------------------------------------------------

template<typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::setSyncWord(uint8_t syncWord)
{
	writeRegister(Sx1276Register::SyncWord,syncWord);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
bool
Sx1276<SpiMaster, Cs>::transmit(uint8_t* data, uint8_t length)
{
	lastTransmitResult = false;

	//read the mode register and check if we're in the RX Mode
	opModeShadow.value = readRegister(Sx1276Register::OpMode);
	if(ModemMode_t::get(opModeShadow) == ModemMode::STDBY)
	{
		writeRegister(Sx1276Register::PayloadLength,length);
		writeRegister(Sx1276Register::FifoTxBaseAddr,0x00);
		writeRegister(Sx1276Register::FifoAddrPtr,0x00);

		modm::this_fiber::poll([&]{ return this->acquireMaster(); });

		buffer[0] = 0x80 | static_cast<uint8_t>(Sx1276Register::Fifo);

		Cs::reset();

		SpiMaster::transfer(buffer,nullptr,1);
		SpiMaster::transfer(data,nullptr,length);

		if(this->releaseMaster())
		{
			Cs::set();
		}

		changeMode(ModemMode::TX);
	}
	else
	{
		MODM_LOG_ERROR<<"SX1276: Transmission failed as the modem is busy"<<modm::endl;
	}

	return lastTransmitResult;
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::enableListening()
{
	//enable the rx interrupts
	irqMaskShadow.reset(Interrupts::RX_DONE);

	writeRegister(Sx1276Register::FifoAddrPtr,0x00);
	writeRegister(Sx1276Register::FifoRxBaseAddr,0x00);

	writeRegister(Sx1276Register::IrqFlagsMask,irqMaskShadow.value);
	writeRegister(Sx1276Register::IrqFlags, 0xFF);

	changeMode(ModemMode::RXCONTINOUS);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::disableListening()
{
	irqMaskShadow.set(Interrupts::RX_DONE);
	writeRegister(Sx1276Register::IrqFlagsMask,irqMaskShadow.value);

	changeMode(ModemMode::SLEEP);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
uint8_t
Sx1276<SpiMaster, Cs>::readPacket(uint8_t* data, uint8_t maxLength)
{
	lastPacketSize = 0;

	//read the mode register and check if we're in the RX Mode
	opModeShadow.value = readRegister(Sx1276Register::OpMode);
	if(ModemMode_t::get(opModeShadow) == ModemMode::RXCONTINOUS)
	{
		irqFlags.value = readRegister(Sx1276Register::IrqFlags);
		if(irqFlags & Interrupts::RX_DONE)
		{
			lastPacketSize = readRegister(Sx1276Register::RxNbBytes);
			modm::this_fiber::poll([&]{ return this->acquireMaster(); });

			buffer[0] = static_cast<uint8_t>(Sx1276Register::Fifo);

			Cs::reset();

			SpiMaster::transfer(buffer,nullptr,1);
			if(lastPacketSize > maxLength)
			{
				MODM_LOG_ERROR<<"SX1276: Read buffer is too small, packet discarded!"<<modm::endl;
				//read it out anyway to clean the fifo
				SpiMaster::transfer(nullptr,nullptr,lastPacketSize);
			}
			else
			{
				SpiMaster::transfer(nullptr,data,lastPacketSize);
			}

			if(this->releaseMaster())
			{
				Cs::set();
			}

			writeRegister(Sx1276Register::IrqFlags, static_cast<uint8_t>(Interrupts::RX_DONE));

		}
	}
	else
	{
		MODM_LOG_ERROR<<"SX1276: ReadPacket can only called when enabling listening first!"<<modm::endl;
	}

	return lastPacketSize;
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
int8_t
Sx1276<SpiMaster, Cs>::getPacketSnr()
{
	readRegister(Sx1276Register::PktSnrValue);
	return static_cast<int8_t>(buffer[1]);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
int16_t
Sx1276<SpiMaster, Cs>::getPacketRssi()
{
	tempRssi = static_cast<int16_t>(readRegister(Sx1276Register::PktRssiValue));
	tempRssi += rssiOffsetLF;
	return tempRssi;
}
// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
int16_t
Sx1276<SpiMaster, Cs>::getCurrentRssi()
{
	tempRssi = static_cast<int16_t>(readRegister(Sx1276Register::RssiValue));
	tempRssi += rssiOffsetLF;
	return tempRssi;
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::transferBuffer(uint8_t length)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	SpiMaster::transfer(buffer,buffer,length);

	if(this->releaseMaster())
	{
		Cs::set();
	}
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::writeRegister(Sx1276Register reg, uint8_t value)
{
	buffer[0] = 0x80 | static_cast<uint8_t>(reg);
	buffer[1] = value;

	transferBuffer(2);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
uint8_t
Sx1276<SpiMaster, Cs>::readRegister(Sx1276Register reg)
{
	buffer[0] = static_cast<uint8_t>(reg);
	buffer[1] = 0x00;

	transferBuffer(2);

	return buffer[1];
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Sx1276<SpiMaster, Cs>::changeMode(ModemMode mode)
{
	ModemMode_t::set(opModeShadow,mode);
	writeRegister(Sx1276Register::OpMode,opModeShadow.value);
}

}
