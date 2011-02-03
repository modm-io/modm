
#ifndef NDS3_HPP
#define NDS3_HPP

#include "nibo2.hpp"

/**
 * \brief	IR distance scanner
 * 
 * The NDS3 features a IR distance sensor mounted to a servo, being able to
 * rotate by 180 degree. It can do a single measurement at a fixed position 
 * or a complete scan of the environment.
 * 
 * The interface to the Nibo2 is done via a I2C (or TWI in Atmels terms).
 * 
 * \author	Fabian Greif
 */
class Nds3
{
public:
	/**
	 * \brief	Check if current operation is finished
	 * 
	 * \return	\c false if an operation is in progress, \c true if the TWI
	 * 			interface is ready to receive a new command.
	 */
	static bool
	isInterfaceReady();
	
public:
	/**
	 * \brief	Aborts the current command
	 */
	static void
	reset();
	
	static void
	readBusyStatus();
	
	static bool
	isBusy();
	
	/**
	 * \brief	Move servo to a new position
	 * \param	position	Target position for the servo
	 */
	static void
	move(uint8_t position);
	
	/**
	 * \brief	Read servo position from the NDS3
	 * 
	 * Use getPosition() to get the result after isBusy() returns \c false.
	 */
	static void
	readPosition();
	
	/**
	 * \brief	Get servo position
	 * \return	Current position of the Servo
	 */
	static uint8_t
	getPosition();
	
	/**
	 * \brief	Read the current distance from NDS3
	 * 
	 * Use getDistance() to get the result after isBusy() returns \c false.
	 */
	static void
	readDistance();
	
	/**
	 * \brief	Get the current distance
	 * \return	Distance in mm
	 */
	static uint8_t
	getDistance();
	
	/**
	 * \brief	Perform a distance scan
	 * 
	 * Scans from the current position to \c endPosition with an increment
	 * of \c stepWidth. For a starting position of 60 and a end position
	 * of 120 with a step width of 3   
	 * 
	 * \param	endPosition	
	 * \param	stepWidth	step width in degree
	 */
	static void
	scan(uint8_t endPosition, int8_t stepWidth);
	
	/**
	 * \brief	Read scan results from NDS3
	 * 
	 * Reads up to 16 values at once.
	 * Use getScanResult() to get the results after isBusy() returns \c false.
	 * 
	 * \param	position	Starting position
	 * \param	step		Step width (incrementing value)
	 */
	static void
	readScanResult(uint8_t position, uint8_t step);
	
	// TODO
	static void
	getScanResult();
	
private:
	static const uint8_t nds3Address = (42 << 1);
	static uint8_t buffer[16];
};

#endif // NDS3_HPP
