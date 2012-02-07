/****************************************************************************
 *   $Id::                                                                  $
 *   Project: NXP LPC11xx CAN example
 *
 *   Description:
 *     This file contains CAN code example which include CAN 
 *     initialization, CAN interrupt handler, and APIs for CAN
 *     access.
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
#include "driver_config.h"
#if CONFIG_ENABLE_DRIVER_CAN==1
#include "can.h"

/* statistics of all the interrupts */
volatile uint32_t BOffCnt = 0;
volatile uint32_t EWarnCnt = 0;
volatile uint32_t EPassCnt = 0;

uint32_t CANRxDone[MSG_OBJ_MAX];

message_object can_buff[MSG_OBJ_MAX];

#if CAN_DEBUG
uint32_t CANStatusLog[100];
uint32_t CANStatusLogCount = 0;
#endif

/*****************************************************************************
** Function name:		MessageProcess
**
** Descriptions:		A valid message received, process message
**
** parameters:			Message Object number
** Returned value:		None
** 
*****************************************************************************/
void CAN_MessageProcess( uint8_t MsgNo )
{
  uint32_t MsgID;
  uint32_t *p_add;

#if BASIC_MODE
  LPC_CAN->IF2_CMDMSK = RD|MASK|ARB|TREQ|DATAA|DATAB;
  LPC_CAN->IF2_CMDREQ = IFCREQ_BUSY;    /* Start message transfer */
#else
  while ( LPC_CAN->IF2_CMDREQ & IFCREQ_BUSY );
  LPC_CAN->IF2_CMDMSK = RD|MASK|ARB|CTRL|INTPND|TREQ|DATAA|DATAB;	
  LPC_CAN->IF2_CMDREQ = MsgNo+1;    /* Start message transfer */
#endif	
  while ( LPC_CAN->IF2_CMDREQ & IFCREQ_BUSY );	/* Check new data bit */

  p_add = (uint32_t *)&can_buff[MsgNo];
  if( LPC_CAN->IF2_ARB2 & ID_MTD )	/* bit 28-0 is 29 bit extended frame */
  {
	/* mask off MsgVal and Dir */ 
	MsgID = (LPC_CAN->IF2_ARB1|((LPC_CAN->IF2_ARB2&0x5FFF)<<16));
  }
  else
  {
	/* bit 28-18 is 11-bit standard frame */
	MsgID = (LPC_CAN->IF2_ARB2 &0x1FFF) >> 2;
  }

  *p_add++ = MsgID;
  *p_add++ = LPC_CAN->IF2_MCTRL & 0x000F;	// Get Msg Obj Data length
  *p_add++ = LPC_CAN->IF2_DA1;
  *p_add++ = LPC_CAN->IF2_DA2;
  *p_add++ = LPC_CAN->IF2_DB1;
  *p_add++ = LPC_CAN->IF2_DB2;
  return;
}

#if !POLLING
#if CONFIG_CAN_DEFAULT_CAN_IRQHANDLER==1
/*****************************************************************************
** Function name:		CAN_IRQHandler
**
** Descriptions:		Processing CAN interrupt
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_IRQHandler(void) 
{
  uint32_t canstat = canstat;
  uint32_t can_int, msg_no;

  while ( (can_int = LPC_CAN->INT) != 0 )
  {
	if ( can_int & CAN_STATUS_INTERRUPT )
	{
	  canstat = LPC_CAN->STAT;
#if CAN_DEBUG
	  CANStatusLog[CANStatusLogCount++] = canstat;
#endif
	  if ( canstat & STAT_EWARN )
	  {
		EWarnCnt++;
		return;
	  }
	  if ( canstat & STAT_BOFF )
	  {
		BOffCnt++;
		return;
	  }
	}
	else
	{
      if ( (canstat & STAT_LEC) == 0 ) 	/* NO ERROR */
	  {
		/* deal with RX only for now. */
		msg_no = can_int & 0x7FFF;
		if ( (msg_no >= 0x01) && (msg_no <= 0x20) )
		{
		  LPC_CAN->STAT &= ~STAT_RXOK;
		  CAN_MessageProcess( msg_no-1 );
		  CANRxDone[msg_no-1] = TRUE;
		}
	  }
	}
  }	
  return;
}
#endif
#endif

/*****************************************************************************
** Function name:		CAN_ConfigureMessages
**
** Descriptions:		Configure all the message buffers(32) that
**           			starting from message object one, the RX and TX 
**                      alternate. It's configured to support both standard
**						and extended frame type. 
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_ConfigureMessages( void )
{
  uint32_t i;
  uint32_t ext_frame = 0;

  /* It's organized in such a way that:
	    obj(x+1)	standard	receive
	    obj(x+2)	standard	transmit 
	    obj(x+3)	extended	receive
	    obj(x+4)	extended	transmit	where x is 0 to 7
	    obj31 is not used. 
		obj32 is for remote date request test only */
  
  for ( i = 0; i < MSG_OBJ_MAX; i++ )
  {
	LPC_CAN->IF1_CMDMSK = WR|MASK|ARB|CTRL|DATAA|DATAB;

	if ( ext_frame == 0 )
	{
	  /* Mxtd: 0, Mdir: 0, Mask is 0x7FF */
	  LPC_CAN->IF1_MSK1 = 0x0000;
	  LPC_CAN->IF1_MSK2 = ID_STD_MASK << 2;

	  /* MsgVal: 1, Mtd: 0, Dir: 0, ID = 0x100 */
	  LPC_CAN->IF1_ARB1 = 0x0000;
	  LPC_CAN->IF1_ARB2 = ID_MVAL | ((RX_MSG_ID+i) << 2);

	  if ( (i % 0x02) == 0 )
	  {
		LPC_CAN->IF1_MCTRL = UMSK|EOB|DLC_MAX;
	  }
	  else
	  {
		LPC_CAN->IF1_MCTRL = UMSK|RXIE|EOB|DLC_MAX;
		ext_frame = 1;
	  }
	}
	else
	{
	  /* Mxtd: 1, Mdir: 0, Mask is 0x1FFFFFFF */
	  LPC_CAN->IF1_MSK1 = ID_EXT_MASK & 0xFFFF;
	  LPC_CAN->IF1_MSK2 = MASK_MXTD | (ID_EXT_MASK >> 16);

	  /* MsgVal: 1, Mtd: 1, Dir: 0, ID = 0x100000 */
	  LPC_CAN->IF1_ARB1 = (RX_EXT_MSG_ID+i) & 0xFFFF;
	  LPC_CAN->IF1_ARB2 = ID_MVAL | ID_MTD | ((RX_EXT_MSG_ID+i) >> 16);

	  if ( (i % 0x02) == 0 )
	  {
		LPC_CAN->IF1_MCTRL = UMSK|EOB|DLC_MAX;
	  }
	  else
	  {
		LPC_CAN->IF1_MCTRL = UMSK|RXIE|EOB|DLC_MAX;
		ext_frame = 0;
	  }
	}
	LPC_CAN->IF1_DA1 = 0x0000;
	LPC_CAN->IF1_DA2 = 0x0000;
	LPC_CAN->IF1_DB1 = 0x0000;
	LPC_CAN->IF1_DB2 = 0x0000;

	/* Transfer data to message RAM */
#if BASIC_MODE
	LPC_CAN->IF1_CMDREQ = IFCREQ_BUSY;
#else
	LPC_CAN->IF1_CMDREQ = i+1;
#endif
	while( LPC_CAN->IF1_CMDREQ & IFCREQ_BUSY );
  }
  return;
}

/*****************************************************************************
** Function name:		CAN_Init
**
** Descriptions:		CAN clock, port initialization
**				
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_Init( uint32_t CANBitClk )
{
  LPC_SYSCON->PRESETCTRL |= (0x1<<3);
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<17);

  /* The USB D- and CAN RX share the dedicated pin. The USB D+ 
  and CAN TX share the dedicated pin. so, no IO configuration is 
  needed for CAN. */
  if ( !(LPC_CAN->CNTL & CTRL_INIT) )
  {
	/* If it's in normal operation already, stop it, reconfigure 
	everything first, then restart. */
	LPC_CAN->CNTL |= CTRL_INIT;		/* Default state */
  }

#if USE_DEFAULT_BIT_TIMING
  /* AHB clock is 48Mhz. The CAN clock divider is within CAN block, 
  set it to 8Mhz for now. Thus, default bit timing doesn't need to 
  be touched. */
  LPC_CAN->CLKDIV = 0x05;			/* Divided by 6 */
   /* Start configuring bit timing */
  LPC_CAN->CNTL |= CTRL_CCE;
  LPC_CAN->BT = 0x2301;
  LPC_CAN->BRPE = 0x0000;
  /* Stop configuring bit timing */
  LPC_CAN->CNTL &= ~CTRL_CCE;
#else
  /* Be very careful with this setting because it's related to
  the input bitclock setting value in CANBitClk. */
  /* popular CAN clock setting assuming AHB clock is 48Mhz:
  CLKDIV = 1, CAN clock is 48Mhz/2 = 24Mhz
  CLKDIV = 2, CAN clock is 48Mhz/3 = 16Mhz
  CLKDIV = 3, CAN clock is 48Mhz/4 = 12Mhz
  CLKDIV = 5, CAN clock is 48Mhz/6 = 8Mhz */

  /* AHB clock is 48Mhz, the CAN clock is 1/6 AHB clock = 8Mhz */
  LPC_CAN->CLKDIV = 0x05;			/* Divided by 6 */
  
  /* Start configuring bit timing */
  LPC_CAN->CNTL |= CTRL_CCE;
  LPC_CAN->BT = CANBitClk;
  LPC_CAN->BRPE = 0x0000;
  /* Stop configuring bit timing */
  LPC_CAN->CNTL &= ~CTRL_CCE;
#endif

  /* Initialization finishes, normal operation now. */
  LPC_CAN->CNTL &= ~CTRL_INIT;
  while ( LPC_CAN->CNTL & CTRL_INIT );

#if (LOOPBACK_MODE | BASIC_MODE)
  LPC_CAN->CNTL |= CTRL_TEST;
  LPC_CAN->TEST &= ~((0x1<<2)|(0x1<<3)|(0x1<<4));
#if LOOPBACK_MODE
  LPC_CAN->TEST |= (0x1<<4);
#endif
#if BASIC_MODE
  LPC_CAN->TEST |= (0x1<<2);
#endif
#endif  

#if !BASIC_MODE
  /* Below is a critical module to configure all the messages */
  /* It's organized in such a way that:
	    obj(x+1)	standard	receive
	    obj(x+2)	standard	transmit 
	    obj(x+3)	extended	receive
	    obj(x+4)	extended	transmit	where x is 0 to 7
	    obj31 is not used. 
		obj32 is for remote date request test only */
  CAN_ConfigureMessages();
#endif

#if !POLLING
  /* Enable the CAN Interrupt */
  NVIC_EnableIRQ(CAN_IRQn);
	
  /* By default, auto TX is enabled, enable all related interrupts */
  LPC_CAN->CNTL |= (CTRL_IE|CTRL_SIE|CTRL_EIE);
#endif
  return;
}

/*****************************************************************************
** Function name:		CAN_Send
**
** Descriptions:		Send a block of data to the CAN port, the 
**						first parameter is the message number, the 2nd 
**						parameter is the received_flag whether it sends received
**						data as the TX data field, the third is the pointer to 
**						the mgs field. When receive_flag is TRUE, the TX msg# is 
**						RX msg#+1, the TX msg ID is twice of the RX msg ID,
**						when received_flag is FALSE, no change in msg#.
**
** parameters:			msg #, received_flag, msg buffer pointer
** Returned value:		None
** 
*****************************************************************************/
void CAN_Send( uint8_t msg_no, uint8_t received_flag, uint32_t *msg_ptr )
{
  uint32_t msg_id, tx_id, Length;   
  
  if ( msg_ptr == NULL )
  {
	while( 1 );
  }

  /* first is the ID, second is length, the next four are data */
  msg_id = *msg_ptr++;
  if ( received_flag )
  {
	/* Mask off MsgVal, Xtd, and Dir, then make the TX msg ID twice the RX msg ID. */
	/* the msg id should be no large than half the max. extended ID allowed. */
	tx_id = (msg_id & 0x1FFFFFFF) << 1;
  }
  else
  {
	tx_id = msg_id;	
  }
  Length = *msg_ptr++;

  if ( Length > DLC_MAX )
  {
	Length = DLC_MAX;	
  }

  if ( !(msg_id & (0x1<<30)) )		/* bit 30 is 0, standard frame */
  {
	/* MsgVal: 1, Mtd: 0, Dir: 1, ID = 0x200 */
	LPC_CAN->IF1_ARB2 = ID_MVAL | ID_DIR | (tx_id << 2);
	LPC_CAN->IF1_ARB1 = 0x0000;

	/* Mxtd: 0, Mdir: 1, Mask is 0x7FF */
	LPC_CAN->IF1_MSK2 = MASK_MDIR | (ID_STD_MASK << 2);
	LPC_CAN->IF1_MSK1 = 0x0000;
  }
  else
  {
	/* MsgVal: 1, Mtd: 1, Dir: 1, ID = 0x200000 */
	LPC_CAN->IF1_ARB2 = ID_MVAL | ID_MTD | ID_DIR | (tx_id >> 16);
	LPC_CAN->IF1_ARB1 = tx_id & 0x0000FFFF;

	/* Mxtd: 1, Mdir: 1, Mask is 0x7FF */
	LPC_CAN->IF1_MSK2 = MASK_MXTD | MASK_MDIR | (ID_EXT_MASK >> 16);
	LPC_CAN->IF1_MSK1 = ID_EXT_MASK & 0x0000FFFF;
  }
  LPC_CAN->IF1_MCTRL = UMSK|TXRQ|EOB|(Length & DLC_MASK);

  LPC_CAN->IF1_DA1 = *msg_ptr++;
  LPC_CAN->IF1_DA2 = *msg_ptr++;
  LPC_CAN->IF1_DB1 = *msg_ptr++;
  LPC_CAN->IF1_DB2 = *msg_ptr;

  LPC_CAN->IF1_CMDMSK = WR|MASK|ARB|CTRL|TREQ|DATAA|DATAB;
#if BASIC_MODE
  LPC_CAN->IF1_CMDREQ = IFCREQ_BUSY;
#else
  /* if receive_ flag is FALSE, the TX uses the normal message object (msg_no+1)
  for transmit. When received_flag is TRUE, msg_no+1 is the message object for RX,
  TX uses the message object next to RX(msg_no+2) for transmit. */ 
  if ( received_flag )
  {
	LPC_CAN->IF1_CMDREQ = msg_no+2;
  }
  else
  {
	LPC_CAN->IF1_CMDREQ = msg_no+1;
  }
  while( LPC_CAN->IF1_CMDREQ & IFCREQ_BUSY );   /* Could spin here forever */
#endif
  return;
}
// #if CONFIG_ENABLE_DRIVER_CAN==1
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/

