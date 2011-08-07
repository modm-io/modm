
#ifndef UART_BASE_H
#define UART_BASE_H

#include <stdint.h>

/**
 * @brief	Abstract base class for UART drivers
 * 
 * @date	May 2011
 * @author	Fabian Greif
 */
class UartInterface
{
public:
	/**
	 * @brief	Initialize the hardware
	 * 
	 * Sets the registers etc.
	 * 
	 * With the trigger level it is possible to make a tradeoff between
	 * latency and interrupt count. A small value will give you more interrupts
	 * but a faster response time, a bigger value will decrease the
	 * number of interrupts.
	 * Nonetheless after 3.5 - 4.5 character times the remaining bytes will
	 * be collected from the FIFO. 
	 * 
	 * Possible values for \c level:
	 * - 0 => 1 byte
	 * - 1 => 4 byte
	 * - 2 => 8 byte
	 * - 3 => 14 byte
	 * 
	 * @brief	level	Trigger level for the UART Fifos.
	 */
	virtual void
	initialize(uint8_t level = 2) = 0;
	
	/**
	 * @brief	Send a character
	 * 
	 * @param	c			character to be send
	 * @param	blocking	If \c true the function will wait until the
	 * 						character could be send. Otherwise the function
	 * 						will return immediately even if sending failed.
	 * @return	\c true if the character was send, \c false otherwise.
	 */
	virtual bool
	write(char c, bool blocking = true) = 0;
	
	/**
	 * @brief	Check if a character was received and is ready to be read
	 */
	virtual bool
	isCharacterAvailable() = 0;
	
	/**
	 * @brief	Read a single character
	 * 
	 * @return	\c true if a character was available and was stored in \c c,
	 * 			false if nothing was available. \c c will remain untouched
	 * 			in this case.
	 */
	virtual bool
	read(char &c, bool blocking = false) = 0;
};

#endif // UART_BASE_H
