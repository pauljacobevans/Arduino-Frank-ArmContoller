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
* ~700 and as high as ~2300.  Servo documentation should give final clarification.
*
* by www.pauljacobevans.com
*
* Version History:
*
* v0.8 - 2018/5/18: Inital Release
* v0.9 - 2018/5/19: Issues still with the speed throttling, but the double-take seems to be fixed now.
*/
// ================================================
// == Variable Declarations
// ================================================

// Arduino I/O Pin Declarations

const int rcPin[2] = { 3, 2 }; // Disengage, Engage, Deploy Arm RC pin  ;;;  Attack Arm  // (RC input pins)
const int servo[4] = { 5, 6, 9, 10 }; // servoDoor ;;; servoBicep  ;;; servoTricep  ;;;  servoWrist  // (PWM servo output pins)

// Servo Final Position

const int attackArm[4] = { 1500, 1000, 600, 1200 }; // servoDoor, servoBicep, servoTricep, servoWrist 'default saved' position

// Servo Speed Changes

const bool slowMove[4] = { 0, 0, 0, 0 }; // Choose which moves are throttled [ Disengage, Engage, Deploy, Attack! ]

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
int rcPrep = 0;
int rcFire = 0;
bool armMoving = false;

void setup() {
	Serial.begin(9600);
	pinMode(rcPin[0], INPUT);
	pinMode(rcPin[1], INPUT);
	Serial.println("Starting Tra$h-03 Arm Controller!");
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
		servoDoor.attach(servo[0]);
		servoBicep.attach(servo[1]);
		servoTricep.attach(servo[2]);
		servoWrist.attach(servo[3]);
		if (slowMove[1])
		{
			moveStep(engageArm[0], engageArm[1], engageArm[2], engageArm[3]);
		}
		else
		{
			moveNow(engageArm[0], engageArm[1], engageArm[2], engageArm[3]);
		}
		break;
	case 2:
		if (slowMove[2])
		{
			moveStep(deployArm[0], deployArm[1], deployArm[2], deployArm[3]);
		}
		else
		{
			moveNow(deployArm[0], deployArm[1], deployArm[2], deployArm[3]);
		}
		break;
	case 3:
		if (slowMove[3])
		{
			moveStep(attackArm[0], attackArm[1], attackArm[2], attackArm[3]);
		}
		else
		{
			moveNow(attackArm[0], attackArm[1], attackArm[2], attackArm[3]);
		}
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

void moveStep(int w, int x, int y, int z) {
	armMoving = true;
	int armNew[4] = { w, x, y, z };
	int armCurrent[4];
	int armTotal[4];
	float armStep[4];
	for (int i = 0; i < 4; i++)
	{
		armCurrent[i] = pulseIn(servo[i], HIGH, 90000);
		armTotal[i] = armNew[i] - armCurrent[i];
		armStep[i] = float(armTotal[i]) / stepsServo;
		Serial.println("Begingin Math Loop");
	}
	for (int j = 0; j < stepsServo; j++)
	{
		int armMove = 0;
		for (int k = 0; k < 4; k++)
		{
			int armNow = pulseIn(servo[k], HIGH, 90000);
			int armRange = armNew[k] - armNow;
			if ((armRange < 10) && (armRange > -10))
			{
				armMove++;
			}
			else
			{
				armCurrent[k] = armCurrent[k] + armStep[k];
			}
		}
		if (armMove < 4)
		{
			servoDoor.writeMicroseconds(armCurrent[0]);
			servoBicep.writeMicroseconds(armCurrent[1]);
			servoTricep.writeMicroseconds(armCurrent[2]);
			servoWrist.writeMicroseconds(armCurrent[3]);
		}
		else
		{
			servoDoor.writeMicroseconds(armNew[0]);
			servoBicep.writeMicroseconds(armNew[1]);
			servoTricep.writeMicroseconds(armNew[2]);
			servoWrist.writeMicroseconds(armNew[3]);
			Serial.println("Last Servo Move");
		}
		Serial.println("Step Loop");
		delay(delayServo);
	}
	armMoving = false;
}

void moveNow(int w, int x, int y, int z) {
	servoDoor.writeMicroseconds(w);
	servoBicep.writeMicroseconds(x);
	servoTricep.writeMicroseconds(y);
	servoWrist.writeMicroseconds(z);
}