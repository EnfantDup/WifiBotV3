#include "stdafx.h"
#include "Robot.h"

Robot::Robot()
{
	speed = 120;
	leftSpeed = 0;
	rightSpeed = 0;

	keyUp = false;
	keyDown = false;
	keyLeft = false;
	keyRight = false;
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

void Robot::proceedSpeed()
{
	if(keyUp && keyLeft)
	{
		leftSpeed = 0.5*speed;
		rightSpeed = speed;
	}
	else if(keyUp && keyRight)
	{
		leftSpeed = speed;
		rightSpeed = 0.5*speed;
	}
	else if(keyDown && keyLeft)
	{
		leftSpeed = -0.5*speed;
		rightSpeed = -speed;
	}
	else if(keyDown && keyRight)
	{
		leftSpeed = -speed;
		rightSpeed = -0.5*speed;
	}
	else if(keyUp)
	{
		leftSpeed = speed;
		rightSpeed = speed;
	}
	else if(keyDown)
	{
		leftSpeed = -speed;
		rightSpeed = -speed;
	}
	else if(keyLeft)
	{
		leftSpeed = -speed;
		rightSpeed = speed;
	}
	else if(keyRight)
	{
		leftSpeed = speed;
		rightSpeed = -speed;
	}
	else
	{
		leftSpeed = 0;
		rightSpeed = 0;
	}
}

void Robot::setKeyUp(bool up)
{
	keyUp = up;
}

void Robot::setKeyDown(bool down)
{
	keyDown = down;
}

void Robot::setKeyLeft(bool left)
{
	keyLeft = left;
}

void Robot::setKeyRight(bool right)
{
	keyRight = right;
}