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
 */		
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__DMA_HPP
#define	XPCC_ATXMEGA__DMA_HPP

namespace xpcc
{
	namespace xmega
	{
		/**
		 * \brief Configuration options for the DMA controller and channels.
		 *
		 * \author	Niklas Hauser
		 * \ingroup xmega_dma
		 */
		namespace dma
		{
			// DmaController
			enum ChannelPriority
			{
				PRIORITY_RR0123 = DMA_PRIMODE_RR0123_gc,		///< Round Robin
				PRIORITY_CH0RR123 = DMA_PRIMODE_CH0RR123_gc, ///< Channel 0 > Round Robin on channel 1/2/3
				PRIORITY_CH01RR23 =  DMA_PRIMODE_CH01RR23_gc,///< Channel 0 > channel 1 > Round Robin on channel 2/3
				PRIORITY_CH0123 = DMA_PRIMODE_CH0123_gc		///< Channel 0 > channel 1 > channel 2 > channel 3
			};
			
			enum DoubleBuffer
			{
				DOUBLE_BUFFER_DISABLED = DMA_DBUFMODE_DISABLED_gc,///< Double buffering disabled
				DOUBLE_BUFFER_CH01 = DMA_DBUFMODE_CH01_gc,		///< Double buffering enabled on channel 0/1
				DOUBLE_BUFFER_CH23 = DMA_DBUFMODE_CH23_gc,		///< Double buffering enabled on channel 2/3
				DOUBLE_BUFFER_CH01CH23 = DMA_DBUFMODE_CH01CH23_gc	///< Double buffering enabled on ch. 0/1 and ch. 2/3
			};
			
			// DmaChannel
			enum BurstLength
			{
				BURST_1BYTE = DMA_CH_BURSTLEN_1BYTE_gc,
				BURST_2BYTE = DMA_CH_BURSTLEN_2BYTE_gc,
				BURST_4BYTE = DMA_CH_BURSTLEN_4BYTE_gc,
				BURST_8BYTE = DMA_CH_BURSTLEN_8BYTE_gc
			};
			
			enum Reload
			{
				RELOAD_NONE = 0x00,
				RELOAD_BLOCK = 0x01,
				RELOAD_BURST = 0x02,
				RELOAD_TRANSACTION = 0x03
			};
			
			enum AddressMode
			{
				ADDRESS_FIXED = 0x00,
				ADDRESS_INCREMENT = 0x01,
				ADDRESS_DECREMENT = 0x02
			};
			
			enum TriggerSource
			{
				TRIGGER_SOFTWARE = DMA_CH_TRIGSRC_OFF_gc,		///< Off software triggers only
				TRIGGER_EVENT_SYSTEM_CH0 = DMA_CH_TRIGSRC_EVSYS_CH0_gc,	///< Event System Channel 0
				TRIGGER_EVENT_SYSTEM_CH1 = DMA_CH_TRIGSRC_EVSYS_CH1_gc,	///< Event System Channel 1
				TRIGGER_EVENT_SYSTEM_CH2 = DMA_CH_TRIGSRC_EVSYS_CH2_gc,	///< Event System Channel 2
				TRIGGER_ADCA_CH0 = DMA_CH_TRIGSRC_ADCA_CH0_gc,			///< ADCA Channel 0
				TRIGGER_ADCA_CH1 = DMA_CH_TRIGSRC_ADCA_CH1_gc,			///< ADCA Channel 1
				TRIGGER_ADCA_CH2 = DMA_CH_TRIGSRC_ADCA_CH2_gc,			///< ADCA Channel 2
				TRIGGER_ADCA_CH3 = DMA_CH_TRIGSRC_ADCA_CH3_gc,			///< ADCA Channel 3
				TRIGGER_ADCA_CH0123 = DMA_CH_TRIGSRC_ADCA_CH4_gc,		///< ADCA Channel 0,1,2,3 combined
				TRIGGER_DACA_CH0 = DMA_CH_TRIGSRC_DACA_CH0_gc,			///< DACA Channel 0
				TRIGGER_DACA_CH1 = DMA_CH_TRIGSRC_DACA_CH1_gc,			///< DACA Channel 1
				TRIGGER_ADCB_CH0 = DMA_CH_TRIGSRC_ADCB_CH0_gc,			///< ADCB Channel 0
				TRIGGER_ADCB_CH1 = DMA_CH_TRIGSRC_ADCB_CH1_gc,			///< ADCB Channel 1
				TRIGGER_ADCB_CH2 = DMA_CH_TRIGSRC_ADCB_CH2_gc,			///< ADCB Channel 2
				TRIGGER_ADCB_CH3 = DMA_CH_TRIGSRC_ADCB_CH3_gc,			///< ADCB Channel 3
				TRIGGER_ADCB_CH0123 = DMA_CH_TRIGSRC_ADCB_CH4_gc,		///< ADCB Channel 0,1,2,3 combined
				TRIGGER_DACB_CH0 = DMA_CH_TRIGSRC_DACB_CH0_gc,			///< DACB Channel 0
				TRIGGER_DACB_CH1 = DMA_CH_TRIGSRC_DACB_CH1_gc,			///< DACB Channel 1
				TRIGGER_TCC0_OVF = DMA_CH_TRIGSRC_TCC0_OVF_gc,			///< Timer/Counter C0 Overflow
				TRIGGER_TCC0_ERR = DMA_CH_TRIGSRC_TCC0_ERR_gc,			///< Timer/Counter C0 Error
				TRIGGER_TCC0_CCA = DMA_CH_TRIGSRC_TCC0_CCA_gc,			///< Timer/Counter C0 Compare or Capture A
				TRIGGER_TCC0_CCB = DMA_CH_TRIGSRC_TCC0_CCB_gc,			///< Timer/Counter C0 Compare or Capture B
				TRIGGER_TCC0_CCC = DMA_CH_TRIGSRC_TCC0_CCC_gc,			///< Timer/Counter C0 Compare or Capture C
				TRIGGER_TCC0_CCD = DMA_CH_TRIGSRC_TCC0_CCD_gc,			///< Timer/Counter C0 Compare or Capture D
				TRIGGER_TCC1_OVF = DMA_CH_TRIGSRC_TCC1_OVF_gc,			///< Timer/Counter C1 Overflow
				TRIGGER_TCC1_ERR = DMA_CH_TRIGSRC_TCC1_ERR_gc,			///< Timer/Counter C1 Error
				TRIGGER_TCC1_CCA = DMA_CH_TRIGSRC_TCC1_CCA_gc,			///< Timer/Counter C1 Compare or Capture A
				TRIGGER_TCC1_CCB = DMA_CH_TRIGSRC_TCC1_CCB_gc,			///< Timer/Counter C1 Compare or Capture B
				TRIGGER_DMA_SPIC = DMA_CH_TRIGSRC_SPIC_gc,				///< SPI C Transfer Complete
				TRIGGER_DMA_USARTC0_RXC = DMA_CH_TRIGSRC_USARTC0_RXC_gc,///< USART C0 Receive Complete
				TRIGGER_DMA_USARTC0_DRE = DMA_CH_TRIGSRC_USARTC0_DRE_gc,///< USART C0 Data Register Empty
				TRIGGER_DMA_USARTC1_RXC = DMA_CH_TRIGSRC_USARTC1_RXC_gc,///< USART C1 Receive Complete
				TRIGGER_DMA_USARTC1_DRE = DMA_CH_TRIGSRC_USARTC1_DRE_gc,///< USART C1 Data Register Empty
				TRIGGER_TCD0_OVF = DMA_CH_TRIGSRC_TCD0_OVF_gc,			///< Timer/Counter D0 Overflow
				TRIGGER_TCD0_ERR = DMA_CH_TRIGSRC_TCD0_ERR_gc,			///< Timer/Counter D0 Error
				TRIGGER_TCD0_CCA = DMA_CH_TRIGSRC_TCD0_CCA_gc,			///< Timer/Counter D0 Compare or Capture A
				TRIGGER_TCD0_CCB = DMA_CH_TRIGSRC_TCD0_CCB_gc,			///< Timer/Counter D0 Compare or Capture B
				TRIGGER_TCD0_CCC = DMA_CH_TRIGSRC_TCD0_CCC_gc,			///< Timer/Counter D0 Compare or Capture C
				TRIGGER_TCD0_CCD = DMA_CH_TRIGSRC_TCD0_CCD_gc,			///< Timer/Counter D0 Compare or Capture D
				TRIGGER_TCD1_OVF = DMA_CH_TRIGSRC_TCD1_OVF_gc,			///< Timer/Counter D1 Overflow
				TRIGGER_TCD1_ERR = DMA_CH_TRIGSRC_TCD1_ERR_gc,			///< Timer/Counter D1 Error
				TRIGGER_TCD1_CCA = DMA_CH_TRIGSRC_TCD1_CCA_gc,			///< Timer/Counter D1 Compare or Capture A
				TRIGGER_TCD1_CCB = DMA_CH_TRIGSRC_TCD1_CCB_gc,			///< Timer/Counter D1 Compare or Capture B
				TRIGGER_DMA_SPID = DMA_CH_TRIGSRC_SPID_gc,				///< SPI D Transfer Complete
				TRIGGER_DMA_USARTD0_RXC = DMA_CH_TRIGSRC_USARTD0_RXC_gc,///< USART D0 Receive Complete
				TRIGGER_DMA_USARTD0_DRE = DMA_CH_TRIGSRC_USARTD0_DRE_gc,///< USART D0 Data Register Empty
				TRIGGER_DMA_USARTD1_RXC = DMA_CH_TRIGSRC_USARTD1_RXC_gc,///< USART D1 Receive Complete
				TRIGGER_DMA_USARTD1_DRE = DMA_CH_TRIGSRC_USARTD1_DRE_gc,///< USART D1 Data Register Empty
				TRIGGER_TCE0_OVF = DMA_CH_TRIGSRC_TCE0_OVF_gc,			///< Timer/Counter E0 Overflow
				TRIGGER_TCE0_ERR = DMA_CH_TRIGSRC_TCE0_ERR_gc,			///< Timer/Counter E0 Error
				TRIGGER_TCE0_CCA = DMA_CH_TRIGSRC_TCE0_CCA_gc,			///< Timer/Counter E0 Compare or Capture A
				TRIGGER_TCE0_CCB = DMA_CH_TRIGSRC_TCE0_CCB_gc,			///< Timer/Counter E0 Compare or Capture B
				TRIGGER_TCE0_CCC = DMA_CH_TRIGSRC_TCE0_CCC_gc,			///< Timer/Counter E0 Compare or Capture C
				TRIGGER_TCE0_CCD = DMA_CH_TRIGSRC_TCE0_CCD_gc,			///< Timer/Counter E0 Compare or Capture D
				TRIGGER_TCE1_OVF = DMA_CH_TRIGSRC_TCE1_OVF_gc,			///< Timer/Counter E1 Overflow
				TRIGGER_TCE1_ERR = DMA_CH_TRIGSRC_TCE1_ERR_gc,			///< Timer/Counter E1 Error
				TRIGGER_TCE1_CCA = DMA_CH_TRIGSRC_TCE1_CCA_gc,			///< Timer/Counter E1 Compare or Capture A
				TRIGGER_TCE1_CCB = DMA_CH_TRIGSRC_TCE1_CCB_gc,			///< Timer/Counter E1 Compare or Capture B
				TRIGGER_DMA_SPIE = DMA_CH_TRIGSRC_SPIE_gc,				///< SPI E Transfer Complete
				TRIGGER_DMA_USARTE0_RXC = DMA_CH_TRIGSRC_USARTE0_RXC_gc,///< USART E0 Receive Complete
				TRIGGER_DMA_USARTE0_DRE = DMA_CH_TRIGSRC_USARTE0_DRE_gc,///< USART E0 Data Register Empty
				TRIGGER_DMA_USARTE1_RXC = DMA_CH_TRIGSRC_USARTE1_RXC_gc,///< USART E1 Receive Complete
				TRIGGER_DMA_USARTE1_DRE = DMA_CH_TRIGSRC_USARTE1_DRE_gc,///< USART E1 Data Register Empty
				TRIGGER_TCF0_OVF = DMA_CH_TRIGSRC_TCF0_OVF_gc,			///< Timer/Counter F0 Overflow
				TRIGGER_TCF0_ERR = DMA_CH_TRIGSRC_TCF0_ERR_gc,			///< Timer/Counter F0 Error
				TRIGGER_TCF0_CCA = DMA_CH_TRIGSRC_TCF0_CCA_gc,			///< Timer/Counter F0 Compare or Capture A
				TRIGGER_TCF0_CCB = DMA_CH_TRIGSRC_TCF0_CCB_gc,			///< Timer/Counter F0 Compare or Capture B
				TRIGGER_TCF0_CCC = DMA_CH_TRIGSRC_TCF0_CCC_gc,			///< Timer/Counter F0 Compare or Capture C
				TRIGGER_TCF0_CCD = DMA_CH_TRIGSRC_TCF0_CCD_gc,			///< Timer/Counter F0 Compare or Capture D
				TRIGGER_TCF1_OVF = DMA_CH_TRIGSRC_TCF1_OVF_gc,			///< Timer/Counter F1 Overflow
				TRIGGER_TCF1_ERR = DMA_CH_TRIGSRC_TCF1_ERR_gc,			///< Timer/Counter F1 Error
				TRIGGER_TCF1_CCA = DMA_CH_TRIGSRC_TCF1_CCA_gc,			///< Timer/Counter F1 Compare or Capture A
				TRIGGER_TCF1_CCB = DMA_CH_TRIGSRC_TCF1_CCB_gc,			///< Timer/Counter F1 Compare or Capture B
				TRIGGER_DMA_SPIF = DMA_CH_TRIGSRC_SPIF_gc,				///< SPI F Transfer Complete
				TRIGGER_DMA_USARTF0_RXC = DMA_CH_TRIGSRC_USARTF0_RXC_gc,///< USART F0 Receive Complete
				TRIGGER_DMA_USARTF0_DRE = DMA_CH_TRIGSRC_USARTF0_DRE_gc,///< USART F0 Data Register Empty
				TRIGGER_DMA_USARTF1_RXC = DMA_CH_TRIGSRC_USARTF1_RXC_gc,///< USART F1 Receive Complete
				TRIGGER_DMA_USARTF1_DRE = DMA_CH_TRIGSRC_USARTF1_DRE_gc,///< USART F1 Data Register Empty
			};
		}
	}
}

#endif	// XPCC_ATXMEGA__DMA_HPP
