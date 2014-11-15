#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
namespace Configuration
{

/* Odometry Settings
 *
 * TODO: translate to English!
 * Wie man die Faktoren berechnet:
 *
 * Wichtig ist das Verhaeltnis von Raddurchmesser zum Abstand der beiden
 * Raeder (hier R/L). Dieses sorgt fuer die korrekte Form der gefahrenen
 * Kurven. Erst zusammen mit dem Abstand der Raeder (hier D) ergibt sich
 * die korrekte Skalierung.
 * Fuer die Berechnung der Drehung ist die Skalierung noch nicht relevant,
 * ausserdem laesst sich die Drehung einfach vermessen. Daher wird zuerst
 * R und L bestimmt.
 *
 * Dazu stellt man den Roboter an eine Spielfeldbande (der hintere Teil
 * muss entsprechend gerade sein) und laesst ihn dann mehrfach Kreise fahren
 * und richtet ihn anschliessend wieder an der Bande aus.
 * Dabei werden die linken und rechten Encoderwerte aufintegiert und die
 * Ergebnisse in ein Gleichungssystem eingetragen:
 *
 * r1 * R + l1 * L = phi1
 * r2 * R + l2 * L = phi2
 *
 *
 * mit l1..n = Summe Encoderwerte links
 *     r1..n = Summe Encoderwerte rechts
 *     phi1..n = m * (2*pi),  m >= 5
 *
 * Dieses Gleichungssystem loest man nun nach R und L. Natuerlich erhaehlt man
 * kein exaktes Ergebnis, daher ist es gut viele Messungen zu machen. Diese
 * Messungen sollten moeglichst unterschiedlich mit moeglichst hohen Werten
 * fuer rn und ln.
 *
 *     | r1  l1 |
 * A = | r2  l2 |
 *     | ..  .. |
 *
 *     | phi1 |
 * d = | phi2 |
 *     | .... |
 *
 *       T     T
 * loese A A | A d
 *
 * Moeglichst unterschiedliche Messungen bedeutet Kreise mit
 * unterschiedlichem Radius zu fahren. Dabei sollte man vermeiden ein Rad
 * stehen zu lassen (der entsprechende Encoderwert ist sonst zu klein).
 * Z.B. koennte man drehen um den Nullpunkt, fahren um Radius 30cm, 50cm, ...
 *
 *
 * Hat man die Verhaeltnisse ausreichend genau bestimmt reicht es den
 * Roboter ein bekannte Strecke vorwaerts zu schieben um D zu berechnen.
 *
 * D = r *
 *
 * Hier:
 * L => ANGLE_FACTOR_LEFT
 * R => ANGLE_FACTOR_RIGHT
 * D => WHEELBASE
 */
struct Odometry
{
	static constexpr float WheelBase = 285.2;
	static constexpr float AngleFactorLeft =   1.2971385268961444E-4;
	static constexpr float AngleFactorRight =  1.2975322755503696E-4;
	static constexpr float BowFactorLeft  = (AngleFactorLeft  * WheelBase / 2);
	static constexpr float BowFactorRight = (AngleFactorRight * WheelBase / 2);
};


/// DifferentialDriveController Configuration
/// 
/// This struct contains all settings needed for the differential drive
/// controller template.
struct DifferentialDriveController
{
	struct V
	{
		static constexpr float kp = 3.0f*12.0/20.0;
		static constexpr float ki = 0.1f*12.0/20.0;
		static constexpr float kd = 0.0f;
		static constexpr float maxErrorSum = 1023;
		static constexpr float maxOutput = 700;
	};

	struct Omega
	{
		static constexpr float kp = 4.5*12.0/20.0;
		static constexpr float ki = 4.0f*12.0/20.0;
		static constexpr float kd = 0.0f;
		static constexpr float maxErrorSum = 1023;
		static constexpr float maxOutput = 700;
	};

	struct OmegaWeak
	{
		static constexpr float kp = 0.5*12.0/20.0;
		static constexpr float ki = 0.0f;
		static constexpr float kd = 0.0f;
		static constexpr float maxErrorSum = 1023;
		static constexpr float maxOutput = 700;
	};

	struct OvercurrentFilterLeft
	{
		static constexpr uint8_t maxValue = 30;
		static constexpr uint8_t lowerBound = 20;
		static constexpr uint8_t upperBound = 25;
	};

	struct OvercurrentFilterRight
	{
		static constexpr uint8_t maxValue = 30;
		static constexpr uint8_t lowerBound = 20;
		static constexpr uint8_t upperBound = 25;
	};

	struct OvercurrentFilterLeftSlow
	{
		static constexpr uint16_t maxValue = 500;
		static constexpr uint16_t lowerBound = 200;
		static constexpr uint16_t upperBound = 400;
	};

	struct OvercurrentFilterRightSlow
	{
		static constexpr uint16_t maxValue = 500;
		static constexpr uint16_t lowerBound = 200;
		static constexpr uint16_t upperBound = 400;
	};

	struct DriftWheelsFilterForward
	{
		static constexpr uint16_t maxValue = 500;
		static constexpr uint16_t lowerBound = 100;
		static constexpr uint16_t upperBound = 400;
	};

	struct DriftWheelsFilterAngle
	{
		static constexpr uint16_t maxValue = 500;
		static constexpr uint16_t lowerBound = 100;
		static constexpr uint16_t upperBound = 400;
	};

	struct SpeedTargetRamp
	{
		static constexpr float increment = 0.2f;
		static constexpr float decrement = 0.2f;
		static constexpr float initialValue = 0.0f;
	};

	struct BatteryVoltage
	{
		static constexpr float minimum = 18.0f;
	};

	/// contains odometry values from configuration/odometry.hpp
	struct Odometry
	{
		static constexpr float WheelBase        = ::Configuration::Odometry::WheelBase;
		static constexpr float AngleFactorLeft  = ::Configuration::Odometry::AngleFactorLeft;
		static constexpr float AngleFactorRight = ::Configuration::Odometry::AngleFactorRight;
		static constexpr float BowFactorLeft    = ::Configuration::Odometry::BowFactorLeft;
		static constexpr float BowFactorRight   = ::Configuration::Odometry::BowFactorRight;
	};

	/// diameter of the driving wheels in mm
	static constexpr float WheelDiameter = 44;
	/// gear transmission ratio
	static constexpr float Gear = 6.25 * 20.0/15.0;
	/// encoder steps per motor revolution
	static constexpr uint16_t StepsPerRevolution = 2000;
	/// contains the encoder ticks per mm of motion
	static constexpr float TicksPerMm = (Gear * StepsPerRevolution) / (WheelDiameter * M_PI);
	/// how often the engine code is run per second
	static constexpr float EngineFrequency = 1000;
	/// time between calls to the engine code in seconds
	static constexpr float EngineTimestep  = (1.0 / EngineFrequency);
	/// time between calls to the engine code in milliseconds
	static constexpr uint16_t EngineTimestepMs =
		static_cast<uint16_t>(EngineTimestep * 1000);
};	// struct Drive
}	// namespace Configuration
#endif // CONFIGURATION_HPP
