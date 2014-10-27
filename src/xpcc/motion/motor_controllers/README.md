# Motor Controllers

These static template classes provide controlled motors. 

## With one motor
These static template classes control a single motor with an encoder. It can be controlled by speed or by position. 

The common interface for configuration is:

	+ void configure(ControlMode, currentLimit)
	+ void setVelocityControllerParameter(xpcc::Pid::Parameter)
	+ void setPositionControllerParameter(xpcc::SCurveController::Parameter)
	
	+ void setPresentPosition(int32_t)
	+ void setTargetArea(uint32_t)
	
	+ void setCurrentLimit(uint16_t)

The common interface for getting information about the ongoing operation of the motor is:

	+ bool    isInitialised()
	+ bool    isCurrentOverLimit()
	+ bool    isMoving()
	+ bool    isTargetPositionReached()
	+ int32_t getPosition()
	+ int32_t getSpeed()
	+ int32_t getCurrentPwm()

To change the control mode between speed and position use:

	+ void setControlMode(ControlMode)

In position controlled mode the motor is accessed by:

	+ void    setTargetPosition(int32)
	+ int32_t getTargetPosition()

In velocity controlled mode use:

	+ void    setTargetSpeed(int32)
	+ int32_t getTargetSpeed()

Common for motors:
	
	+ void disable()
	+ void run()

### SingleMotorController

### iMotor

## With two motors
### DifferentialDriveController
Build a controller for differential 