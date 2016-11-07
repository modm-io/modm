/*
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LPC11__TIMER_REGISTERS_HPP
#define XPCC_LPC11__TIMER_REGISTERS_HPP

namespace xpcc
{
	///< 
	namespace lpc
	{
		enum TMR_TCR
		{
			TCR_CEn 	= (1 << 0),		///< Counter Enable.
			TCR_CRst 	= (1 << 1),		///< Counter Reset.
		};

		///< Match Control Register
		enum TMR_MCR
		{
			MCR_MR0I 	= (1 <<  0),	///< Interrupt on MR0 match
			MCR_MR0R 	= (1 <<  1),	///< Reset on MR0R
			MCR_MR0S 	= (1 <<  2),	///< Stop on MR0S
			MCR_MR1I 	= (1 <<  3),	///< Interrupt on MR1 match
			MCR_MR1R 	= (1 <<  4),	///< Reset on MR1
			MCR_MR1S 	= (1 <<  5),	///< Stop on MR1
			MCR_MR2I 	= (1 <<  6),	///< Interrupt on MR2 match
			MCR_MR2R 	= (1 <<  7),	///< Reset on MR2
			MCR_MR2S 	= (1 <<  8),	///< Stop on MR2
			MCR_MR3I 	= (1 <<  9),	///< Interrupt on MR3 match
			MCR_MR3R 	= (1 << 10),	///< Reset on MR3
			MCR_MR3S 	= (1 << 11),	///< Stop on MR3
		};

		///< Capture Control Register
		enum TMR_CCR
		{
			CCR_CAP0RE 	= (1 << 0),		///< Capture on Rising Edge
			CCR_CAP0FE 	= (1 << 1),		///< Capture on Falling Edge
			CCR_CAP0I 	= (1 << 2),		///< Enable Interrupt on Capture
		};

		///< External Match Register
		enum TMR_EMR
		{
			EMR_EM0 	= (1 <<  0),		///< External Match 0
			EMR_EM1 	= (1 <<  1),		///< External Match 1
			EMR_EM2 	= (1 <<  2),		///< External Match 2
			EMR_EM3 	= (1 <<  3),		///< External Match 3
			EMR_EMC0 	= (3 <<  4),		///< External Match Control 0
			EMR_EMC0_0 	= (1 <<  4),		///< External Match Control 0
			EMR_EMC0_1 	= (1 <<  5),		///< External Match Control 0
			EMR_EMC1 	= (3 <<  6),		///< External Match Control 1
			EMR_EMC1_0 	= (1 <<  6),		///< External Match Control 1
			EMR_EMC1_1 	= (1 <<  7),		///< External Match Control 1
			EMR_EMC2 	= (3 <<  8),		///< External Match Control 2
			EMR_EMC2_0 	= (1 <<  8),		///< External Match Control 2
			EMR_EMC2_1 	= (1 <<  9),		///< External Match Control 2
			EMR_EMC3 	= (3 << 10),		///< External Match Control 3
			EMR_EMC3_0 	= (1 << 10),		///< External Match Control 3
			EMR_EMC3_1 	= (1 << 11),		///< External Match Control 3
		};

		///< Count Control Register
		enum TMR_CTCR
		{
			CTCR_CTM 	= (3 << 0),			///< Counter/Timer Mode
			CTCR_CTM_0 	= (1 << 0),			///< Counter/Timer Mode
			CTCR_CTM_1 	= (1 << 1),			///< Counter/Timer Mode
			CTCR_CIS 	= (3 << 2),			///< Counter Input Select
			CTCR_CIS_0 	= (1 << 2),			///< Counter Input Select
			CTCR_CIS_1 	= (1 << 3),			///< Counter Input Select
		};

		///< PWM Controk Register
		enum TMR_PWMC
		{
			PWMC_PWMEN0 	= (1 << 0),			///< PWM Channel0 Enable
			PWMC_PWMEN1 	= (1 << 1),			///< PWM Channel1 Enable
			PWMC_PWMEN2 	= (1 << 2),			///< PWM Channel2 Enable
			PWMC_PWMEN3 	= (1 << 3),			///< PWM Channel3 Enable
		};


	}
}

#endif // XPCC_LPC11__TIMER_REGISTERS_HPP
