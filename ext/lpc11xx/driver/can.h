/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx CAN example
 *
 *   Description:
 *     This file contains CAN code header definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#ifndef __CAN_H__
#define __CAN_H__

#if CONFIG_ENABLE_DRIVER_CAN==1

#include "driver_config.h"
#include "type.h"


/* If TRANSMIT_ONLY is 0, it's a RX_TX bouncing mode, 
use a PC workstation with CAN analyzer to generate
TX message(0x101) and send to LPC11xx, once the target board(LPC11xx) 
receives the matching message ID, it sends the corresponding 
message ID back to the PC workstation(bouncing). */

/* Here are combination of settings:
  1: In LOOPBACK mode, polling only, set POLLING to 1. 
  2: TRANSMIT_ONLY and LOOPBACK_MODE can be set only one
at a time.
*/

#define TRANSMIT_ONLY           0
#define POLLING                 0
#define LOOPBACK_MODE           0		
#define BASIC_MODE              0

#define CAN_DEBUG				0

/* Only below flag is set, it uses the default setting 
in the CAN bittiming configuration value. In this case, 
CAN clock should be 8MHz CAN clock bitrate should be 500kbps. */
#define USE_DEFAULT_BIT_TIMING  1

/* Data structure for a CAN message */
typedef struct
{
    uint32_t	id;
    uint32_t 	dlc;
    uint32_t	data[4];
} message_object;

#define MSG_OBJ_MAX			0x0020
#define DLC_MAX				8

#define RX_MSG_ID			0x100
#define RX_EXT_MSG_ID		0x00100000

#define TX_MSG_ID			0x200
#define TX_EXT_MSG_ID		0x00200000

/* BRP+1 = Fpclk/(CANBitRate * QUANTAValue)
   QUANTAValue = 1 + (Tseg1+1) + (Tseg2+1)
   QUANTA value varies based on the Fpclk and sample point
   e.g. (1) sample point is 87.5%, Fpclk is 48Mhz
   the QUANTA should be 16
        (2) sample point is 90%, Fpclk is 12.5Mhz
   the QUANTA should be 10 
   Fpclk = Fclk /APBDIV
   or
   BitRate = Fcclk/(APBDIV * (BRP+1) * ((Tseg1+1)+(Tseg2+1)+1))
*/ 	
/*
 * Bit Timing Values for 24MHz clk frequency
 */
/*    LPC_CAN->BT = 0x0101;	*/	/* 1Mbps with 8M sysclk */
/*    LPC_CAN->BT = 0x4501;	*/	/* 1Mbps with 24M sysclk */
/*    LPC_CAN->BT = 0x5801;	*/	/* 500kbps with 16M sysclk */
/*    LPC_CAN->BT = 0x5803;	*/	/* 250kbps with 16M sysclk */

#define BITRATE100K8MHZ           0x00000113
#define BITRATE125K8MHZ           0x0000010F
#define BITRATE250K8MHZ           0x00000107
#define BITRATE500K8MHZ           0x00000103
#define BITRATE1000K8MHZ          0x00000101

#define BITRATE100K16MHZ          0x00005809
#define BITRATE125K16MHZ          0x00005807
#define BITRATE250K16MHZ          0x00005803
#define BITRATE500K16MHZ          0x00005801

#define BITRATE100K24MHZ          0x00007E09
#define BITRATE125K24MHZ          0x0000450F
#define BITRATE250K24MHZ          0x00004507
#define BITRATE500K24MHZ          0x00004503
#define BITRATE1000K24MHZ         0x00004501


#define CAN_STATUS_INTERRUPT      0x8000    /* 0x0001-0x0020 are the # of the message 
										    object */
                                            /* 0x8000 is the status interrupt */

/* CAN Message interface register definitions */
/* bit field of IF command request n register */
#define IFCREQ_BUSY               0x8000    /* 1 is writing is progress, cleared when 
                                            RD/WR done */
/* CAN CTRL register */
#define CTRL_INIT		(1 << 0)
#define CTRL_IE			(1 << 1) 
#define CTRL_SIE		(1 << 2)
#define CTRL_EIE		(1 << 3) 
#define CTRL_DAR		(1 << 5)
#define CTRL_CCE		(1 << 6) 
#define CTRL_TEST		(1 << 7)
	
/* CAN Status register */
#define STAT_LEC		(0x7 << 0)
#define STAT_TXOK		(1 << 3) 
#define STAT_RXOK		(1 << 4)
#define STAT_EPASS		(1 << 5) 
#define STAT_EWARN		(1 << 6)
#define STAT_BOFF		(1 << 7)

/* bit field of IF command mask register */
#define	DATAB	(1 << 0)   /* 1 is transfer data byte 4-7 to message object, 0 is not */ 
#define	DATAA	(1 << 1)   /* 1 is transfer data byte 0-3 to message object, 0 is not */ 
#define	TREQ	(1 << 2)   /* 1 is set the TxRqst bit, 0 is not */
#define	INTPND	(1 << 3)       
#define	CTRL	(1 << 4)   /* 1 is transfer the CTRL bit to the message object, 0 is not */
#define	ARB		(1 << 5)   /* 1 is transfer the ARB bits to the message object, 0 is not */
#define	MASK	(1 << 6)   /* 1 is transfer the MASK bit to the message object, 0 is not */
#define	WR		(1 << 7)   /* 0 is READ, 1 is WRITE */
#define RD      0x0000

/* bit field of IF mask 2 register */
#define	MASK_MXTD	(1 << 15)     /* 1 extended identifier bit is used in the RX filter unit, 0 is not */ 
#define	MASK_MDIR	(1 << 14)     /* 1 direction bit is used in the RX filter unit, 0 is not */

/* bit field of IF identifier 2 register */
#define	ID_MVAL		(1 << 15)     /* Message valid bit, 1 is valid in the MO handler, 0 is ignored */ 
#define	ID_MTD		(1 << 14)     /* 1 extended identifier bit is used in the RX filter unit, 0 is not */ 
#define	ID_DIR		(1 << 13)     /* 1 direction bit is used in the RX filter unit, 0 is not */

/* bit field of IF message control register */
#define	NEWD		(1 << 15)     /* 1 indicates new data is in the message buffer.  */ 
#define	MLST		(1 << 14)     /* 1 indicates a message loss. */ 
#define	INTP		(1 << 13)     /* 1 indicates message object is an interrupt source */
#define UMSK    	(1 << 12)     /* 1 is to use the mask for the receive filter mask. */
#define	TXIE		(1 << 11)     /* 1 is TX interrupt enabled */ 
#define	RXIE		(1 << 10)     /* 1 is RX interrupt enabled */
#define	ROEN		(1 << 9)      /* 1 is remote frame enabled */
#define TXRQ    	(1 << 8)      /* 1 is TxRqst enabled */
#define	EOB			(1 << 7)      /* End of buffer, always write to 1 */ 
#define	DLC			0x000F        /* bit mask for DLC */ 

#define ID_STD_MASK		0x07FF		
#define ID_EXT_MASK		0x1FFFFFFF
#define DLC_MASK		0x0F

extern void CAN_IRQHandler (void);
extern void CAN_Init( uint32_t CANBitClk );
extern void CAN_ConfigureMessages( void );
extern void CAN_MessageProcess( uint8_t MsgObjNo );
extern void CAN_Send( uint8_t msg_no, uint8_t received_flag, uint32_t *data );

#endif
#endif  /* __CAN_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/

