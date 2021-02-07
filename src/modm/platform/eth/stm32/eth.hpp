/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ETH_HPP
#define MODM_ETH_HPP

#include <modm/platform/gpio/connector.hpp>
#include <modm/architecture/interface.hpp>
#include <modm/math/utils/bit_constants.hpp>
#include "../device.hpp"

#include <array>
#include <cstring>

namespace modm
{
namespace platform
{

struct eth
{
	enum class
	MediaInterface : uint32_t {
		MII  = 0x00,
		RMII = SYSCFG_PMC_MII_RMII_SEL
	};

	enum class
	DuplexMode {
		Half = 0,
		Full = 1,
	};

	enum class
	Speed {
		Speed10M  = 0,
		Speed100M = 1,
	};

	enum class
	LinkStatus {
		Down = 0,
		Up,
	};

	enum class
	MacAddressIndex : uint32_t {
		Index0 = 0x00000000,
		Index1 = 0x00000008,
		Index2 = 0x00000010,
		Index3 = 0x00000018,
	};

	enum class
	Interrupt : uint32_t {
		NormalIrqSummary   		  = modm::Bit16,
		AbnormalIrqSummary        = modm::Bit15,
		EarlyReceive      		  = modm::Bit14,
		FatalBusError     		  = modm::Bit13,
		EarlyTransmit     		  = modm::Bit10,
		ReceiveWatchdog   	   	  = modm::Bit9,
		ReceiveStopped 		      = modm::Bit8,
		ReceiveBufferUnavailable  = modm::Bit7,
		Receive  				  = modm::Bit6,
		TransmitUnderflow		  = modm::Bit5,
		ReceiveOverflow           = modm::Bit4,
		TransmitJabberTimeout     = modm::Bit3,
		TransmitBufferUnavailable = modm::Bit2,
		TransmitStopped			  = modm::Bit1,
		Transmit 				  = modm::Bit0,
	};
	MODM_FLAGS32(Interrupt);

	enum class
	InterruptFlags : uint32_t {
		TimeStampTrigger  		  = modm::Bit29,
		Pmt               		  = modm::Bit28,
		Mmc               		  = modm::Bit27,
		ErrorBitStatus2			  = modm::Bit25,
		ErrorBitStatus1			  = modm::Bit24,
		ErrorBitStatus0			  = modm::Bit23,
		TransmitProcessState2	  = modm::Bit22,
		TransmitProcessState1	  = modm::Bit21,
		TransmitProcessState0	  = modm::Bit20,
		ReceiveProcessState2	  = modm::Bit19,
		ReceiveProcessState1	  = modm::Bit18,
		ReceiveProcessState0	  = modm::Bit17,
		NormalIrqSummary          = modm::Bit16,
		AbnormalIrqSummary        = modm::Bit15,
		EarlyReceive      		  = modm::Bit14,
		FatalBusError     		  = modm::Bit13,
		EarlyTransmit     		  = modm::Bit10,
		ReceiveWatchdog   	   	  = modm::Bit9,
		ReceiveStopped 		      = modm::Bit8,
		ReceiveBufferUnavailable  = modm::Bit7,
		Receive  				  = modm::Bit6,
		TransmitUnderflow		  = modm::Bit5,
		ReceiveOverflow           = modm::Bit4,
		TransmitJabberTimeout     = modm::Bit3,
		TransmitBufferUnavailable = modm::Bit2,
		TransmitStopped			  = modm::Bit1,
		Transmit 				  = modm::Bit0,
	};
	MODM_FLAGS32(InterruptFlags);

	enum class
	Event : uint32_t {
		None     = 0x00,
		Receive  = 0x01,
		Transmit = 0x02,
		Error    = 0x04,
	};
	MODM_FLAGS32(Event);

protected:
	static constexpr uint32_t i(MediaInterface interface) { return uint32_t(interface); }
};

template <class PHY>
class Eth : public eth
{
	enum class
	MacConfiguration : uint32_t {
		WatchDogDisable     = modm::Bit23,
		JabberDisable       = modm::Bit22,
		InterframeGap2      = modm::Bit19,
		InterframeGap1      = modm::Bit18,
		InterframeGap0      = modm::Bit17,
		CarrierSense        = modm::Bit16,
		EthernetSpeed       = modm::Bit14,
		ReceiveOwn          = modm::Bit13,
		LoopBack            = modm::Bit12,
		DuplexMode          = modm::Bit11,
		Ipv4ChecksumOffLoad = modm::Bit10,
		RetryDisable        = modm::Bit9,
		AutomaticPad        = modm::Bit7,
		BackOffLimit1       = modm::Bit6,
		BackOffLimit0       = modm::Bit5,
		DeferalCheck        = modm::Bit4,
		TransmitterEnable   = modm::Bit3,
		ReceiveEnable       = modm::Bit2,
	};
	MODM_FLAGS32(MacConfiguration);

	static constexpr uint32_t MacCrClearMask { 0xFF20810F };

	enum class
	Watchdog : uint32_t {
		Enable  = 0b0,
		Disable = 0b1,
	};
	using Watchdog_t = modm::Configuration<MacConfiguration_t, Watchdog, 0b1, 23>;

	enum class
	Jabber : uint32_t {
		Enable  = 0b0,
		Disable = 0b1,
	};
	using Jabber_t = modm::Configuration<MacConfiguration_t, Jabber, 0b1, 22>;

	enum class
	InterframeGap : uint32_t {
		Gap96BitTimes = 0b000,
		Gap88BitTimes = 0b001,
		Gap80BitTimes = 0b010,
		Gap72BitTimes = 0b011,
		Gap64BitTimes = 0b100,
		Gap56BitTimes = 0b101,
		Gap48BitTimes = 0b110,
		Gap40BitTimes = 0b111,
	};
	using InterframeGap_t = modm::Configuration<MacConfiguration_t, InterframeGap, 0b111, 17>;

	enum class
	CarrierSense : uint32_t {
		Enable  = 0b0,
		Disable = 0b1,
	};
	using CarrierSense_t = modm::Configuration<MacConfiguration_t, CarrierSense, 0b1, 16>;

	using Speed_t = modm::Configuration<MacConfiguration_t, Speed, 0b1, 14>;
	using DuplexMode_t = modm::Configuration<MacConfiguration_t, DuplexMode, 0b1, 11>;

	enum class
	MacFrameFilter : uint32_t {
		ReceiveAll                = modm::Bit31,
		HashOrPerfect             = modm::Bit10,
		SourceAddress             = modm::Bit9,
		SourceAddressInverse      = modm::Bit8,
		PassControl1              = modm::Bit7,
		PassControl0              = modm::Bit6,
		BroadcastDisable          = modm::Bit5,
		PassAllMulticast          = modm::Bit4,
		DestinationAddressInverse = modm::Bit3,
		HashMulticast             = modm::Bit2,
		HasUnicast                = modm::Bit1,
		PromiscuousMode           = modm::Bit0,
	};
	MODM_FLAGS32(MacFrameFilter);

	enum class
	PassControlFrame : uint32_t {
		BlockAll = 0b00,
		AllExceptPause = 0b01,
		AllAddressFilterFail = 0b10,
		AllAddressFilterPass = 0b11,
	};
	using PassControlFrame_t = modm::Configuration<MacFrameFilter_t, PassControlFrame, 0b11, 6>;

	enum class
	MacFlowControl : uint32_t {
		PauseTime15 = modm::Bit31,
		PauseTime14 = modm::Bit30,
		PauseTime13 = modm::Bit29,
		PauseTime12 = modm::Bit28,
		PauseTime11 = modm::Bit27,
		PauseTime10 = modm::Bit26,
		PauseTime9  = modm::Bit25,
		PauseTime8  = modm::Bit24,
		PauseTime7  = modm::Bit23,
		PauseTime6  = modm::Bit22,
		PauseTime5  = modm::Bit21,
		PauseTime4  = modm::Bit20,
		PauseTime3  = modm::Bit19,
		PauseTime2  = modm::Bit18,
		PauseTime1  = modm::Bit17,
		PauseTime0  = modm::Bit16,
		ZeroQuantaPauseDisable    = modm::Bit7,
		PauseLowThreshold1        = modm::Bit5,
		PauseLowThreshold0        = modm::Bit4,
		UnicastPauseDetect        = modm::Bit3,
		ReceiveFlowControlEnable  = modm::Bit2,
		TransmitFlowControlEnable = modm::Bit1,
		FlowControlBusy           = modm::Bit0,
	};
	MODM_FLAGS32(MacFlowControl);

	static constexpr uint32_t MacFcrClearMask { 0x0000FF41 };

	enum class
	PauseLowThreshold : uint32_t {
		Minus4SlotTimes   = 0b00,
		Minus28SlotTimes  = 0b01,
		Minus144SlotTimes = 0b10,
		Minus256SlotTimes = 0b11,
	};
	using PauseLowThreshold_t = modm::Configuration<MacFlowControl_t, PauseLowThreshold, 0b11, 4>;

	enum class
	DmaOperationMode : uint32_t {
		DropCrcErrorFrameDisable    = modm::Bit26,
		ReceiveStoreAndForward      = modm::Bit25,
		DisableFlushReceivedFrames  = modm::Bit24,
		TransmitStoreAndForward     = modm::Bit21,
		FlushTransmitFifo           = modm::Bit20,
		TransmitThreshold2          = modm::Bit16,
		TransmitThreshold1          = modm::Bit15,
		TransmitThreshold0          = modm::Bit14,
		StartTransmission           = modm::Bit13,
		ForwardErrorFrames          = modm::Bit7,
		ForwardUndersizedGoodFrames = modm::Bit6,
		ReceiveThreshold1           = modm::Bit4,
		ReceiveThreshold0           = modm::Bit3,
		OperateOnSecondFrame        = modm::Bit2,
		StartReceive                = modm::Bit1,
	};
	MODM_FLAGS32(DmaOperationMode);

	static constexpr uint32_t DmaOmrClearMask { 0xF8DE3F23 };

	enum class
	TransmitThreshold : uint32_t {
		Bytes64  = 0b000,
		Bytes128 = 0b001,
		Bytes192 = 0b010,
		Bytes256 = 0b011,
		Bytes40  = 0b100,
		Bytes32  = 0b101,
		Bytes24  = 0b110,
		Bytes16  = 0b111,
	};
	using TransmitThreshold_t = modm::Configuration<DmaOperationMode_t, TransmitThreshold, 0b111, 14>;

	enum class
	ReceiveThreshold : uint32_t {
		Bytes64  = 0b00,
		Bytes32  = 0b01,
		Bytes96  = 0b10,
		Bytes128 = 0b11,
	};
	using ReceiveThreshold_t = modm::Configuration<DmaOperationMode_t, ReceiveThreshold, 0b11, 3>;

	enum class
	DmaBusMode : uint32_t {
		MixedBurst            = modm::Bit26,
		AddressAlignedBeats   = modm::Bit25,
		PblModeX4             = modm::Bit24,
		UseSeparatePbl        = modm::Bit23,
		RxDmaPbl5             = modm::Bit22,
		RxDmaPbl4             = modm::Bit21,
		RxDmaPbl3             = modm::Bit20,
		RxDmaPbl2             = modm::Bit19,
		RxDmaPbl1             = modm::Bit18,
		RxDmaPbl0             = modm::Bit17,
		FixedBurst            = modm::Bit16,
		RxTxPriorityRatio1    = modm::Bit15,
		RxTxPriorityRatio0    = modm::Bit14,
		Pbl5                  = modm::Bit13,
		Pbl4                  = modm::Bit12,
		Pbl3                  = modm::Bit11,
		Pbl2                  = modm::Bit10,
		Pbl1                  = modm::Bit9,
		Pbl0                  = modm::Bit8,
		EnhancedDescFormat    = modm::Bit7,
		DescriptorSkipLength4 = modm::Bit6,
		DescriptorSkipLength3 = modm::Bit5,
		DescriptorSkipLength2 = modm::Bit4,
		DescriptorSkipLength1 = modm::Bit3,
		DescriptorSkipLength0 = modm::Bit2,
		DmaArbitration        = modm::Bit1,
		SoftwareReset         = modm::Bit0,
	};
	MODM_FLAGS32(DmaBusMode);

	enum class
	BurstLength : uint32_t {
		Length1Beat   = 0b000001,
		Length2Beats  = 0b000010,
		Length4Beats  = 0b000100,
		Length8Beats  = 0b001000,
		Length16Beats = 0b010000,
		Length32Beats = 0b100000,
	};
	using BurstLength_t = modm::Configuration<DmaBusMode_t, BurstLength, 0b111111, 8>;
	using RxDmaBurstLength_t = modm::Configuration<DmaBusMode_t, BurstLength, 0b111111, 17>;

	template <Peripheral peripheral, template <Peripheral _> class... Signals>
	struct GpioConfigurator;
	template <Peripheral peripheral, template <Peripheral _> class Signal, template <Peripheral _> class... Signals>
	struct GpioConfigurator<peripheral, Signal, Signals...>
	{
		static inline void
		configure() {
			Signal<peripheral>::Gpio::configure(modm::platform::Gpio::OutputType::PushPull, modm::platform::Gpio::OutputSpeed::VeryHigh);
			GpioConfigurator<peripheral, Signals...>::configure();
		}
	};
	template <Peripheral peripheral>
	struct GpioConfigurator<peripheral>
	{
		static inline void
		configure() {}
	};

public:
	template <template<Peripheral _> class... Signals>
	static void
	connect()
	{
		using Configurator = GpioConfigurator<Peripheral::Eth, Signals...>;
		using Connector = GpioConnector<Peripheral::Eth, Signals...>;

		Configurator::configure();
		Connector::connect();
	}

	template <MediaInterface Interface>
	static inline bool
	initialize(uint8_t priority=5)
	{
		using namespace modm::literals;

		Rcc::enable<Peripheral::Eth>();

		NVIC_SetPriority(ETH_IRQn, priority);
		NVIC_EnableIRQ(ETH_IRQn);

		/* Select MII or RMII Mode*/
		SYSCFG->PMC &= ~(SYSCFG_PMC_MII_RMII_SEL);
		SYSCFG->PMC |= uint32_t(Interface);

		/* Ethernet Software reset */
		/* Set the SWR bit: resets all MAC subsystem internal registers and logic */
		/* After reset all the registers holds their respective reset values */
		ETH->DMABMR |= DmaBusMode_t(DmaBusMode::SoftwareReset | DmaBusMode::EnhancedDescFormat).value;

		/* Wait for software reset */
		/* Note: The SWR is not performed if the ETH_RX_CLK or the ETH_TX_CLK are
		 * not available, please check your external PHY or the IO configuration */
		int timeout = 1'000; // max 1ms
		while ((DmaBusMode(ETH->DMABMR) & DmaBusMode_t(DmaBusMode::SoftwareReset)) and (timeout-- > 0)) {
			// Wait until the PHY has reset.
			modm::delay_us(1);

			// NOTE: If the program hangs here check MII/RMII value.
		}
		if (not modm_assert_continue_fail_debug(timeout > 0, "eth.init",
				"ETH::initialize() timed out on software reset!", 1))
			return false;

		/* Configure SMI clock range */
		uint32_t csr_clock_divider = ETH->MACMIIAR & ETH_MACMIIAR_CR_Msk;
		if (SystemCoreClock >= 20_MHz and SystemCoreClock < 35_MHz)
				csr_clock_divider |= ETH_MACMIIAR_CR_Div16;
		else if (SystemCoreClock >= 35_MHz and SystemCoreClock < 60_MHz)
				csr_clock_divider |= ETH_MACMIIAR_CR_Div26;
		else if (SystemCoreClock >= 60_MHz and SystemCoreClock < 100_MHz)
				csr_clock_divider |= ETH_MACMIIAR_CR_Div42;
		else if (SystemCoreClock >= 100_MHz and SystemCoreClock < 150_MHz)
				csr_clock_divider |= ETH_MACMIIAR_CR_Div62;
		else if (SystemCoreClock >= 150_MHz)
				csr_clock_divider |= ETH_MACMIIAR_CR_Div102;

		ETH->MACMIIAR = csr_clock_divider;

		// Initialize PHY
		uint32_t phy_register { 0 };

		/* Reset */
		(void) readPhyRegister(PHY::Register::BCR, phy_register);
		phy_register |= PHY::Reset;
		if (not writePhyRegister(PHY::Register::BCR, phy_register)) {
			configureMac(true);
			configureDma();
			return false;
		}

		// wait for reset done
		modm::delay_us(PHY::ResetDelay);
		timeout = 1'000;

		do {
			(void) readPhyRegister(PHY::Register::BCR, phy_register);
			if ((phy_register & PHY::Reset) == 0)
				break;
			modm::delay_ms(1);
		} while (timeout-- > 0);
		if (timeout <= 0) {
			linkStatus = LinkStatus::Down;
			return false;
		}

		// FIXME: Delegate these values to the PHY
		// configure auto negotiation
		phy_register = 0x100 // 100 FD
				| 0x80 // 100
				| 0x40 // 10 FD
				| 0x20 // 10
				| 0x01 // 802.3
				;
		(void) writePhyRegister(PHY::Register::AN, phy_register);

		configureMac(true);
		configureDma();

		return true;
	}

	static void
	configureMac(bool autoNegotiationFailed = false)
	{
		uint32_t tmp;

		if (autoNegotiationFailed) {
			duplexMode = DuplexMode::Full;
			speed = Speed::Speed100M;
		}

		MacConfiguration_t maccr {
			MacConfiguration::Ipv4ChecksumOffLoad |
			MacConfiguration::RetryDisable
		};
		maccr |= Speed_t(speed);
		maccr |= DuplexMode_t(duplexMode);

		tmp = ETH->MACCR & MacCrClearMask;
		tmp |= maccr.value;
		writeMACCR(tmp);

		MacFrameFilter_t macffr {
			PassControlFrame_t(PassControlFrame::BlockAll),
		};
		writeMACFFR(macffr.value);

		// Hash table
		ETH->MACHTHR = 0x00000000;
		ETH->MACHTLR = 0x00000000;

		MacFlowControl_t macfcr {
			MacFlowControl::ZeroQuantaPauseDisable
		};
		tmp = ETH->MACFCR & MacFcrClearMask;
		tmp |= macfcr.value;
		writeMACFCR(tmp);

		// no VLAN support for now
		writeMACVLANTR(0x00000000);
	}

	static void
	configureDma()
	{
		uint32_t tmp;

		DmaOperationMode_t dmaomr {
			DmaOperationMode::ReceiveStoreAndForward |
			DmaOperationMode::TransmitStoreAndForward |
			DmaOperationMode::OperateOnSecondFrame
		};
		tmp = ETH->DMAOMR & DmaOmrClearMask;
		tmp |= dmaomr.value;
		writeDMAOMR(tmp);

		DmaBusMode_t dmabmr {
			DmaBusMode::AddressAlignedBeats |
			DmaBusMode::FixedBurst |
			DmaBusMode::EnhancedDescFormat |
			DmaBusMode::UseSeparatePbl
		};
		dmabmr |= BurstLength_t(BurstLength::Length32Beats);
		dmabmr |= RxDmaBurstLength_t(BurstLength::Length32Beats);
		writeDMABMR(dmabmr.value);

		enableInterrupt(Interrupt::NormalIrqSummary | Interrupt::Receive);

		configureMacAddresses();
	}

	static void
	setMacAddress(MacAddressIndex index, uint8_t const *macAddress)
	{
		std::memcpy(macAddresses[uint32_t(index) / 8].data(), macAddress, 6);
	}

	static void
	configureMacAddresses();

	static void
	start();
	static void
	stop();

	static void
	setDmaTxDescriptorTable(uint32_t address) {
		ETH->DMATDLAR = address;
	}
	static void
	setDmaRxDescriptorTable(uint32_t address) {
		ETH->DMARDLAR = address;
	}

	static InterruptFlags
	getInterruptFlags() {
		return InterruptFlags(ETH->DMASR);
	}
	static void
	acknowledgeInterrupt(InterruptFlags_t irq) {
		// set only the bits you want to clear!
		// using an |= here would clear other fields as well
		ETH->DMASR = irq.value;
	}
	static void
	enableInterrupt(Interrupt_t irq) {
		ETH->DMAIER |= irq.value;
	}

	// FIXME: Make this more generic by delegating specifics to the PHY
	static bool
	phyStartAutoNegotiation()
	{
		uint32_t phy_register { 0 };

		// enable auto-negotiation
		(void) readPhyRegister(PHY::Register::BCR, phy_register);
		phy_register |= PHY::RestartAutoNegotiation;
		if (not writePhyRegister(PHY::Register::BCR, phy_register))
			return false;

		// wait for auto-negotiation complete (5s)
		int timeout = 5'000;
		do {
			(void) readPhyRegister(PHY::Register::BSR, phy_register);
			if ((phy_register & PHY::AutoNegotiationComplete) == PHY::AutoNegotiationComplete)
				break;
			modm::delay_ms(1);
		} while (timeout-- > 0);
		if (timeout <= 0)
			return false;

		// read auto-negotiation result
		if (not readPhyRegister(PHY::Register::SR, phy_register))
			return false;

		if ((phy_register & PHY::DuplexStatus) == PHY::DuplexStatus)
			duplexMode = DuplexMode::Full;
		else
			duplexMode = DuplexMode::Half;

		if ((phy_register & PHY::SpeedStatus) == PHY::SpeedStatus)
			speed = Speed::Speed10M;
		else
			speed = Speed::Speed100M;

		return true;
	}

	static LinkStatus
	phyReadLinkStatus()
	{
		uint32_t phy_register { 0 };

		(void) readPhyRegister(PHY::Register::BSR, phy_register);
		if ((phy_register & PHY::LinkedStatus) == PHY::LinkedStatus)
			linkStatus = LinkStatus::Up;
		else
			linkStatus = LinkStatus::Down;

		return linkStatus;
	}
	static LinkStatus
	getLinkStatus() {
		return linkStatus;
	}

private:
	static void
	writeMACCR(uint32_t value) {
		ETH->MACCR = value;
		(void) ETH->MACCR;
		modm::delay_ms(1);
		ETH->MACCR = value;
	}
	static void
	writeMACFCR(uint32_t value) {
		ETH->MACFCR = value;
		(void) ETH->MACFCR;
		modm::delay_ms(1);
		ETH->MACFCR = value;
	}
	static void
	writeMACFFR(uint32_t value) {
		ETH->MACFFR = value;
		(void) ETH->MACFFR;
		modm::delay_ms(1);
		ETH->MACFFR = value;
	}
	static void
	writeMACVLANTR(uint32_t value) {
		ETH->MACVLANTR = value;
		(void) ETH->MACVLANTR;
		modm::delay_ms(1);
		ETH->MACVLANTR = value;
	}
	static void
	writeDMABMR(uint32_t value) {
		ETH->DMABMR= value;
		(void) ETH->DMABMR;
		modm::delay_ms(1);
		ETH->DMABMR = value;
	}
	static void
	writeDMAOMR(uint32_t value) {
		ETH->DMAOMR= value;
		(void) ETH->DMAOMR;
		modm::delay_ms(1);
		ETH->DMAOMR = value;
	}

	static bool
	writePhyRegister(uint16_t reg, uint32_t value);
	static bool
	readPhyRegister(uint16_t reg, uint32_t &value);

	static inline DuplexMode duplexMode = eth::DuplexMode::Full;
	static inline Speed speed = eth::Speed::Speed100M;
	static inline LinkStatus linkStatus = eth::LinkStatus::Down;

	using MacAddress = std::array<uint8_t, 6>;
	using MacAddresses = std::array<MacAddress, 4>;
	static inline MacAddresses macAddresses;
};

}
}

#include "eth_impl.hpp"

#endif // MODM_ETH_HPP
