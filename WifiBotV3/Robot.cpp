#include "Robot.h"

Robot::Robot()
{
	leftSpeed = 0;
	rightSpeed = 0;
}

int Robot::getLeftSpeed()
{
	return leftSpeed;
}

int Robot::getRightSpeed()
{
	return rightSpeed;
}

void Robot::setLeftSpeed(int lSpeed)
{
	if(lSpeed > -241 && lSpeed < 241)
		leftSpeed = lSpeed;
}
		
void Robot::setRightSpeed(int rSpeed)
{
	if(rSpeed > -241 && rSpeed < 241)
		rightSpeed = rSpeed;
}

void Robot::setSpeed(int l, int r)
{
	setLeftSpeed(l);
	setRightSpeed(r);
}