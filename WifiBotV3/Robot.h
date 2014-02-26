#ifndef DEF_ROBOT
#define DEF_ROBOT
#include "stdafx.h"

class Robot
{
	public:
		Robot();
		int getLeftSpeed();
		int getRightSpeed();
		void setLeftSpeed(int lSpeed);
		void setRightSpeed(int rSpeed);
		void setSpeed(int l, int r);
		void proceedSpeed();
		void setKeyUp(bool up);
		void setKeyDown(bool down);
		void setKeyLeft(bool left);
		void setKeyRight(bool right);

	private:
		bool keyUp;
		bool keyDown;
		bool keyLeft;
		bool keyRight;
		int speed;
		int leftSpeed;
		int rightSpeed;
};

#endif