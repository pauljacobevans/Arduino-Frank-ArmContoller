// ================================================
// == Frank ==  The Arduino Arm Controller!
// ================================================
/*
* This is a arduino-based controller that uses input from an RC reciver to control a variety of servos.
* There is also a servo speed funciton with two variables to adjust the speed of moves.
* We're also looking at adding a positional save funciton, but that's a strech goal. ;)
*
* The servos use microseconds in this script:
* 1000 = Full counter-clockwise
* 1500 = centered
* 2000 = Full clockwise
*
* Note: look up the specs on the servos, as some DO allow outside of these ranges (could be as low as
* ~700 and as high as ~2300.  In fact, the actual PWM frame technically has room for any value between
* 0 - 19,999.  The specific servo documentation should give final clarification on final values.
*
* by www.pauljacobevans.com
*
*/
// ================================================
// == Variable Declarations
// ================================================

// Arduino I/O Pin Declarations
// rcPin[0] = a PWM 3-way toggle switch on the RC controller, rcPon[1] = a momentary push button for the attack.

const int rcPin[2] = { 2, 3 }; // Disengage, Engage, Deploy Arm RC pin  ;;;  Attack Arm  // (RC input pins)
const int servo[4] = { 5, 6, 9, 10 }; // servoDoor ;;; servoBicep  ;;; servoTricep  ;;;  servoWrist  // (PWM servo output pins)

// Servo Final Position

const int attackArm[3] = { 1000, 600, 1200 }; // servoBicep, servoTricep, servoWrist 'default saved' position

// Servo Speed Changes

const bool slowMove[3] = { 0, 0, 0 }; // Choose which moves are throttled [ Engage, Deploy, Attack! ]

const int delayServo = 50;  // servoBicep, servoTricep, servoWrist delay (ms) during movement (between steps)
const int stepsServo = 20;  // servoBicep, servoTricep, servoWrist # of steps to make per move

// =================================================================
// == Shouldn't have to edit anything under this line!
// =================================================================

const int engageArm[4] = { 1500, 1950, 975, 2100 }; // servoDoor, servoBicep, servoTricep, servoWrist 'engaged' position
const int deployArm[4] = { 1500, 1600, 1100, 800 }; // servoDoor, servoBicep, servoTricep, servoWrist 'deployed' position

// =================================================================
// == END OF Varible Declarations
// =================================================================

#include <Servo.h>

Servo servoDoor;
Servo servoBicep;
Servo servoTricep;
Servo servoWrist;

int statusArm = 0;
bool doorOpen = false;
int rcPrep = 0;
int rcFire = 0;
int door = 0;
int bicep = 0;
int tricep = 0;
int armThere[2] = { 0, 0 };

void setup() {
	Serial.begin(9600);
	pinMode(rcPin[0], INPUT);
	pinMode(rcPin[1], INPUT);
	Serial.println("Starting Frank! the Arduino Arm Controller!");
}

void loop() {

	switch (statusArm) {
	case 0:
		servoDoor.detach();
		servoBicep.detach();
		servoTricep.detach();
		servoWrist.detach();
		break;
	case 1:
		if (armThere[0] != 1 || armThere[1] = 0)
		{
			servoDoor.attach(servo[0]);
			servoBicep.attach(servo[1]);
			servoTricep.attach(servo[2]);
			servoWrist.attach(servo[3]);
			if (slowMove[0])
			{
				moveStep(engageArm[1], engageArm[2], engageArm[3], 1);
			}
			else
			{
				moveNow(engageArm[1], engageArm[2], engageArm[3], 1);
			}
			bicep = servoBicep.readMicroseconds(engageArm[1]);
			tricep = servoBicep.readMicroseconds(engageArm[2]);
			if (bicep >= engageArm[1] && tricep <= engageArm[2])
			{
				servoDoor.writeMicroseconds(engageArm[0]);
				door = servoDoor.readMicroseconds;
				if (door <= engageArm[0])
				{
					doorOpen = false;
					armThere[1] = 1;
				}
			}
		}
		break;
	case 2:
		if (armThere[0] != 2 || armThere[1] = 0)
		{
			servoDoor.writeMicroseconds(deployArm[0]);
			door = servoDoor.readMicroseconds;
			if (door >= deployArm[0])
			{
				doorOpen = true;
				if (slowMove[1])
				{
					moveStep(deployArm[1], deployArm[2], deployArm[3], 2);
				}
				else
				{
					moveNow(deployArm[1], deployArm[2], deployArm[3], 2);
				}
				armThere[1] = 1;
			}
		}
		break;
	case 3:
		if (armThere[0] != 3 || armThere[1] = 0)
		{
			if (slowMove[2])
			{
				moveStep(attackArm[0], attackArm[1], attackArm[2], 3);
			}
			else
			{
				moveNow(attackArm[0], attackArm[1], attackArm[2], 3);
			}
			armThere[1] = 1;
		{
		break;
	}

	rcPrep = pulseIn(rcPin[0], HIGH, 90000);
	rcFire = pulseIn(rcPin[1], HIGH, 90000);

	if (rcPrep > 1250)
	{
		if (rcPrep > 1750)
		{
			statusArm = 2;
			if (rcFire > 1750)
			{
				statusArm = 3;
			}
		}
		else
		{
			statusArm = 1;
		}
	}
	else
	{
		statusArm = 0;
	}

}

void moveStep(int x, int y, int z, int v) {
	armThere[0] = v;
	armThere[1] = 0;
	int armNew[3] = { x, y, z };
	int armCurrent[3];
	int armTotal[3];
	float armStep[3];
	int rcCheck[2];

	for (int i = 0; i < 3; i++)
	{
		armCurrent[i] = pulseIn(servo[i + 1], HIGH, 90000);
		armTotal[i] = armNew[i] - armCurrent[i];
		armStep[i] = float(armTotal[i]) / stepsServo;
	}

	for (int j = 0; j < stepsServo; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			armCurrent[k] = armCurrent[k] + armStep[k];
		}
		servoBicep.writeMicroseconds(armCurrent[0]);
		servoTricep.writeMicroseconds(armCurrent[1]);
		servoWrist.writeMicroseconds(armCurrent[2]);

		delay(delayServo);

		rcCheck[0] = pulseIn(rcPin[0], HIGH, 90000);
		rcCheck[1] = pulseIn(rcPin[1], HIGH, 90000);
		
		if (rcPrep > 1250)
		{
			if (rcPrep > 1750)
			{
				if (rcFire < 1750)
				{
					statusArm = 2;
					return;
				}
			}
			else
			{
				statusArm = 1;
				return;
			}
		}
		else
		{
			statusArm = 0;
			return;
		}
	}

	servoBicep.writeMicroseconds(x);
	servoTricep.writeMicroseconds(y);
	servoWrist.writeMicroseconds(z);
}

void moveNow(int x, int y, int z, int v) {
	armThere[0] = v;
	armThere[1] = 0;
	servoBicep.writeMicroseconds(x);
	servoTricep.writeMicroseconds(y);
	servoWrist.writeMicroseconds(z);
}
