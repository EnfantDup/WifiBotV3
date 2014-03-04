#ifndef DEF_ROBOT
#define DEF_ROBOT
#include "stdafx.h"

class Robot
{
	public:
		Robot();
		int getLeftSpeed();
		int getRightSpeed();
		bool getSimulateur();
		void setLeftSpeed(int lSpeed);
		void setRightSpeed(int rSpeed);
		void setSpeed(int s);
		void proceedSpeed();
		void setKeyUp(bool up);
		void setKeyDown(bool down);
		void setKeyLeft(bool left);
		void setKeyRight(bool right);
		void setSimulateur(bool b);

	private:
		bool keyUp;
		bool keyDown;
		bool keyLeft;
		bool keyRight;
		bool simulateur;
		int speed;
		int leftSpeed;
		int rightSpeed;
};

#endif