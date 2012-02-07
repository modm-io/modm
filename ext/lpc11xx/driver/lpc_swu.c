/***********************************************************************
 * $Id::                                                               $
 *
 * Project: LPC11XX/LPC13XX Software UART
 *
 * Description:
 *   This software acts as a standard UART with 1 start, 8 data and 1
 *   stop bit (no parity) setup. TIMER32_0 MAT0.3 on P0.11 (pin 32
 *   on LQFP48 package) generates UART output while TIMER32_0
 *   CAP0.0 on P1.5 (pin 45 on LQFP48 package) receives serial data.
 *
 *   While P3.0 is low, a routine that calculates future transmission
 *   parameters is active. P3.1 is low while TIMER32_0 interrupt service
 *   routine (ISR) is running. P3.2 is low while software UART
 *   transmission portion of the TIMER32_0 ISR is executed. P3.3 indicates
 *   when the software UART receiving related TIMER32_0 ISR is active.
 *   P1.11 activity matches write access to the tx FIFO.
 *
 *   For this demo to run successfully pin P0.11(Soft TX) must be connected to
 *   P1.7(True TX) and pin P1.5(Soft RX) must be connected to P1.6(True RX).
 *   A PC or similar equipment capable of handling ASCII data at 9600 bit/s must be
 *   connected to the UART0 of the microcontroller. P1.7 and P1.6 are configured
 *   as GPIO pins and inactive mode (no pull-down/pull-up resistor enabled)
 *   in order not to influence the communication of soft uart.
 *
 *   UART0 of the LPC111x is not used in this demo. Regular U0Tx and U0Rx
 *   functionality is replaced by the software uart pins. UART0 pins are
 *   selected because most of the development boards already have needed
 *   hardware for voltage translation between the microcontroller and an
 *   external device.
 *   fosc = 12 MHz; PLL is used; System clock = AHB clock =48Mhz
 *
 ***********************************************************************
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
 **********************************************************************/

/*
 * Pin list:
 * P1.7:           [GPIO]  - needed because of a standard interface hw
 * P1.6:           [GPIO]  - needed because of a standard interface hw
 * P3.0:   TX_PRO  [GPO]   - next tx parameter processing (active low)
 * P3.1:   INT     [GPO]   - TIMER0 interrupt indicator (active low)
 * P3.2:   INT_TX  [GPO]   - swu tx interrupt indicator (active low)
 * P3.3:   INT_RX  [GPO]   - swu rx interrupt indicator (active low)
 * P1.11:  CALL    [GPO]   - tx UART routine call ind. (active low)
 * P0.11:  SWU_Tx  [MAT0.3]- software UART Tx pin
 * P1.5:   SWU_RX  [CAP0.0]- software UART Rx pin
 *
 *********************************************************/
#include "driver_config.h"
#if CONFIG_ENABLE_DRIVER_SWUART==1
#include "target_config.h"

#include "gpio.h"
#include "lpc_swu.h"

/*********************************************************
 ** Macro Definitions                                 	**
 *********************************************************/
#define RX_OVERFLOW 4
#define RX_ACTIVE   2
#define TX_ACTIVE   1
#define ADJUST      (1<<30)
#define ALL1        0x000000FF

/*********************************************************
 ** Global Variables                                     **
 *********************************************************/
static volatile unsigned char cnt_edges; //no of char edges
static volatile unsigned char edge_index; //edge index for output
static volatile unsigned char swu_tx_st; //sw UART status
static volatile unsigned long int edge[11]; //array of edges
static volatile unsigned char last_edge_index, char_end_index; //last two events index
//software UART Tx related definitions
static volatile unsigned long int tx_fifo_wr_ind, tx_fifo_rd_ind;
static volatile signed long int swu_tx_cnt, swu_tx_trigger;
static volatile unsigned short int swu_tx_fifo[TXBUFF_LEN];
//software UART Rx related definitions
static volatile unsigned long int rx_fifo_wr_ind, rx_fifo_rd_ind;
static volatile signed long int swu_rx_cnt, swu_rx_trigger;
static volatile unsigned char swu_bit, cnt, cnt_bits, swu_rx_chr_fe;
static volatile unsigned long int swu_rbr, swu_rbr_mask;
static volatile signed long int edge_last, edge_sample, edge_current, edge_stop;
static volatile unsigned short int swu_rx_fifo[RXBUFF_LEN];
//definitions common for transmitting and receiving data
static volatile unsigned long int swu_status;

/*********************************************************
 ** Local Functions                                      **
 *********************************************************/
static void swu_setup_tx(void); //tx param processing
__attribute__ ((weak)) void swu_rx_callback(void);

/******************************************************************************
 ** Function name:   swu_setup_tx
 **
 ** Descriptions:
 **
 ** This routine prepares an array of toggle points that will be used to generate
 ** a waveform for the currently selected character in the software UART transmission
 ** FIFO; at the end this routine starts the transmission itself.
 **
 ** parameters:	    None
 ** Returned value:  None
 **
 ******************************************************************************/
static void swu_setup_tx(void) {
	unsigned char bit, i;
	unsigned long int ext_data, delta_edges, mask, reference;

	GPIOSetValue(PORT_TX_PRO, PIN_TX_PRO, 0); //indicate routine begin
	if (tx_fifo_wr_ind != tx_fifo_rd_ind) //data to send, proceed
	{
		swu_status |= TX_ACTIVE; //sw uart tx is active
		tx_fifo_rd_ind++; //update the tx fifo ...
		if (tx_fifo_rd_ind == TXBUFF_LEN) //read index...
			tx_fifo_rd_ind = 0; //...
		ext_data = (unsigned long int) swu_tx_fifo[tx_fifo_rd_ind]; //read the data
		ext_data = 0xFFFFFE00 | (ext_data << 1); //prepare the pattern
		edge[0] = BIT_LENGTH; //at least 1 falling edge...
		cnt_edges = 1; //... because of the START bit
		bit = 1; //set the bit counter
		reference = 0x00000000; //init ref is 0 (start bit)
		mask = 1 << 1; //prepare the mask
		delta_edges = BIT_LENGTH; //next edge at least 1 bit away
		while (bit != 10) { //until all bits are examined
			if ((ext_data & mask) == (reference & mask)) { //bit equal to the reference?
				delta_edges += BIT_LENGTH; //bits identical=>update length
			} //...
			else { //bits are not the same:
				edge[cnt_edges] = //store new...
						edge[cnt_edges - 1] + delta_edges; //... edge data
				reference = ~reference; //update the reference
				delta_edges = BIT_LENGTH; //reset delta_ to 1 bit only
				cnt_edges++; //update the edges counter
			}
			mask = mask << 1; //update the mask
			bit++; //move on to the next bit
		}
		edge[cnt_edges] = //add the stop bit end...
				edge[cnt_edges - 1] + delta_edges; //... to the list
		cnt_edges++; //update the number of edges
		last_edge_index = cnt_edges - 2; //calculate the last edge index
		char_end_index = cnt_edges - 1; //calc. the character end index

		edge_index = 0; //reset the edge index
		reference = LPC_TMR32B0->TC + BIT_LENGTH; //get the reference from TIMER0
		for (i = 0; i != cnt_edges; i++) //recalculate toggle points...
			edge[i] = (edge[i] + reference) //... an adjust for the...
					& 0x3FFFFFFF; //... timer range
		LPC_TMR32B0->MR3 = edge[0]; //load MR3
		LPC_TMR32B0->MCR = LPC_TMR32B0->MCR | (1 << 9); //enable interrupt on MR3 match
		LPC_TMR32B0->EMR = LPC_TMR32B0->EMR | (3 << 10); //enable toggle on MR3 match
	}
	GPIOSetValue(PORT_TX_PRO, PIN_TX_PRO, 1); //indicate routine exit
	return; //return from the routine
}

/******************************************************************************
 ** Function name:		swu_tx_str
 **
 ** Descriptions:
 **
 ** This routine transfers a string of characters one by one into the
 ** software UART tx FIFO.
 **
 ** parameters:		string pointer
 ** Returned value:		None
 **
 ******************************************************************************/
void swu_tx_str(unsigned char const* ptr_out) {
	while (*ptr_out != 0x00) //read all chars...
	{
		swu_tx_chr(*ptr_out); //...put the char in tx FIFO...
		ptr_out++; //...move to the next char...
	} //...
	return; //return from the routine
}

/******************************************************************************
 ** Function name:		swu_tx_chr
 **
 ** Descriptions:
 **
 ** This routine puts a single character into the software UART tx FIFO.
 **
 ** parameters:		char
 ** Returned value:		None
 **
 ******************************************************************************/
void swu_tx_chr(const unsigned char out_char) {
	GPIOSetValue(PORT_CALL, PIN_CALL, 0); //IOCLR0  = pin_call;
	//write access to tx FIFO begin

	while (swu_tx_cnt == TXBUFF_LEN)
		; //wait if the tx FIFO is full
	tx_fifo_wr_ind++; //update the write pointer...
	if (tx_fifo_wr_ind == TXBUFF_LEN) //...
		tx_fifo_wr_ind = 0; //...
	swu_tx_fifo[tx_fifo_wr_ind] = out_char; //put the char into the FIFO
	swu_tx_cnt++; //update no.of chrs in the FIFO
	if ((swu_status & TX_ACTIVE) == 0)
		swu_setup_tx(); //start tx if tx is not active

	GPIOSetValue(PORT_CALL, PIN_CALL, 1); //IOSET0  = pin_call;
	//write access to tx FIFO end
	return; //return from the routine
}

/******************************************************************************
 ** Function name:		swu_rx_chr
 **
 ** Descriptions:
 **
 ** This routine reads a single character from the software UART rx FIFO.
 ** If no new data is available, it returns the last one read;
 ** The framing error indicator is updated, too.
 **
 ** parameters:		None
 ** Returned value:		char
 **
 ******************************************************************************/
unsigned char swu_rx_chr(void) {
	if (swu_rx_cnt != 0) { //update the rx indicator...
		rx_fifo_rd_ind++; //... if data are present...
		if (rx_fifo_rd_ind == RXBUFF_LEN)
			rx_fifo_rd_ind = 0; //...
		swu_rx_cnt--; //...
	}
	if ((swu_rx_fifo[rx_fifo_rd_ind] & 0x0100) == 0) //update...
		swu_rx_chr_fe = 0; //... the framing error...
	else
		//... indicator...
		swu_rx_chr_fe = 1; //...
	swu_status &= ~RX_OVERFLOW; //clear the overfloe flag
	return ((unsigned char) (swu_rx_fifo[rx_fifo_rd_ind] & 0x00FF)); //return data
}

/******************************************************************************
 ** Function name: swu_isr_tx
 **
 ** Descriptions:
 **
 ** Handle timer output toggle events. If there are additional edge events
 ** in the edge[] array, continue to load the match register with them. If
 ** the last edge has been toggled, check for additional characters to load
 ** from FIFO, otherwise terminate transmission of characters.
 **
 ** parameters:  TX_ISR_TIMER pointer to timer resource to enable
 **              increased portability.
 ** Returned value:  None
 **
 ******************************************************************************/
void swu_isr_tx(LPC_TMR_TypeDef* const TX_ISR_TIMER) {
	if (0 != (TX_ISR_TIMER->IR & 0x08)) { //tx routine interrupt begin
		GPIOSetValue(PORT_INT_TX, PIN_INT_TX, 0); //tx interrupt activity begin

		TX_ISR_TIMER->IR = 0x08; //clear the MAT3 flag
		if (edge_index == char_end_index) //the end of the char:
		{
			TX_ISR_TIMER->MCR &= ~(7 << 9); //MR3 impacts T0 ints no more
			swu_tx_cnt--; //update no.of chars in tx FIFO
			if (tx_fifo_wr_ind != tx_fifo_rd_ind) //if more data pending...
				swu_setup_tx(); //... spin another transmission
			else
				swu_status &= ~TX_ACTIVE; //no data left=>turn off the tx
		} else {
			if (edge_index == last_edge_index) //is this the last toggle?
				TX_ISR_TIMER->EMR = 0x000003FF; //no more toggle on MAT3
			edge_index++; //update the edge index
			TX_ISR_TIMER->MR3 = edge[edge_index]; //prepare the next toggle event
		}

		GPIOSetValue(PORT_INT_TX, PIN_INT_TX, 1); //tx interrupt activity end
	}
}

/******************************************************************************
 ** Function name: swu_isr_rx
 **
 ** Descriptions:
 **
 ** Look for start bit falling edge. If found begin comparing edge events to
 ** decode the received character, followed by stop bit detection routine.
 **
 ** parameters:  TX_ISR_TIMER pointer to timer resource to enable
 **              increased portability.
 ** Returned value:  None
 **
 ******************************************************************************/
void swu_isr_rx(LPC_TMR_TypeDef* const RX_ISR_TIMER) {
	signed long int edge_temp;
	//sw uart receive isr code begin
	if (0 != (RX_ISR_TIMER->IR & 0x10)) //capture interrupt occured:
	{
		GPIOSetDir(PORT_INT_RX, PIN_INT_RX, 0); //rx interrupt activity begin

		RX_ISR_TIMER->IR = 0x10; //edge dtcted=>clear CAP0 flag
		//change the targeted edge
		RX_ISR_TIMER->CCR = 0x0004 | (0x0003 - (RX_ISR_TIMER->CCR & 0x0003));
		if ((swu_status & RX_ACTIVE) == 0) { //sw UART not active (start):
			edge_last = (signed long int) RX_ISR_TIMER->CR0; //initialize the last edge
			edge_sample = edge_last + (BIT_LENGTH >> 1); //initialize the sample edge
			if (edge_sample < edge_last) //adjust the sample edge...
				edge_sample |= ADJUST; //... if needed
			swu_bit = 0; //rx bit is 0 (a start bit)
			RX_ISR_TIMER->IR = 0x02; //clear MAT1 int flag
			edge_stop = edge_sample + STOP_BIT_SAMPLE; //estimate the end of the byte
			if (edge_stop < edge_last) //adjust the end of byte...
				edge_stop |= ADJUST; //... if needed
			RX_ISR_TIMER->MR1 = edge_stop; //set MR1 (stop bit center)
			RX_ISR_TIMER->MCR = RX_ISR_TIMER->MCR | (1 << 3); //int on MR1
			cnt = 9; //initialize the bit counter
			swu_status |= RX_ACTIVE; //update the swu status
			swu_rbr = 0x0000; //reset the sw rbr
			swu_rbr_mask = 0x0001; //initialize the mask
		} else {
			//reception in progress:
			edge_current = (signed long int) RX_ISR_TIMER->CR0; //initialize the current edge
			if (edge_current < edge_last) //adjust the current edge...
				edge_current |= ADJUST; //... if needed
			while (edge_current > edge_sample) { //while sampling edge is within
				if (cnt_bits != 0) {
					if (swu_bit != 0) //update data...
						swu_rbr |= swu_rbr_mask; //...
					swu_rbr_mask = swu_rbr_mask << 1; //update mask
				}
				cnt_bits++; //update the bit count
				edge_temp = edge_last + BIT_LENGTH; //estimate the last edge
				if (edge_temp < edge_last) //adjust...
					edge_last = edge_temp | ADJUST; //... the last edge...
				else
					//... if...
					edge_last = edge_temp; //... needed
				edge_temp = edge_sample + BIT_LENGTH; //estimate the sample edge
				if (edge_temp < edge_sample) //adjust...
					edge_sample = edge_temp | ADJUST; //... the sample edge...
				else
					//... if...
					edge_sample = edge_temp; //... needed
				cnt--; //update the no of rcved bits
			}
			swu_bit = 1 - swu_bit; //change the received bit
		}

		GPIOSetDir(PORT_INT_RX, PIN_INT_RX, 1); //rx interrupt activity end
	}

	if (0 != (RX_ISR_TIMER->IR & 0x02)) //stop bit timing matched:
	{
		GPIOSetDir(PORT_INT_RX, PIN_INT_RX, 0); //rx interrupt activity begin

		RX_ISR_TIMER->IR = 0x02; //clear MR1 flag
		if (cnt != 0) { //not all data bits received...
			swu_rbr = swu_rbr << cnt; //... make space for the rest...
			if (swu_bit != 0)
				swu_rbr += ALL1 << (8 - cnt); //... add needed 1(s)...
		} //...
		swu_rbr &= 0x00FF; //extract data bits only
		if (swu_bit == 0) //if the stop bit was 0 =>
			swu_rbr |= 0x00000100; //... framing error!
		swu_status &= ~RX_ACTIVE; //sw UART not active any more
		cnt_bits = 0; //reset the rx bit count
		if (swu_rx_cnt != RXBUFF_LEN) //store the rcved character...
		{
			swu_rx_cnt++; //... into the sw UART...
			rx_fifo_wr_ind++; //... rx FIFO
			if (rx_fifo_wr_ind == RXBUFF_LEN)
				rx_fifo_wr_ind = 0; //...
			swu_rx_fifo[rx_fifo_wr_ind] = swu_rbr; //...
			if (swu_rx_cnt >= swu_rx_trigger)
				swu_rx_callback(); //rx 'isr' trig excded
		} else {
			swu_status |= RX_OVERFLOW; //rx FIFO full => overflow
		}
		RX_ISR_TIMER->MCR &= ~(7 << 3); //MR0 impacts TIMER0 no more

		GPIOSetDir(PORT_INT_RX, PIN_INT_RX, 1); //IOSET0 = pin_intrx;     //rx interrupt activity end
	}
	//sw uart receive isr code end
}

/******************************************************************************
 ** Function name: 	swu_init
 **
 ** Description:		Initialize variables used by software UART.
 ** 					Configure specific IO pins.
 ** 					Setup and start timer running.
 **
 ** parameters:  	UART_TIMER  - pointer to timer resource to enable configuration.
 ** Returned value:  None
 **
 ******************************************************************************/
void swu_init(LPC_TMR_TypeDef* const UART_TIMER) {
	//setup the software uart
	swu_tx_cnt = 0; //no data in the swu tx FIFO
	tx_fifo_wr_ind = 0; //last char written was on 0
	tx_fifo_rd_ind = 0; //last char updated was on 0
	swu_rx_trigger = 1; //>=1 char gnrts a rx interrupt
	swu_status = 0; //neither tx nor rx active

	/* Some of the I/O pins need to be clearfully planned if
	 you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 9);

	LPC_IOCON->PIO1_5 &= ~0x07; /*  Timer0_32 I/O config */
	LPC_IOCON->PIO1_5 |= 0x02; /* Timer0_32 CAP0 */
	LPC_IOCON->R_PIO0_11 &= ~0x07;
	LPC_IOCON->R_PIO0_11 |= 0x03; /* Timer0_32 MAT3 */

	//Timer0_32 setup
	UART_TIMER->TCR = 0x00; //stop TIMER0_32
	UART_TIMER->TCR = 0x02; //reset counter
	UART_TIMER->TCR = 0x00; //release the reset

	UART_TIMER->IR = 0x01F; //clear all TIMER0 flags
	UART_TIMER->PR = 0x00000000; //no prescaler
	UART_TIMER->MR0 = 0x3FFFFFFF; //TIMER0_32 counts 0 - 0x3FFFFFFF
	UART_TIMER->MCR = 2; //reset TIMER0_32 on MR0
	UART_TIMER->EMR = 0x0008; //drive MAT0.3 high
	UART_TIMER->TCR = 0x01; //let TIMER0_32 run
	NVIC_EnableIRQ(TIMER_32_0_IRQn); //Enable the TIMER0_32 Interrupt

	while (0 == (LPC_GPIO1->DATA & (1 << PIN_SW_RX)))
		;//wait for 1 on sw UART rx line
	UART_TIMER->IR = 0x10; //clear CAP0.0 flag
	UART_TIMER->CCR = 0x0006; //int on CAP0.0 falling edge

	cnt_bits = 0; //reset the rx bit count
}

/******************************************************************************
 ** Function name: 	swu_rx_callback
 **
 ** Descriptions:	Default call-back function for handling RX character events.
 ** 					Runs in interrupt's context. If user doesn't overload, do
 ** 					nothing.
 **
 ** Parameters:  	None
 ** Returned value:  None
 **
 ******************************************************************************/
__attribute__ ((weak)) void swu_rx_callback(void) {
  ;//do nothing if user doesn't overload
}
#endif
